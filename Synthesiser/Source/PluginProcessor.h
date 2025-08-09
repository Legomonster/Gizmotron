#pragma once

#include <JuceHeader.h>

namespace ParamIDs
{
    const char* const drive = "drive";
    const char* const amp = "amp";
    const char* const drift = "drift";
    const char* const wowDepth = "wowDepth";
    const char* const wowRate = "wowRate";
    const char* const jitter = "jitter";
    const char* const edgeJitter = "edgeJitter";
    const char* const pwm = "pwm";
    const char* const compSlew = "compSlew";
    const char* const freqPink = "freqPink";
    const char* const freqBrown = "freqBrown";
    const char* const pwmPink = "pwmPink";
    const char* const pwmBrown = "pwmBrown";
    const char* const capHealth = "capHealth";
    const char* const humAmt = "humAmt";
    const char* const humHz = "humHz";
    const char* const os2x = "os2x";
    const char* const cutoff = "cutoff";
    const char* const res = "res";
    const char* const filterDrive = "filterDrive";
    const char* const filterEnvAmt = "filterEnvAmt";
    const char* const ampA = "ampA";
    const char* const ampD = "ampD";
    const char* const ampS = "ampS";
    const char* const ampR = "ampR";
    const char* const filA = "filA";
    const char* const filD = "filD";
    const char* const filS = "filS";
    const char* const filR = "filR";
    const char* const mixA = "mixA";
    const char* const mixB = "mixB";
    const char* const detuneB = "detuneB";
    const char* const fmAB = "fmAB";
    const char* const fmBA = "fmBA";
    const char* const waveA = "waveA";
    const char* const waveB = "waveB";
}

class SynthesiserAudioProcessor  : public juce::AudioProcessor
{
public:
    SynthesiserAudioProcessor();
    ~SynthesiserAudioProcessor() override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessor)
};
