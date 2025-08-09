/*
  ==============================================================================

    ADSR.h
    Created: 9 Aug 2025 6:44:55am
    Author:  Jules

  ==============================================================================
*/

#pragma once

namespace SynthDSP
{

class ADSR
{
public:
    ADSR();

    void set(float attack, float decay, float sustain, float release);
    void noteOn(float velocity = 1.0f);
    void noteOff();
    float process(float sampleRate);

    // Gets the current envelope state
    bool isActive() const { return state != State::Idle; }

private:
    enum class State {
        Idle,
        Attack,
        Decay,
        Sustain, // Note: JS version doesn't have a sustain state, it just holds in decay. C++ will be the same.
        Release
    };

    State state;
    float attackTime, decayTime, sustainLevel, releaseTime;
    float output;
    float velocity;
};

} // namespace SynthDSP
