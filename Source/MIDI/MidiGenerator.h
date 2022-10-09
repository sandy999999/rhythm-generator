/*
  ==============================================================================

    MidiGenerator.h
    Created: 9 Oct 2022 9:09:12pm
    Author:  Andreas Sandersen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../utils/euclidean.h"

//==============================================================================
/**
*/
class MidiGenerator
{
public:
    MidiGenerator();
    ~MidiGenerator() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    struct Rhythm
    {
        Rhythm(AudioParameterBool* isActive, AudioParameterInt* noteNumber, AudioParameterInt* stepsNumber, AudioParameterInt* pulseNumber, AudioParameterBool* sphereOn);

        void reset();

        AudioParameterBool* activated;
        AudioParameterInt* note;
        AudioParameterInt* steps;
        AudioParameterInt* pulses;
        AudioParameterBool* sphere;

        int cachedMidiNote;

    };

    OwnedArray<Rhythm> rhythms;

private:
    
    AudioProcessorValueTreeState valueTree;

    double fs;
    int time;
    int stepIndex;

    bool noteIsOn;
    
    int timerPeriodMs;

    void timerCallback() override;
    
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout(int rhythmCount) const;

    static int getRhythmCount();

    AudioPlayHead::CurrentPositionInfo posInfo;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Euclidean_seqAudioProcessor)
};
