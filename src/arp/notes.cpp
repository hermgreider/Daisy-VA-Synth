#include "daisy_seed.h"
#include "notes.h"

using namespace daisy;
using namespace arpeggiator;

extern DaisySeed hardware;

void Notes::Init(Config *config)
{
    config_ = config;
    ClearNotes();
}

void Notes::NewNoteEvent(MidiEvent message)
{
    if (message.type == NoteOn) {

        NoteOnEvent noteon = message.AsNoteOn();

        // Reface sends note_on with velocity 0 rather than note_off - handle same as note off
        if (noteon.velocity == 0) {
            RemoveNote(noteon.note);
            hardware.PrintLine("Arp::Notes:note_on, removed message: %d", noteon.note);
        }

        else {
            // Add new note 
            InsertNote(noteon);
            hardware.PrintLine("Arp::Notes:note_on saved message: %d", noteon.note);
        }
    }

    else if (message.type == NoteOff) {

        NoteOffEvent noteoff = message.AsNoteOff();

        // Remove note from notes list
        RemoveNote(noteoff.note);
        hardware.PrintLine("Arp::Notes:note_off, removed message: %d", noteoff.note);
    }

    PrintNotes();
}

void Notes::ClearNotes()
{
    num_notes_ = 0;
    last_insert_time_ms_ = 0;
}

/**
 * Add note to sorted list
 */
void Notes::InsertNote(NoteOnEvent note)
{
    // Clear existing notes if hold is on and new set of notes played
    if (config_->hold && last_insert_time_ms_ + 100 < System::GetNow()) {
        num_notes_ = 0;
    }

    // find insertion point
    uint8_t current = 0;
    while (current < num_notes_ && notes_[current].note < note.note) {
        current++;
    }

    // Skip if duplicate
    if (notes_[current].note == note.note) {
        return;
    }

    // Shift elements to the right to make space for the new note
    for (uint8_t i = num_notes_; i > current; --i) {
        notes_[i] = notes_[i - 1];
    }

    // Insert the new note
    notes_[current] = note;
    ++num_notes_;
    last_insert_time_ms_ = System::GetNow();
}

/**
 * Search for note. Then shrink list to keep sorted.
 * note_value = MIDI note value 0-127
 */
void Notes::RemoveNote(uint8_t note_value)
{
    if (num_notes_ == 0 || config_->hold) {
        return;
    }

    // Find the index of the note to remove
    int8_t note_index = -1;
    for (uint8_t i = 0; i < num_notes_; i++) {
        if (notes_[i].note == note_value) {
            note_index = i;
            break;
        }
    }

    if (note_index == -1) {
        // Note not found, nothing to remove
        return;
    }

    // Shift elements to the left to fill the gap
    for (int i = note_index; i < num_notes_ - 1; i++) {
        notes_[i] = notes_[i + 1];
    }

    --num_notes_;

    // Clear last note since the shifting doesn't remove it.
    // This ensures duplicate detection works correctly on next insert.
    notes_[num_notes_].note = 0;
}

void Notes::PrintNotes()
{
    if (num_notes_ == 0) {
        hardware.PrintLine("Arp::Notes: no notes on");
        return;
    }

    hardware.PrintLine("Arp::Notes: num_notes: %d", num_notes_);
    for (uint16_t i = 0; i < num_notes_; i++) {
        hardware.Print("  %d", notes_[i].note);
    }

    hardware.PrintLine(" ");
}
