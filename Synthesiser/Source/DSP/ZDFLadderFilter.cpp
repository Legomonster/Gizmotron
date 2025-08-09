/*
  ==============================================================================

    ZDFLadderFilter.cpp
    Created: 9 Aug 2025 6:45:10am
    Author:  Jules

  ==============================================================================
*/

#include "ZDFLadderFilter.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SynthDSP
{
    void ZDFLadderFilter::prepare(double sr, float smoothTimeMs)
    {
        sampleRate = (sr > 1.0 ? sr : 44100.0);
        // one-pole smoothing coef
        const float tau = std::max(0.1f, smoothTimeMs) * 1e-3f;
        smoothCoef = std::exp(-1.0f / (float(sampleRate) * tau));
        invOneMinusSmooth = 1.0f - smoothCoef;
        reset();
    }

    // c = normalized cutoff [0..1], r = resonance [0..1], d = drive [0..1]
    void ZDFLadderFilter::set(float c, float r, float d)
    {
        cutoffNormTarget = std::clamp(c, 0.0f, 1.0f);
        resonanceTarget = std::clamp(r, 0.0f, 1.2f);
        driveTarget = std::clamp(d, 0.0f, 1.0f);
    }

    void ZDFLadderFilter::reset()
    {
        s1 = s2 = s3 = s4 = 0.0f;
        cutoffNormSm = cutoffNormTarget;
        resonanceSm = resonanceTarget;
        driveSm = driveTarget;
    }

    // Expo map: 0..1 -> [minHz .. 0.45*fs]
    float ZDFLadderFilter::mapNormToCutoffExp(float norm, float fs, float minHz = 20.0f)
    {
        norm = std::clamp(norm, 0.0f, 1.0f);
        const float top = 0.40f * fs;               // safer than Nyquist
        return minHz * std::pow(top / std::max(minHz, 1.0f), norm);
    }

    float ZDFLadderFilter::processSample(float x)
    {
        // --- smooth params ---
        cutoffNormSm += invOneMinusSmooth * (cutoffNormTarget - cutoffNormSm);
        resonanceSm += invOneMinusSmooth * (resonanceTarget - resonanceSm);
        driveSm += invOneMinusSmooth * (driveTarget - driveSm);

        // --- coefficients ---
        const float fs = (float)sampleRate;
        const float fc = mapNormToCutoffExp(cutoffNormSm, fs, 20.0f);
        const float g = std::tan((float)M_PI * (fc / fs));   // TPT prewarp
        const float G = g / (1.0f + g);

        // Slightly stronger resonance frequency-comp (empirical but robust)
        const float r = std::clamp(resonanceSm, 0.0f, 1.0f);
        const float k = 4.0f * (r * (1.0f - 0.6f * G + 0.3f * G * G)); // was 1 - 0.5G + 0.25G^2

        // Softer drive (optional)
        const float driveGain = 1.0f + 5.0f * driveSm; // was 6 or 9

        // --- Frozen-state linearization pieces (A, B, S) as before ---
        const float A = G * G * G * G;
        const float B = (1.0f - G) * (G * G * G * s1 + G * G * s2 + G * s3 + s4);
        const float S = s4 + G * (s3 + G * (s2 + G * s1));

        // Linear seed stays (good fallback)
        const float alpha0 = 1.0f / (1.0f + k * A);
        const float u_lin = alpha0 * (x - k * S);

        // ***** NEW: warm start Newton from previous solution *****
        float u = uPrev;

        // If we’re far off (first call, or parameter jump), fall back to a better seed
        if (!std::isfinite(u) || std::fabs(u - u_lin) > 2.0f) {
            const float y4_lin = A * u_lin + B;
            u = std::tanh(driveGain * (x - k * y4_lin));
        }

        // Newton – unchanged math
        for (int i = 0; i < 2; ++i) {
            const float y4u = A * u + B;
            const float z = driveGain * (x - k * y4u);
            const float t = std::tanh(z);
            const float sech2 = 1.0f - t * t;

            const float f = u - t;
            const float df = 1.0f + driveGain * k * A * sech2;

            u -= f / (df + 1e-12f);
        }

        // Store for next sample
        uPrev = u;

        // Integrators as before, but **no state clamps** in normal use
        float v1 = (u - s1) * G; float y1 = v1 + s1; s1 = y1 + v1;
        float v2 = (y1 - s2) * G; float y2 = v2 + s2; s2 = y2 + v2;
        float v3 = (y2 - s3) * G; float y3 = v3 + s3; s3 = y3 + v3;
        float v4 = (y3 - s4) * G; float y4 = v4 + s4; s4 = y4 + v4;

        // NaN/Inf guard (prevents one bad step from killing the voice)
        if (!std::isfinite(y4)) { s1 = s2 = s3 = s4 = 0.0f; uPrev = 0.0f; return 0.0f; }

        return y4;
    }

//ZDFLadderFilter::ZDFLadderFilter()
//    : sampleRate(44100.0), cutoff(1000.0f), resonance(0.5f), drive(0.2f),
//      z1(0.0f), z2(0.0f), z3(0.0f), z4(0.0f)
//{
//}
//
//void ZDFLadderFilter::prepare(double sr)
//{
//    sampleRate = sr;
//    reset();
//}
//
//void ZDFLadderFilter::set(float c, float r, float d)
//{
//    cutoff = mapNormToCutoffExp(c,sampleRate,
//         20.0f);
//    resonance = r;
//    drive = d;
//}

//void ZDFLadderFilter::reset()
//{
//    z1 = z2 = z3 = z4 = 0.0f;
//}
//
//float ZDFLadderFilter::mapNormToCutoffExp(float norm, float sampleRate,
//    float minHz)
//{
//    norm = std::clamp(norm, 0.0f, 1.0f);
//
//    // Map 0..1 -> minHz..Nyquist exponentially
//    const float nyquist = sampleRate * 0.5f;
//    return minHz * std::pow(nyquist / minHz, norm);
//}
//
//float ZDFLadderFilter::processSample(float x)
//{
//    // --- Pre-warp / integrator coefficient (TPT) ---
//    const float fs = (float)sampleRate;
//    const float fc = std::max(1e-3f, std::min(cutoff, 0.49f * fs));       // guard Nyquist
//    const float g = std::tan((float)M_PI * (fc / fs));
//    const float G = g / (1.0f + g);
//
//    // --- Feedback amount ---
//    const float k = 4.0f * std::clamp(resonance, 0.0f, 1.25f);            // allow a touch past 1.0 if you like
//
//    // --- Input drive mapping ---
//    // Map drive 0..1 -> ~1x .. ~10x; tweak to taste
//    const float driveGain = 1.0f + 9.0f * std::max(0.0f, drive);
//
//    // --- Useful powers / constants ---
//    const float G2 = G * G, G3 = G2 * G, G4 = G2 * G2;
//
//    // dy4/du for this TPT ladder with current (frozen) states:
//    // y4(u) = A*u + B, with A = G^4 and B from the stored states (below).
//    const float A = G4;
//
//    // Two different state combos we need:
//    // (1) For the linear initial guess (no tanh), the classic sum:
//    //     S = z4 + G*z3 + G^2*z2 + G^3*z1
//    const float S = z4 + G * (z3 + G * (z2 + G * z1));
//
//    // (2) For Newton, we need B in y4(u) = A*u + B with *frozen* states:
//    //     y1 = G*u + (1-G)*z1
//    //     y2 = G*y1 + (1-G)*z2 = G^2*u + (1-G)*(G*z1 + z2)
//    //     y3 = G*y2 + (1-G)*z3 = G^3*u + (1-G)*(G^2*z1 + G*z2 + z3)
//    //     y4 = G*y3 + (1-G)*z4 = G^4*u + (1-G)*(G^3*z1 + G^2*z2 + G*z3 + z4)
//    const float B = (1.0f - G) * (G3 * z1 + G2 * z2 + G * z3 + z4);
//
//    // Linear alpha0 for a good initial guess
//    // Linear case solves: u = (x - k*B_lin) / (1 + k*A)
//    // but the classic form uses S instead of B; both are fine. Use S for a robust seed:
//    const float alpha0 = 1.0f / (1.0f + k * A);
//    const float u_lin = alpha0 * (x - k * S);
//
//    // Seed Newton with a “one-shot” nonlinearity pass
//    // Estimate y4 from linear u, then apply tanh once.
//    const float y4_lin = A * u_lin + B;
//    float u = std::tanh(driveGain * (x - k * y4_lin));
//
//    // --- Newton-Raphson to solve u = tanh(driveGain * (x - k*(A*u + B))) ---
//    // f(u)  = u - tanh( driveGain * (x - k*(A*u + B)) ) = 0
//    // f'(u) = 1 + driveGain * k * A * sech^2( driveGain * (x - k*(A*u + B)) )
//    // Two iterations are plenty for audio:
//    for (int i = 0; i < 2; ++i)
//    {
//        const float y4u = A * u + B;
//        const float z = driveGain * (x - k * y4u);
//        const float t = std::tanh(z);
//        const float sech2 = 1.0f - t * t; // tanh' = sech^2
//
//        const float f = u - t;
//        const float df = 1.0f + driveGain * k * A * sech2;
//
//        // Safe-guard df
//        const float inv_df = 1.0f / (df + 1e-12f);
//        u -= f * inv_df;
//    }
//
//    // --- Now run the four TPT integrators with the solved u ---
//    float v1 = (u - z1) * G; float y1 = v1 + z1; z1 = y1 + v1;
//    float v2 = (y1 - z2) * G; float y2 = v2 + z2; z2 = y2 + v2;
//    float v3 = (y2 - z3) * G; float y3 = v3 + z3; z3 = y3 + v3;
//    float v4 = (y3 - z4) * G; float y4 = v4 + z4; z4 = y4 + v4;
//
//    // Optional simple output gain-comp to keep level roughly even at low resonance
//    // const float comp = 1.0f + 0.5f*k;  // tweak/disable as desired
//    // return y4 / comp;
//
//    return y4;
//}

} // namespace SynthDSP
