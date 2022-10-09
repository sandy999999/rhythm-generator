/*
  ==============================================================================

    Arp.h
    Created: 9 Oct 2022 9:12:18pm
    Author:  Andreas Sandersen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <iterator>

class Arp  : public juce::Component
{
public:
    Arp();
    ~Arp() override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    
    void releaseResources() override {};
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi) override;

private:
    ValueTree valueTree { "params" };
    
    AudioParameterFloat* speed;
    int currentNote, lastNoteValue;
    int time;
    float rate;
    SortedSet<int> notes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Arp)
};

