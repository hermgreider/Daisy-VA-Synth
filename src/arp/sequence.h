#pragma once
#ifndef SEQUENCE_H_
#define SEQUENCE_H_

/**
 * Sequence container to keep and create arpeggiator note sequence
 */

#include <array>
#include "daisy_seed.h"
#include "config.h"
#include "notes.h"

using namespace daisy;

namespace arpeggiator {

class Sequence 
{
public:
    void Init(Config *config);
    void Create(Notes *notes);
    uint16_t NumSequenceNotes() { return last_sequence_index_; }
    void PrintSequence();

    // List of notes in the sequence
    NoteOnEvent sequence_[100];

private:
    void Randomize();

    Config *config_;
    uint8_t last_sequence_index_;
};
}

#endif