#pragma once

#include "PluginProcessor.h"
#include "DSP/OscilloscopeComponent.h"
#include <vector>

// To avoid cluttering the main editor, we'll create component classes for each tab
class MainPanel : public juce::Component
{
public:
    MainPanel(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;
private:
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::Label>> labels;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::unique_ptr<juce::ComboBox> waveA, waveB;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveAAttach, waveBAttach;
};

class ImperfectionPanel : public juce::Component
{
public:
    ImperfectionPanel(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;
private:
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::Label>> labels;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};


//==============================================================================
class SynthesiserAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SynthesiserAudioProcessorEditor (SynthesiserAudioProcessor&);
    ~SynthesiserAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SynthesiserAudioProcessor& audioProcessor;

    juce::TabbedComponent tabs;
    MainPanel mainPanel;
    ImperfectionPanel imperfectionPanel;
    OscilloscopeComponent scopeComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesiserAudioProcessorEditor)
};
