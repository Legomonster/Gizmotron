#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth/AnalogSound.h"
#include "Synth/AnalogVoice.h"

// Helper function to create the parameter layout
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Helper lambda for adding parameters
    auto addParam = [&](const juce::String& id, const juce::String& name, float min, float max, float def) {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(id, name, min, max, def));
    };

    // JS parameter descriptors translated to JUCE parameters
    addParam(ParamIDs::drive, "Drive", 0.0f, 1.0f, 0.35f);
    addParam(ParamIDs::amp, "Amp", 0.0f, 1.0f, 0.4f);
    addParam(ParamIDs::drift, "Drift", 0.0f, 50.0f, 4.0f);
    addParam(ParamIDs::wowDepth, "Wow Depth", 0.0f, 80.0f, 6.0f);
    addParam(ParamIDs::wowRate, "Wow Rate", 0.01f, 12.0f, 0.6f);
    addParam(ParamIDs::jitter, "Jitter", 0.0f, 0.02f, 0.002f);
    addParam(ParamIDs::edgeJitter, "Edge Jitter", 0.0f, 0.02f, 0.003f);
    addParam(ParamIDs::pwm, "PWM", 0.05f, 0.95f, 0.5f);
    addParam(ParamIDs::compSlew, "Comp Slew", 0.0f, 0.02f, 0.0003f);
    addParam(ParamIDs::freqPink, "Freq Pink", 0.0f, 40.0f, 2.0f);
    addParam(ParamIDs::freqBrown, "Freq Brown", 0.0f, 80.0f, 4.0f);
    addParam(ParamIDs::pwmPink, "PWM Pink", 0.0f, 0.2f, 0.01f);
    addParam(ParamIDs::pwmBrown, "PWM Brown", 0.0f, 0.4f, 0.02f);
    addParam(ParamIDs::capHealth, "Cap Health", 1.0f, 20.0f, 1.0f);
    addParam(ParamIDs::humAmt, "Hum Amount", 0.0f, 0.01f, 0.001f);
    addParam(ParamIDs::humHz, "Hum Hz", 40.0f, 120.0f, 50.0f);
    addParam(ParamIDs::os2x, "OS 2x", 0.0f, 1.0f, 1.0f);
    addParam(ParamIDs::cutoff, "Cutoff", 0.0f, 1.0f, 0.5f);
    addParam(ParamIDs::res, "Resonance", 0.0f, 1.2f, 0.5f);
    addParam(ParamIDs::filterDrive, "Filter Drive", 0.0f, 1.0f, 0.2f);
    addParam(ParamIDs::filterEnvAmt, "Filter Env Amount", -1.0f, 1.0f, 0.5f);
    addParam(ParamIDs::ampA, "Amp Attack", 0.0f, 2.0f, 0.005f);
    addParam(ParamIDs::ampD, "Amp Decay", 0.0f, 4.0f, 0.15f);
    addParam(ParamIDs::ampS, "Amp Sustain", 0.0f, 1.0f, 0.7f);
    addParam(ParamIDs::ampR, "Amp Release", 0.0f, 4.0f, 0.25f);
    addParam(ParamIDs::filA, "Filter Attack", 0.0f, 2.0f, 0.01f);
    addParam(ParamIDs::filD, "Filter Decay", 0.0f, 4.0f, 0.2f);
    addParam(ParamIDs::filS, "Filter Sustain", 0.0f, 1.0f, 0.4f);
    addParam(ParamIDs::filR, "Filter Release", 0.0f, 4.0f, 0.3f);
    addParam(ParamIDs::mixA, "Mix A", 0.0f, 1.0f, 0.6f);
    addParam(ParamIDs::mixB, "Mix B", 0.0f, 1.0f, 0.6f);
    addParam(ParamIDs::detuneB, "Detune B", -24.0f, 24.0f, 7.0f);
    addParam(ParamIDs::fmAB, "FM A->B", 0.0f, 2000.0f, 0.0f);
    addParam(ParamIDs::fmBA, "FM B->A", 0.0f, 2000.0f, 0.0f);

    juce::StringArray waves = { "Saw", "Square", "Triangle" };
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::waveA, "Wave A", waves, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParamIDs::waveB, "Wave B", waves, 0));

    return { params.begin(), params.end() };
}


SynthesiserAudioProcessor::SynthesiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    synth.addSound(new AnalogSound());
    synth.addVoice(new AnalogVoice(apvts));
}

SynthesiserAudioProcessor::~SynthesiserAudioProcessor()
{
}

const juce::String SynthesiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthesiserAudioProcessor::acceptsMidi() const { return true; }
bool SynthesiserAudioProcessor::producesMidi() const { return false; }
bool SynthesiserAudioProcessor::isMidiEffect() const { return false; }
double SynthesiserAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SynthesiserAudioProcessor::getNumPrograms() { return 1; }
int SynthesiserAudioProcessor::getCurrentProgram() { return 0; }
void SynthesiserAudioProcessor::setCurrentProgram (int index) {}
const juce::String SynthesiserAudioProcessor::getProgramName (int index) { return {}; }
void SynthesiserAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void SynthesiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<AnalogVoice*>(synth.getVoice(i)))
        {
            voice->prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
        }
    }

    scopeBuffer.setSize(1, 512);
    scopeBuffer.clear();
}

void SynthesiserAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthesiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}
#endif

void SynthesiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Copy the output to the scope buffer
    int numSamplesToCopy = std::min(buffer.getNumSamples(), scopeBuffer.getNumSamples());
    scopeBuffer.copyFrom(0, 0, buffer, 0, 0, numSamplesToCopy);
}

bool SynthesiserAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SynthesiserAudioProcessor::createEditor()
{
    return new SynthesiserAudioProcessorEditor (*this);
}

void SynthesiserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthesiserAudioProcessor();
}
