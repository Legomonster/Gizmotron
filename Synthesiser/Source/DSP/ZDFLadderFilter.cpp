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

ZDFLadderFilter::ZDFLadderFilter()
    : sampleRate(44100.0),
      cutoff(1000.0f),
      resonance(0.5f),
      drive(0.2f)
{
    reset();
}

void ZDFLadderFilter::prepare(double sr)
{
    sampleRate = sr;
    reset();
}

void ZDFLadderFilter::set(float c, float r, float d)
{
    cutoff = c;
    resonance = r;
    drive = d;
}

void ZDFLadderFilter::reset()
{
    z1 = 0.0f;
    z2 = 0.0f;
    z3 = 0.0f;
    z4 = 0.0f;
}

float ZDFLadderFilter::processSample(float x)
{
    const float g = std::tan(M_PI * std::min(0.49f, cutoff / (float)sampleRate));
    const float k = 4.0f * resonance;

    // Input nonlinearity
    float u = std::tanh((x - z4 * k) * (1.0f + 3.0f * drive));

    // 4 cascaded one-pole (TPT integrators)
    const float v1 = (u - z1) * g / (1.0f + g);
    const float y1 = v1 + z1;
    z1 = y1 + v1;

    const float v2 = (y1 - z2) * g / (1.0f + g);
    const float y2 = v2 + z2;
    z2 = y2 + v2;

    const float v3 = (y2 - z3) * g / (1.0f + g);
    const float y3 = v3 + z3;
    z3 = y3 + v3;

    const float v4 = (y3 - z4) * g / (1.0f + g);
    const float y4 = v4 + z4;
    z4 = y4 + v4;

    return y4;
}

} // namespace SynthDSP
