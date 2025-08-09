/*
  ==============================================================================

    NoiseGenerators.h
    Created: 9 Aug 2025 6:44:42am
    Author:  Jules

  ==============================================================================
*/

#pragma once
#include <cstdint>

namespace SynthDSP
{

// A simple Linear Congruential Generator PRNG.
class PRNG
{
public:
    PRNG(uint32_t seed = 22222) : s(seed) {}

    // Returns a random float between 0.0 and 1.0
    float next()
    {
        s = (1664525 * s + 1013904223);
        return (static_cast<float>(s >> 8) / 16777216.0f);
    }

    // Returns a random float between -1.0 and 1.0
    float bipolar()
    {
        return next() * 2.0f - 1.0f;
    }

private:
    uint32_t s;
};

// Voss-McCartney pink noise generator
class Pink
{
public:
    Pink() : b0(0.0f), b1(0.0f), b2(0.0f) {}

    float process(float white)
    {
        b0 = 0.99765f * b0 + white * 0.0990460f;
        b1 = 0.96300f * b1 + white * 0.2965164f;
        b2 = 0.57000f * b2 + white * 1.0526913f;
        return (b0 + b1 + b2 + white * 0.1848f) * 0.05f;
    }

private:
    float b0, b1, b2;
};

// Simple brown noise generator (leaky integrator)
class Brown
{
public:
    Brown() : y(0.0f) {}

    float process(float white)
    {
        y = (y + white * 0.02f) * 0.995f;
        return y;
    }

private:
    float y;
};

} // namespace SynthDSP
