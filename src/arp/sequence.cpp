#include "daisy_seed.h"
#include "notes.h"
#include "sequence.h"

using namespace daisy;
using namespace arpeggiator;

extern DaisySeed hardware;

void Sequence::Init(Config *config)
{
    config_ = config;
}

/**
 * Create sequence of notes for the arpeggiator. If octaves = 1, this is effectively 
 * the same list as Notes. Otherwise, this adds additional notes for each octave.
 */
void Sequence::Create(Notes *notes)
{
    last_sequence_index_ = 0;

    for (uint8_t octave = 0; octave < config_->octaves; octave++) {
        for (uint16_t i = 0; i < notes->NumNotes(); i++) {
            sequence_[last_sequence_index_] = notes->notes_[i];
            sequence_[last_sequence_index_].note = notes->notes_[i].note + (octave * 12);
            ++last_sequence_index_;
        }
    }

    if (config_->pattern == RANDOM) {
        Randomize();        
    }
}   

void Sequence::Randomize()
{

}

void Sequence::PrintSequence() 
{
    if (last_sequence_index_ == 0) {
        hardware.PrintLine("Arp::Sequence: empty");
        return;
    }

    hardware.PrintLine("Arp::Sequence: num_notes: %d", last_sequence_index_);
    for (uint8_t i = 0; i < last_sequence_index_; i++) {
        hardware.Print("  %d", sequence_[i].note);
    }

    hardware.PrintLine(" ");
}