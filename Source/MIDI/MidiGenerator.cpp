/*
  ==============================================================================
    MidiGenerator.cpp
    Created: 9 Oct 2022 9:09:12pm
    Author:  Andreas Sandersen
  ==============================================================================
*/

#include "MidiGenerator.h"

MidiGenerator::MidiGenerator(),

parameters(*this, nullptr, Identifier("RhythmGeneratorPlugin"), createParameterLayout(getRhythmCount()))
    {
        for (int i = 0; i < getRhythmCount(); ++i)
        {
            auto paramIDs = getParameterIDs(i);
            jassert(paramIDs.size() == 5);

            auto activeParam = dynamic_cast<AudioParameterBool*>(parameters.getParameter(paramIDs[0]));
            jassert(activeParam != nullptr);

            auto noteParam = dynamic_cast<AudioParameterInt*>(parameters.getParameter(paramIDs[1]));
            jassert(noteParam != nullptr);

            auto stepsParam = dynamic_cast<AudioParameterInt*>(parameters.getParameter(paramIDs[2]));
            jassert(stepsParam != nullptr);

            auto pulseParam = dynamic_cast<AudioParameterInt*>(parameters.getParameter(paramIDs[3]));
            jassert(pulseParam != nullptr);

            auto sphereParam = dynamic_cast<AudioParameterBool*>(parameters.getParameter(paramIDs[4]));
            jassert(sphereParam != nullptr);

            rhythms.add(new Rhythm(activeParam, noteParam, stepsParam, pulseParam, sphereParam));
        }
        
        startTimer(20);
    }
}

MidiGenerator::~MidiGenerator()
{
    stopTimer();
}


MidiGenerator::ParameterLayout SandysRhythmGeneratorAudioProcessor::createParameterLayout(const int rhythmCount) const
{

    AudioProcessorValueTreeState::ParameterLayout params;

    for (int i = 0; i < rhythmCount; ++i)
    {
        auto paramIDs = getParameterIDs(i);
        jassert(paramIDs.size() == 5);

        params.add(std::make_unique<AudioParameterBool>(paramIDs[0], paramIDs[0], false));
        params.add(std::make_unique<AudioParameterInt>(paramIDs[1], paramIDs[1], 24, 127, 36));
        params.add(std::make_unique<AudioParameterInt>(paramIDs[2], paramIDs[2], 1, 32, 8));
        params.add(std::make_unique<AudioParameterInt>(paramIDs[3], paramIDs[3], 1, 32, 4));
        params.add(std::make_unique<AudioParameterBool>(paramIDs[4], paramIDs[4], false));
    }

    return params;
}
int MidiGenerator::getRhythmCount()
{
    return 4;
}

void MidiGenerator::prepareToPlay (double sampleRate, int samplesPerBlock) override
{
    ignoreUnused (samplesPerBlock);

    notes.clear();
    currentNote = 0;
    lastNoteValue = -1;
    time = 0;
    rate = static_cast<float> (sampleRate);
}


void MidiGenerator::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    buffer.clear();

    auto numSamples = buffer.getNumSamples();
    
    //Get timing info from host
    auto* playHead = getPlayHead();

    if (playHead != nullptr)
    {
        playHead->getCurrentPosition(posInfo);
    }

    auto bpm = posInfo.bpm;
    auto bps = bpm / 60.0f;
    auto samplesPerBeat = (fs / bps) / 2;
    auto beatsPerBuffer = numSamples * bpm / 60.0 / fs;
    auto counter = static_cast<int>(posInfo.timeInSamples % (int)(samplesPerBeat));
    auto ppqPos = posInfo.ppqPosition;
    auto eventTime = counter % numSamples;
    
    for (auto rhythm : rhythms)
    {
        if (rhythm->activated->get() == true && posInfo.isPlaying == true)
        {

            int steps = rhythm->steps->get();
            int pulses = rhythm->pulses->get();

            
            int note = rhythm->note->get();

            if (pulses > steps)
            {
                rhythm->pulses->setValueNotifyingHost(steps);
            }

            //Call Euclidean algorithm and store pulses and steps into string
            std::string rhythmSeq = euclidean(pulses, steps);

            if ((counter + numSamples) >= samplesPerBeat || posInfo.ppqPosition == 0.0)
            //if (ppqPos == floor(ppqPos) || (ppqPos + beatsPerBuffer) >= (floor(ppqPos) + 1))
            {
                //Reset indexing when reaching end of rhythm
                if (stepIndex >= steps - 1)
                {
                    stepIndex = -1;
                }

                stepIndex++;
                
                if (rhythmSeq[stepIndex] == '1')
                {
                    midiMessages.addEvent(MidiMessage::noteOn(1, note, (juce::uint8) 127), midiMessages.getLastEventTime() + 1);
                    rhythm->sphere->setValueNotifyingHost(true);
                }
                else if (rhythmSeq[stepIndex] == '0')
                {
                    midiMessages.addEvent(MidiMessage::noteOff(1, note, (juce::uint8) 0), midiMessages.getLastEventTime() + 1);
                    rhythm->sphere->setValueNotifyingHost(true);
                }
            }
            else
            {
                rhythm->sphere->setValueNotifyingHost(false);
            }

               
        }

    }
}

MidiGenerator::Rhythm::Rhythm(AudioParameterBool* isActive, AudioParameterInt* noteNumber, AudioParameterInt* stepsNumber, AudioParameterInt* pulseNumber, AudioParameterBool* sphereOn)
    :
    activated(isActive), note(noteNumber), steps(stepsNumber), pulses(pulseNumber), sphere(sphereOn)
{
    
}

void MidiGenerator::Rhythm::reset()
{
    cachedMidiNote = note->get();
    activated->setValueNotifyingHost(false);
    note->setValueNotifyingHost(36);
    steps->setValueNotifyingHost(8);
    pulses->setValueNotifyingHost(4);
    sphere->setValueNotifyingHost(false);
}

StringArray MidiGenerator::getParameterIDs(const int rhythmIndex)
{
    String activated = "Activated";
    String note = "NoteNumber";
    String steps = "Steps";
    String pulses = "Pulses";
    String sphere = "SphereOn";

    StringArray paramIDs = { activated, note, steps, pulses, sphere};

    //Append Rhythms index to parameter IDs
    for (int i = 0; i < paramIDs.size(); ++i)
        paramIDs.getReference(i) += rhythmIndex;
    return paramIDs;
}

void MidiGenerator::timerCallback()
{
    
    for (auto rhythm : rhythms)
    {
        if (rhythm->activated->get() != true)
            return;
    }
}
