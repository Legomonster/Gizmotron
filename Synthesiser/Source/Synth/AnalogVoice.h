/*
  ==============================================================================

    AnalogVoice.h
    Created: 9 Aug 2025 6:45:30am
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AnalogSound.h"
#include "../DSP/AnalogOscillator.h"
#include "../DSP/ADSR.h"
#include "../DSP/ZDFLadderFilter.h"

//==============================================================================
class AnalogVoice : public juce::SynthesiserVoice
{
public:
    AnalogVoice(juce::AudioProcessorValueTreeState& apvts);

    void prepare(const juce::dsp::ProcessSpec& spec);

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    void updateParameters(const juce::dsp::ProcessSpec& spec);
    SynthDSP::OscParams getOscParams(const juce::String& oscId);

    juce::AudioProcessorValueTreeState& apvts;

    SynthDSP::AnalogOscillator oscA;
    SynthDSP::AnalogOscillator oscB;
    SynthDSP::ADSR ampEnv;
    SynthDSP::ADSR filEnv;
    SynthDSP::ZDFLadderFilter filt;

    // Voice-level state
    float currentHz = 0.0f;
    float lastA = 0.0f, lastB = 0.0f;

    // Parameter caches
    // These will be updated from the APVTS at the start of each block
    // to avoid repeated lookups in the audio thread.
    float mixA, mixB, detuneB, fmAB, fmBA;
    int waveA, waveB;
    float amp, filterEnvAmt;
};
