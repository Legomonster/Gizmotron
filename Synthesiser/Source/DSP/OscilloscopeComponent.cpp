/*
  ==============================================================================

    OscilloscopeComponent.cpp
    Created: 9 Aug 2025 8:51:56am
    Author:  Jules

  ==============================================================================
*/

#include "OscilloscopeComponent.h"

//==============================================================================
OscilloscopeComponent::OscilloscopeComponent(SynthesiserAudioProcessor& p)
    : audioProcessor(p)
{
    startTimerHz(30);
}

OscilloscopeComponent::~OscilloscopeComponent()
{
    stopTimer();
}

void OscilloscopeComponent::paint(juce::Graphics& g)
{
    return;
    auto bounds = getLocalBounds().toFloat();
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::green);

    auto& buffer = audioProcessor.getScopeBuffer();
    auto* bufferData = buffer.getReadPointer(0);
    int numSamples = buffer.getNumSamples();

    if (numSamples <= 0) return;

    juce::Path p;
    p.startNewSubPath(0, bounds.getCentreY() - bufferData[0] * bounds.getHeight() * 0.5f);

    for (int i = 1; i < numSamples; ++i)
    {
        float x = (float)i / (float)(numSamples - 1) * bounds.getWidth();
        float y = bounds.getCentreY() - bufferData[i] * bounds.getHeight() * 0.5f;
        p.lineTo(x, y);
    }

    g.strokePath(p, juce::PathStrokeType(1.0f));
}

void OscilloscopeComponent::resized()
{
    // This component doesn't have any child components, so this is empty.
}

void OscilloscopeComponent::timerCallback()
{
    repaint();
}
