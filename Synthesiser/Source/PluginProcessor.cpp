#include "PluginProcessor.h"
#include "PluginEditor.h"

SynthesiserAudioProcessor::SynthesiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SynthesiserAudioProcessor::~SynthesiserAudioProcessor()
{
}

const juce::String SynthesiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthesiserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthesiserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SynthesiserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SynthesiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthesiserAudioProcessor::getNumPrograms()
{
    return 1;
}

int SynthesiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthesiserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SynthesiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void SynthesiserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SynthesiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void SynthesiserAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthesiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SynthesiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Do nothing with MIDI messages
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
}

void SynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthesiserAudioProcessor();
}
