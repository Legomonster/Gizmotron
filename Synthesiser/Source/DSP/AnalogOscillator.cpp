/*
  ==============================================================================

    AnalogOscillator.cpp
    Created: 9 Aug 2025 6:45:23am
    Author:  Jules

  ==============================================================================
*/

#include "AnalogOscillator.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SynthDSP
{

// Helper from JS
static float wrap01(float x) { return x - std::floor(x); }

AnalogOscillator::AnalogOscillator(uint32_t seed) : prng(seed)
{
    // This logic is from the 'makeOsc' part of the JS code
    cal.freqCent = prng.bipolar() * 1.5f;
    cal.pwmBias = prng.bipolar() * 0.02f;
    cal.driveSkew = 0.9f + 0.2f * prng.next(); // prng.next() is 0-1, JS Math.random() is 0-1

    // Initialize other random states
    h1 = prng.next();
    h2 = prng.next();
    wowPhase = prng.next();
    phase = prng.next();
}

void AnalogOscillator::prepare(double sampleRate)
{
    sr = sampleRate;
}

float AnalogOscillator::polyBLEP(float t, float dt)
{
    if (t < dt) { t /= dt; return t + t - t * t - 1.0f; }
    if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
    return 0.0f;
}

float AnalogOscillator::adaaTanh(float x, float& xp)
{
    const float dx = x - xp;
    if (std::abs(dx) > 1e-6f) {
        return (std::log(std::cosh(x)) - std::log(std::cosh(xp))) / dx;
    }
    else {
        return std::tanh(0.5f * (x + xp));
    }
}


float AnalogOscillator::process(float baseHz, float pwmParam, const OscParams& params)
{
    // noise floor
    const float floor = 1e-5f * prng.bipolar();

    driftCents += prng.bipolar() * params.drift * 0.0006f;
    driftCents *= 0.9998f;

    wowPhase = wrap01(wowPhase + params.wowRate / (float)sr);
    const float wowCents = std::sin(2.0f * M_PI * wowPhase) * params.wowDepth;

    const float w1 = prng.bipolar();
    const float centsPink = pinkF.process(w1) * params.freqPink;
    const float centsBrown = brownF.process(w1) * params.freqBrown;

    // failing cap RC slosh
    const float rc = 0.9995f;
    const float rawCents = driftCents + wowCents + centsPink + centsBrown + cal.freqCent;
    rcCents = rc * rcCents + (1.0f - rc) * rawCents;
    const float centsTotal = std::max(-4800.0f, std::min(4800.0f, rcCents * params.capHealth));
    const float centScale = std::pow(2.0f, centsTotal / 1200.0f);

    // hum ripple
    h1 = std::fmod(h1 + params.humHz / (float)sr, 1.0f);
    h2 = std::fmod(h2 + 2.0f * params.humHz / (float)sr, 1.0f);
    const float hum = params.humAmt * (0.7f * std::sin(2.0f * M_PI * h1) + 0.3f * std::sin(2.0f * M_PI * h2));

    float phInc = (baseHz * centScale) / (float)sr;
    phInc *= (1.0f + hum);
    phInc = std::max(1e-6f, std::min(0.5f, phInc));
    phInc += phInc * std::min(0.25f, params.jitter) * prng.bipolar();

    // PWM noise & smoothing
    const float w2 = prng.bipolar();
    const float pwmNoise = pinkP.process(w2) * params.pwmPink + brownP.process(w2) * params.pwmBrown;
    const float pwmTarget = std::min(0.95f, std::max(0.05f, params.pwm + pwmParam + cal.pwmBias + pwmNoise));
    pwmState += (pwmTarget - pwmState) * 0.0015f;
    const float duty = std::min(0.95f, std::max(0.05f, pwmState));

    // advance phase
    phase += phInc;
    if (phase >= 1.0f) { phase -= 1.0f; phase += 0.0005f * prng.bipolar(); }
    const float t = phase;
    const float dt = phInc;
    const float dtJ = std::max(1e-6f, dt * (1.0f + params.edgeJitter * prng.bipolar()));

    float v = 0.0f;
    if (params.wave == 0) { // saw
        v = 2.0f * t - 1.0f;
        v -= polyBLEP(t, dtJ);
    }
    else if (params.wave == 1) { // square
        float sq = (t < duty) ? 1.0f : -1.0f;
        sq += polyBLEP(t, dtJ);
        float tf = t - duty; tf -= std::floor(tf);
        sq -= polyBLEP(tf, dtJ);
        float ac = sq - (2.0f * duty - 1.0f);
        if (params.compSlew <= 0.0f) {
            compState = ac;
            v = ac;
        }
        else {
            const float alpha = 1.0f - std::exp(-1.0f / ((float)sr * params.compSlew));
            compState += (ac - compState) * alpha;
            v = compState;
        }
    }
    else { // triangle
        float sq = (t < 0.5f) ? 1.0f : -1.0f;
        sq += polyBLEP(t, dtJ);
        float th = t - 0.5f; th -= std::floor(th);
        sq -= polyBLEP(th, dtJ);
        const float g = std::min(0.25f, dt * 0.5f);
        tri += g * (sq - tri);
        v = tri * 2.0f;
        v = std::tanh(v * 1.6f) / std::tanh(1.6f);
    }

    // DC blocker
    const float R = 0.995f;
    const float yhp = v - dc_x1 + R * dc_y1;
    dc_x1 = v;
    dc_y1 = yhp;

    // ADAA tanh drive
    const float k = (1.0f + 9.0f * params.drive) * cal.driveSkew;
    float y;
    if (params.os2x) {
        const float vmid = 0.5f * (vPrev + yhp);
        const float y1 = adaaTanh(k * vmid, drivePrev);
        drivePrev = k * vmid;
        const float y2 = adaaTanh(k * yhp, drivePrev);
        drivePrev = k * yhp;
        y = 0.5f * (y1 + y2);
        vPrev = yhp;
    }
    else {
        y = adaaTanh(k * yhp, drivePrev);
        drivePrev = k * yhp;
        vPrev = yhp;
    }

    // tiny floor + amp wander
    ampW += prng.bipolar() * 0.00002f;
    ampW *= 0.99995f;

    return y * (1.0f + 0.02f * ampW) + floor;
}

} // namespace SynthDSP
