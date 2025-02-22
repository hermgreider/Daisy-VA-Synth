#include "daisy_seed.h"
#include "../vasynth.h"
#include "arp.h"

using namespace daisy;
using namespace arpeggiator;

extern DaisySeed hardware;
extern VASynth vasynth;

void Arp::Init()
{
    notes_.Init(&config_);
    sequence_.Init(&config_);

    running_ = true;
    playing_ = false;
    start_new_sequence_ = false;
    next_note_ms_ = 0;
}

void Arp::Refresh() 
{
    if (running_) {
        if (!playing_) {

            if (notes_.NumNotes() > 0) {

                hardware.PrintLine("Starting sequence");

                // start on first note
                start_new_sequence_ = true;
                playing_ = true;

                // wait 30ms for additional notes to start sequence
                next_note_ms_ = System::GetNow() + 30;
            }
        }

        else if (notes_.NumNotes() == 0) {
            playing_ = false;

            // clear all notes
            for (uint8_t i = 0; i < sequence_.NumSequenceNotes(); i++) {
                vasynth.NoteOff(sequence_.sequence_[i].note+1);
            }
        }

        else if (next_note_ms_ < System::GetNow()) {
            // clear all notes
            for (uint8_t i = 0; i < sequence_.NumSequenceNotes(); i++) {
                vasynth.NoteOff(sequence_.sequence_[i].note+1);
            }

            Play();
        }
    }
}

void Arp::Stop() 
{
    running_ = false;
}

void Arp::Play()
{
    if (start_new_sequence_) {

        start_new_sequence_ = false;

        swing_num = 0;
        if (config_.swing == 0.66) {
            swing_num = 1;
        }

        sequence_.Create(&notes_);

        hardware.PrintLine("Arp: Created new sequence");
        notes_.PrintNotes();
        sequence_.PrintSequence();

        if ((config_.pattern == UP) or (config_.pattern == UPDOWN) or (config_.pattern == RANDOM)) {
            current_play_note_ = 0;
            direction_ = 1;
        }

        else if (config_.pattern == DOWN ) {
            current_play_note_ = sequence_.NumSequenceNotes() - 1;
            direction_ = -1;
        }
    }
    
    hardware.PrintLine("Arp: pattern: %d, current_play_note: %d, direction: %d", config_.pattern, current_play_note_, direction_);

    // TODO: Send to the vasynth
    PlayNote(sequence_.sequence_[current_play_note_]);

    current_play_note_ += direction_;

    hardware.PrintLine("Arp: after play, current_play_note: %d, num sequence notes: %d", current_play_note_, sequence_.NumSequenceNotes());

    if ((config_.pattern == UP or config_.pattern == RANDOM) && (current_play_note_ >= sequence_.NumSequenceNotes())) {
        start_new_sequence_ = 1;
    }

    else if ((config_.pattern == DOWN || (config_.pattern == UPDOWN and direction_ == -1)) and (current_play_note_ == -1)) {
        start_new_sequence_ = 1;
    }

    else if ((config_.pattern == UPDOWN && direction_ == 1) && (current_play_note_ == sequence_.NumSequenceNotes())) {
        --current_play_note_; // repeat top
        direction_ = -1;
    }

    next_note_ms_ += ((60 * 1000) / config_.bpm);
    hardware.PrintLine("next ms: %d", next_note_ms_);

    // # if (pmid_swing_current() == STRAIGHT) {
    // #     next_note_64 += 60000000 / notes_per_min;
    // # }
    // # else {
    // #     // first note is 33% of 2 notes so 67% of one
    // #     // second note is 67% of 2 notes so 1.34 of one
    // #     // For 66, sequence starts with swing_num = 1 so reversed

    // #     next_note_64 += (60000000 / notes_per_min) * (67 * (swing_num+1)) / 100;
    // #     swing_num = (swing_num + 1) % 2;
    // # }
}

// Play a note in notes
// current_play_note is the index of the note to play
void Arp::PlayNote(NoteOnEvent note) 
{ 
    // TODO: Instead of sending to MIDI, call the synth
    // self.output.send(message)
    vasynth.NoteOn(note.note+1, note.velocity);

    hardware.PrintLine("Arp: played %d", note.note);
}
