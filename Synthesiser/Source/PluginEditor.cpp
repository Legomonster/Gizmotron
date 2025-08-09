#include "PluginProcessor.h"
#include "PluginEditor.h"

// Helper to create a slider and its attachment
void createSlider(juce::AudioProcessorValueTreeState& apvts,
                  std::vector<std::unique_ptr<juce::Slider>>& sliders,
                  std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>& attachments,
                  juce::Component& parent,
                  const juce::String& paramID)
{
    auto slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    parent.addAndMakeVisible(*slider);
    attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *slider));
    sliders.push_back(std::move(slider));
}

// ======================= MainPanel ============================

MainPanel::MainPanel(juce::AudioProcessorValueTreeState& apvts)
{
    const char* paramIDs[] = {
        ParamIDs::mixA, ParamIDs::mixB, ParamIDs::detuneB, ParamIDs::fmAB, ParamIDs::fmBA,
        ParamIDs::ampA, ParamIDs::ampD, ParamIDs::ampS, ParamIDs::ampR,
        ParamIDs::filA, ParamIDs::filD, ParamIDs::filS, ParamIDs::filR,
        ParamIDs::cutoff, ParamIDs::res, ParamIDs::filterDrive, ParamIDs::filterEnvAmt,
        ParamIDs::amp
    };
    for (const auto* id : paramIDs)
    {
        createSlider(apvts, sliders, attachments, *this, id);
    }

    waveA = std::make_unique<juce::ComboBox>();
    addAndMakeVisible(*waveA);
    waveAAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, ParamIDs::waveA, *waveA);

    waveB = std::make_unique<juce::ComboBox>();
    addAndMakeVisible(*waveB);
    waveBAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, ParamIDs::waveB, *waveB);
}

void MainPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    int sliderWidth = 100;
    int sliderHeight = 100;
    int x = 0, y = 0;

    for (auto& slider : sliders)
    {
        slider->setBounds(x, y, sliderWidth, sliderHeight);
        x += sliderWidth;
        if (x + sliderWidth > bounds.getWidth())
        {
            x = 0;
            y += sliderHeight;
        }
    }

    y += sliderHeight;
    waveA->setBounds(x, y, sliderWidth, 20);
    x += sliderWidth;
    waveB->setBounds(x, y, sliderWidth, 20);
}

// ======================= ImperfectionPanel ============================

ImperfectionPanel::ImperfectionPanel(juce::AudioProcessorValueTreeState& apvts)
{
    const char* paramIDs[] = {
        ParamIDs::drive, ParamIDs::drift, ParamIDs::wowDepth, ParamIDs::wowRate,
        ParamIDs::jitter, ParamIDs::edgeJitter, ParamIDs::pwm, ParamIDs::compSlew,
        ParamIDs::freqPink, ParamIDs::freqBrown, ParamIDs::pwmPink, ParamIDs::pwmBrown,
        ParamIDs::capHealth, ParamIDs::humAmt, ParamIDs::humHz, ParamIDs::os2x
    };
    for (const auto* id : paramIDs)
    {
        createSlider(apvts, sliders, attachments, *this, id);
    }
}

void ImperfectionPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    int sliderWidth = 100;
    int sliderHeight = 100;
    int x = 0, y = 0;

    for (auto& slider : sliders)
    {
        slider->setBounds(x, y, sliderWidth, sliderHeight);
        x += sliderWidth;
        if (x + sliderWidth > bounds.getWidth())
        {
            x = 0;
            y += sliderHeight;
        }
    }
}


// ======================= SynthesiserAudioProcessorEditor ============================

SynthesiserAudioProcessorEditor::SynthesiserAudioProcessorEditor (SynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      tabs(juce::TabbedButtonBar::Orientation::TabsAtTop),
      mainPanel(p.getAPVTS()),
      imperfectionPanel(p.getAPVTS())
{
    tabs.addTab("Main", juce::Colours::darkgrey, &mainPanel, false);
    tabs.addTab("Imperfections", juce::Colours::darkgrey, &imperfectionPanel, false);
    addAndMakeVisible(tabs);

    setSize (800, 600);
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
    tabs.setBounds(getLocalBounds());
}
