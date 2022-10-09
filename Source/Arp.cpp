/*
  ==============================================================================

    Arp.cpp
    Created: 9 Oct 2022 9:12:18pm
    Author:  Andreas Sandersen

  ==============================================================================
*/

#include "Arp.h"

Arp::Arp()
{
    valueTree.addParameter (speed = new AudioParameterFloat ({ "speed", 1 }, "Arpeggiator Speed", 0.0, 1.0, 0.5));
}

Arp::~Arp()
{
    
}

void Arp::prepareToPlay (double sampleRate, int samplesPerBlock) override
{
    ignoreUnused (samplesPerBlock);

    notes.clear();
    currentNote = 0;
    lastNoteValue = -1;
    time = 0;
    rate = static_cast<float> (sampleRate);
}


void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi) override
{
    // A pure MIDI plugin shouldn't be provided any audio data
    jassert (buffer.getNumChannels() == 0);

    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();

    // get note duration
    auto noteDuration = static_cast<int> (std::ceil (rate * 0.25f * (0.1f + (1.0f - (*speed)))));

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber());
    }

    midi.clear();

    if ((time + numSamples) >= noteDuration)
    {
        auto offset = jmax (0, jmin ((int) (noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0)
        {
            midi.addEvent (MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if (notes.size() > 0)
        {
            currentNote = (currentNote + 1) % notes.size();
            lastNoteValue = notes[currentNote];
            midi.addEvent (MidiMessage::noteOn  (1, lastNoteValue, (uint8) 127), offset);
        }

    }

    time = (time + numSamples) % noteDuration;
}
