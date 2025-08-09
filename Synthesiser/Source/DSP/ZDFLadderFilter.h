/*
  ==============================================================================

    ZDFLadderFilter.h
    Created: 9 Aug 2025 6:45:05am
    Author:  Jules

  ==============================================================================
*/

#pragma once

namespace SynthDSP
{

class ZDFLadderFilter
{
public:
    ZDFLadderFilter();

    void prepare(double sampleRate);
    void set(float cutoff, float resonance, float drive);
    void reset();
    float processSample(float x);

private:
    double sampleRate;
    float cutoff, resonance, drive;
    float z1, z2, z3, z4; // state
};

} // namespace SynthDSP
