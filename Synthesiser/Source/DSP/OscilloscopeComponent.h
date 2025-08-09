/*
  ==============================================================================

    OscilloscopeComponent.h
    Created: 9 Aug 2025 8:51:48am
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

//==============================================================================
class OscilloscopeComponent : public juce::Component,
                              private juce::Timer
{
public:
    OscilloscopeComponent(SynthesiserAudioProcessor&);
    ~OscilloscopeComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    SynthesiserAudioProcessor& audioProcessor;
};
