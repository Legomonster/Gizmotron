#include "PluginProcessor.h"
#include "PluginEditor.h"

SynthesiserAudioProcessorEditor::SynthesiserAudioProcessorEditor (SynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);
}

SynthesiserAudioProcessorEditor::~SynthesiserAudioProcessorEditor()
{
}

void SynthesiserAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SynthesiserAudioProcessorEditor::resized()
{
}
