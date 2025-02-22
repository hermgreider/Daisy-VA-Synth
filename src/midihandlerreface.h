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
#define PERF 1
#define VCF 2
#define VCA 3
#define LFOPWM 4
#define ARP 5

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
