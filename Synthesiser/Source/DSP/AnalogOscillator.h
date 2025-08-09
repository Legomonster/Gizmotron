/*
  ==============================================================================

    AnalogOscillator.h
    Created: 9 Aug 2025 6:45:17am
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include "NoiseGenerators.h"
#include <vector>


namespace SynthDSP
{

// A collection of all parameters that can be modulated per-oscillator
struct OscParams
{
    float drive = 0.35f;
    float drift = 4.0f;
    float wowDepth = 6.0f;
    float wowRate = 0.6f;
    float jitter = 0.002f;
    float edgeJitter = 0.003f;
    float pwm = 0.5f;
    float compSlew = 0.0003f;
    float freqPink = 2.0f;
    float freqBrown = 4.0f;
    float pwmPink = 0.01f;
    float pwmBrown = 0.02f;
    float capHealth = 1.0f;
    float humAmt = 0.001f;
    float humHz = 50.0f;
    bool os2x = true;
    int wave = 0; // 0: Saw, 1: Square, 2: Triangle
};

class AnalogOscillator
{
public:
    AnalogOscillator(uint32_t seed);

    void prepare(double sampleRate);
    float process(float baseHz, float pwmParam, const OscParams& params);

private:
    float adaaTanh(float x, float& xp);
    float polyBLEP(float t, float dt);

    double sr;

    // Per-oscillator state
    PRNG prng;
    Pink pinkF, pinkP;
    Brown brownF, brownP;

    float driftCents = 0.0f;
    float wowPhase = 0.0f;
    float phase = 0.0f;
    float tri = 0.0f;
    float compState = 0.5f;
    float dc_x1 = 0.0f, dc_y1 = 0.0f;
    float drivePrev = 0.0f, vPrev = 0.0f;
    float rcCents = 0.0f;
    float h1 = 0.0f, h2 = 0.0f;
    float pwmState = 0.5f;
    float ampW = 0.0f;
    

    struct Calibration {
        float freqCent;
        float pwmBias;
        float driveSkew;
    } cal;
};

} // namespace SynthDSP
