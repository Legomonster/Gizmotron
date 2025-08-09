/*
  ==============================================================================

    ADSR.cpp
    Created: 9 Aug 2025 6:44:59am
    Author:  Jules

  ==============================================================================
*/

#include "ADSR.h"
#include <cmath>
#include <algorithm>

namespace SynthDSP
{

// Corrected State enum to match JS logic
enum class ADSR::State {
    Idle,
    Attack,
    Decay,
    Release
};

ADSR::ADSR()
    : state(State::Idle),
      attackTime(0.01f),
      decayTime(0.1f),
      sustainLevel(0.5f),
      releaseTime(0.2f),
      output(0.0f),
      velocity(1.0f)
{
}

void ADSR::set(float attack, float decay, float sustain, float release)
{
    attackTime = attack;
    decayTime = decay;
    sustainLevel = sustain;
    releaseTime = release;
}

void ADSR::noteOn(float vel)
{
    state = State::Attack;
    velocity = vel;
}

void ADSR::noteOff()
{
    if (state != State::Idle)
        state = State::Release;
}

float ADSR::process(float sampleRate)
{
    const float dt = 1.0f / sampleRate;

    switch (state)
    {
    case State::Attack:
    {
        const float k = std::exp(-dt / std::max(1e-6f, attackTime));
        output = 1.0f + (output - 1.0f) * k;
        if (output > 0.999f)
        {
            output = 1.0f;
            state = State::Decay;
        }
        break;
    }
    case State::Decay:
    {
        const float k = std::exp(-dt / std::max(1e-6f, decayTime));
        output = sustainLevel + (output - sustainLevel) * k;
        break;
    }
    case State::Release:
    {
        const float k = std::exp(-dt / std::max(1e-6f, releaseTime));
        output = 0.0f + (output - 0.0f) * k;
        if (output < 1e-5f)
        {
            output = 0.0f;
            state = State::Idle;
        }
        break;
    }
    case State::Idle:
    default:
        output = 0.0f;
        break;
    }

    return output * velocity;
}

} // namespace SynthDSP
