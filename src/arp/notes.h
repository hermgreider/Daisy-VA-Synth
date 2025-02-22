#pragma once
#ifndef ARP_NOTES_
#define ARP_NOTES_

/**
 * Notes container to keep currently pressed notes
 */

#include <array>
#include "daisy_seed.h"
#include "config.h"

using namespace daisy;

const uint8_t MAX_ARP_NOTES = 16;

namespace arpeggiator {

  class MyClass {
    public:
        int value;
        MyClass() : value(0) {}  // Default constructor (value initialized to 0)
    };
class Notes
{
public:
    void Init(Config *config);
    void NewNoteEvent(MidiEvent m);
    void ClearNotes();
    bool Playing() { return num_notes_ > 0; }
    uint16_t NumNotes() { return num_notes_; }
    void PrintNotes();

    // List of currently playing notes
    NoteOnEvent notes_[MAX_ARP_NOTES];

private:
    void InsertNote(NoteOnEvent note);
    void RemoveNote(uint8_t note_value);

    uint8_t num_notes_ = 0;
    uint32_t last_insert_time_ms_;

    Config *config_;
};
}

#endif