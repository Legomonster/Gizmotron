#include "PluginProcessor.h"
#include "PluginEditor.h"

// Helper to create a slider, its label, and its attachment
void createSliderWithLabel(juce::AudioProcessorValueTreeState& apvts,
                           std::vector<std::unique_ptr<juce::Slider>>& sliders,
                           std::vector<std::unique_ptr<juce::Label>>& labels,
                           std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>& attachments,
                           juce::Component& parent,
                           const juce::String& paramID,
                           const juce::String& labelText)
{
    // Slider
    auto slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    parent.addAndMakeVisible(*slider);
    attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, *slider));
    sliders.push_back(std::move(slider));

    // Label
    auto label = std::make_unique<juce::Label>(paramID, labelText);
    label->attachToComponent(sliders.back().get(), false);
    label->setJustificationType(juce::Justification::centred);
    parent.addAndMakeVisible(*label);
    labels.push_back(std::move(label));
}


// ======================= MainPanel ============================

MainPanel::MainPanel(juce::AudioProcessorValueTreeState& apvts)
{
    const std::pair<const char*, const char*> paramIDs[] = {
        {ParamIDs::mixA, "Mix A"}, {ParamIDs::mixB, "Mix B"}, {ParamIDs::detuneB, "Detune B"}, {ParamIDs::fmAB, "FM A->B"}, {ParamIDs::fmBA, "FM B->A"},
        {ParamIDs::ampA, "Amp Att"}, {ParamIDs::ampD, "Amp Dec"}, {ParamIDs::ampS, "Amp Sus"}, {ParamIDs::ampR, "Amp Rel"},
        {ParamIDs::filA, "Filt Att"}, {ParamIDs::filD, "Filt Dec"}, {ParamIDs::filS, "Filt Sus"}, {ParamIDs::filR, "Filt Rel"},
        {ParamIDs::cutoff, "Cutoff"}, {ParamIDs::res, "Resonance"}, {ParamIDs::filterDrive, "Filt Drive"}, {ParamIDs::filterEnvAmt, "Filt Env"},
        {ParamIDs::amp, "Amp"}
    };
    for (const auto& id_pair : paramIDs)
    {
        createSliderWithLabel(apvts, sliders, labels, attachments, *this, id_pair.first, id_pair.second);
    }

    waveA = std::make_unique<juce::ComboBox>("Wave A");
    addAndMakeVisible(*waveA);
    waveAAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, ParamIDs::waveA, *waveA);
    auto waveALabel = std::make_unique<juce::Label>("Wave A Label", "Wave A");
    waveALabel->attachToComponent(waveA.get(), false);
    waveALabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*waveALabel);
    labels.push_back(std::move(waveALabel));

    waveB = std::make_unique<juce::ComboBox>("Wave B");
    addAndMakeVisible(*waveB);
    waveBAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, ParamIDs::waveB, *waveB);
    auto waveBLabel = std::make_unique<juce::Label>("Wave B Label", "Wave B");
    waveBLabel->attachToComponent(waveB.get(), false);
    waveBLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*waveBLabel);
    labels.push_back(std::move(waveBLabel));
}

void MainPanel::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    int sliderWidth = 100;
    int sliderHeight = 100;
    int labelHeight = 20;
    int totalHeight = sliderHeight + labelHeight;
    int x = 0, y = 0;

    // Layout sliders first
    for (size_t i = 0; i < sliders.size(); ++i)
    {
        sliders[i]->setBounds(x, y + labelHeight, sliderWidth, sliderHeight);
        labels[i]->setBounds(x, y, sliderWidth, labelHeight);
        x += sliderWidth;
        if (x + sliderWidth > bounds.getWidth())
        {
            x = 0;
            y += totalHeight;
        }
    }

    // Layout combo boxes
    y += totalHeight;
    x = 0;
    waveA->setBounds(x, y + labelHeight, sliderWidth, 25);
    labels[sliders.size()]->setBounds(x, y, sliderWidth, labelHeight);
    x += sliderWidth;
    waveB->setBounds(x, y + labelHeight, sliderWidth, 25);
    labels[sliders.size() + 1]->setBounds(x, y, sliderWidth, labelHeight);
}

// ======================= ImperfectionPanel ============================

ImperfectionPanel::ImperfectionPanel(juce::AudioProcessorValueTreeState& apvts)
{
    const std::pair<const char*, const char*> paramIDs[] = {
        {ParamIDs::drive, "Osc Drive"}, {ParamIDs::drift, "Drift"}, {ParamIDs::wowDepth, "Wow Depth"}, {ParamIDs::wowRate, "Wow Rate"},
        {ParamIDs::jitter, "Jitter"}, {ParamIDs::edgeJitter, "Edge Jitter"}, {ParamIDs::pwm, "PWM"}, {ParamIDs::compSlew, "Slew"},
        {ParamIDs::freqPink, "Freq Pink"}, {ParamIDs::freqBrown, "Freq Brown"}, {ParamIDs::pwmPink, "PWM Pink"}, {ParamIDs::pwmBrown, "PWM Brown"},
        {ParamIDs::capHealth, "Cap Health"}, {ParamIDs::humAmt, "Hum Amt"}, {ParamIDs::humHz, "Hum Hz"}, {ParamIDs::os2x, "OS 2x"}
    };
    for (const auto& id_pair : paramIDs)
    {
        createSliderWithLabel(apvts, sliders, labels, attachments, *this, id_pair.first, id_pair.second);
    }
}

void ImperfectionPanel::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    int sliderWidth = 100;
    int sliderHeight = 100;
    int labelHeight = 20;
    int totalHeight = sliderHeight + labelHeight;
    int x = 0, y = 0;

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        sliders[i]->setBounds(x, y + labelHeight, sliderWidth, sliderHeight);
        labels[i]->setBounds(x, y, sliderWidth, labelHeight);
        x += sliderWidth;
        if (x + sliderWidth > bounds.getWidth())
        {
            x = 0;
            y += totalHeight;
        }
    }
}


// ======================= SynthesiserAudioProcessorEditor ============================

SynthesiserAudioProcessorEditor::SynthesiserAudioProcessorEditor (SynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      tabs(juce::TabbedButtonBar::Orientation::TabsAtTop),
      mainPanel(p.getAPVTS()),
      imperfectionPanel(p.getAPVTS()),
      scopeComponent(p)
{
    tabs.addTab("Main", juce::Colours::darkgrey, &mainPanel, false);
    tabs.addTab("Imperfections", juce::Colours::darkgrey, &imperfectionPanel, false);
    addAndMakeVisible(tabs);

    addAndMakeVisible(scopeComponent);

    // Increased height to accommodate labels and scope
    setSize (800, 850);
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
    auto bounds = getLocalBounds();
    int scopeHeight = 150;
    tabs.setBounds(bounds.removeFromTop(getHeight() - scopeHeight));
    scopeComponent.setBounds(bounds);
}
