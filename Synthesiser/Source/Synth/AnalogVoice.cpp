/*
  ==============================================================================

    AnalogVoice.cpp
    Created: 9 Aug 2025 6:45:35am
    Author:  Jules

  ==============================================================================
*/

#include "AnalogVoice.h"
#include "../PluginProcessor.h" // To get parameter IDs

// A helper to get parameter values safely
static float getParamValue(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    auto* param = apvts.getParameter(paramID);
    return param ? param->getValue() : 0.0f;
}

AnalogVoice::AnalogVoice(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts), oscA(1234567), oscB(9876543)
{
}

void AnalogVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    oscA.prepare(spec.sampleRate);
    oscB.prepare(spec.sampleRate);
    filt.prepare(spec.sampleRate);
}

bool AnalogVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<AnalogSound*>(sound) != nullptr;
}

void AnalogVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    currentHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    ampEnv.noteOn(velocity);
    filEnv.noteOn(1.0f);

    lastA = 0.0f;
    lastB = 0.0f;
}

void AnalogVoice::stopNote(float velocity, bool allowTailOff)
{
    ampEnv.noteOff();
    filEnv.noteOff();

    if (!allowTailOff)
    {
        clearCurrentNote();
    }
}

void AnalogVoice::pitchWheelMoved(int newPitchWheelValue) {}
void AnalogVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

void AnalogVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    // Get all parameter values from APVTS at the start of the block
    // Shared Osc Params
    SynthDSP::OscParams oscParams;
    oscParams.drive = getParamValue(apvts, ParamIDs::drive);
    oscParams.drift = getParamValue(apvts, ParamIDs::drift);
    oscParams.wowDepth = getParamValue(apvts, ParamIDs::wowDepth);
    oscParams.wowRate = getParamValue(apvts, ParamIDs::wowRate);
    oscParams.jitter = getParamValue(apvts, ParamIDs::jitter);
    oscParams.edgeJitter = getParamValue(apvts, ParamIDs::edgeJitter);
    oscParams.pwm = getParamValue(apvts, ParamIDs::pwm);
    oscParams.compSlew = getParamValue(apvts, ParamIDs::compSlew);
    oscParams.freqPink = getParamValue(apvts, ParamIDs::freqPink);
    oscParams.freqBrown = getParamValue(apvts, ParamIDs::freqBrown);
    oscParams.pwmPink = getParamValue(apvts, ParamIDs::pwmPink);
    oscParams.pwmBrown = getParamValue(apvts, ParamIDs::pwmBrown);
    oscParams.capHealth = getParamValue(apvts, ParamIDs::capHealth);
    oscParams.humAmt = getParamValue(apvts, ParamIDs::humAmt);
    oscParams.humHz = getParamValue(apvts, ParamIDs::humHz);
    oscParams.os2x = getParamValue(apvts, ParamIDs::os2x) >= 0.5f;

    // Waveforms (assuming these are choice parameters 0, 1, 2)
    oscParams.wave = (int)getParamValue(apvts, ParamIDs::waveA);
    auto oscBParams = oscParams;
    oscBParams.wave = (int)getParamValue(apvts, ParamIDs::waveB);

    // Envelopes
    ampEnv.set(getParamValue(apvts, ParamIDs::ampA), getParamValue(apvts, ParamIDs::ampD), getParamValue(apvts, ParamIDs::ampS), getParamValue(apvts, ParamIDs::ampR));
    filEnv.set(getParamValue(apvts, ParamIDs::filA), getParamValue(apvts, ParamIDs::filD), getParamValue(apvts, ParamIDs::filS), getParamValue(apvts, ParamIDs::filR));

    // Filter
    const float baseCut = getParamValue(apvts, ParamIDs::cutoff);
    const float res = getParamValue(apvts, ParamIDs::res);
    const float fdrive = getParamValue(apvts, ParamIDs::filterDrive);
    const float fEnvAmt = getParamValue(apvts, ParamIDs::filterEnvAmt);

    // Mix & FM
    const float m_mixA = getParamValue(apvts, ParamIDs::mixA);
    const float m_mixB = getParamValue(apvts, ParamIDs::mixB);
    const float m_detuneB = getParamValue(apvts, ParamIDs::detuneB);
    const float m_fmAB = getParamValue(apvts, ParamIDs::fmAB);
    const float m_fmBA = getParamValue(apvts, ParamIDs::fmBA);
    const float m_amp = getParamValue(apvts, ParamIDs::amp);

    // The main processing loop
    for (int i = startSample; i < startSample + numSamples; ++i)
    {
        // Calculate one sample of the voice's output
        const float aEnv = ampEnv.process(getSampleRate());
        const float fEnv = filEnv.process(getSampleRate());

        const float hzA = std::max(0.0f, currentHz + m_fmBA * lastB);
        const float detuneMultiplier = std::pow(2.0f, m_detuneB / 1200.0f);
        const float hzB = std::max(0.0f, currentHz * detuneMultiplier + m_fmAB * lastA);

        const float sA = oscA.process(hzA, 0, oscParams);
        const float sB = oscB.process(hzB, 0, oscBParams);

        lastA = sA;
        lastB = sB;

        float mix = sA * m_mixA + sB * m_mixB;

        const float modCut = std::max(40.0f, std::min(16000.0f, baseCut * std::pow(2.0f, fEnvAmt * fEnv)));
        filt.set(modCut, res, fdrive);
        const float y = filt.processSample(mix);

        const float outputSample = y * m_amp * aEnv;

        // Write the calculated sample to all channels in the output buffer
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.getWritePointer(channel)[i] += outputSample;
        }

        // If the amp envelope is finished, the voice is no longer active
        if (!ampEnv.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
