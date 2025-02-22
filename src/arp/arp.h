#pragma once
#ifndef ARP_H_
#define ARP_H_

#include "daisy_seed.h"
#include "sequence.h"
#include "config.h"
#include "notes.h"

using namespace daisy;

namespace arpeggiator {

class Arp 
{
public:
    void Init();
    void Refresh();
    void UpdateConfig();
    void NewNoteEvent(MidiEvent new_note) { notes_.NewNoteEvent(new_note); }
    void Stop();
    bool Running() { return running_; }

private:
    void CreateSequence(void);
    void Play(void);
    void PlayNote(NoteOnEvent note); 

    Config config_;
    Sequence sequence_;
    Notes notes_;

    bool running_ = true;
    bool playing_ = false;
    bool start_new_sequence_ = false;

    uint16_t current_play_note_ = 0;
    uint8_t direction_ = 1;

    uint8_t swing_num = 0;
    uint32_t next_note_ms_ = 0;
    NoteOnEvent prev_note;
};
}

#endif 
