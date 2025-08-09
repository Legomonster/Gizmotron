#pragma once

#include "PluginProcessor.h"

class SynthesiserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SynthesiserAudioProcessorEditor (SynthesiserAudioProcessor&);
    ~SynthesiserAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SynthesiserAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessorEditor)
};
