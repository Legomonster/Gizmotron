/*
  ==============================================================================

    AnalogSound.h
    Created: 9 Aug 2025 6:45:41am
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class AnalogSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int /*midiNoteNumber*/) override      { return true; }
    bool appliesToChannel (int /*midiChannel*/) override    { return true; }
};
