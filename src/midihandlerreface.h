#pragma once
#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include "daisy_seed.h"

using namespace daisy;

// MIDI
#define MIDI_CHANNEL_ONE 0
#define MIDI_CHANNEL_ALL 17
#define MIDI_VELOCITY_MAX 127
extern uint8_t preset_number;

// Reface Mode (Type Knob)
#define OSC 0
#define VCF 1
#define ENV 2
#define VCA 3
#define LFOPWM 4
#define FX 5
#define ARP 6

class MidiHandlerReface
{
	public:

    MidiHandlerReface() {}
    ~MidiHandlerReface() {}

    void Init();
    void Refresh();
    void HandleMidiMessage(MidiEvent m);
    
};

#endif
