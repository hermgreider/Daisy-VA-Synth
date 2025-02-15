#pragma once
#ifndef PMID_ARP_H_
#define PMID_ARP_H_

#include "daisy_seed.h"

using namespace daisy;

typedef struct {
  NoteOnEvent note;
  uint32_t offset_ms; /* ms from start of the track */
} Note;

enum Pattern {
  UP = 0, DOWN = 1, UPDOWN = 2, RANDOM = 3, LAST_PATTERN = 4
};

const uint8_t MAX_ARP_NOTES = 16;

class Arp 
{
public:
  void pmid_arp_init();
  void set_pattern(Pattern pattern);
  void set_octaves(uint8_t num_octaves);
  void ready(NoteOnEvent note_on);
  void create_sequence(void);
  void play(void);
  void play_note(Note note); 

private:

  Note notes[MAX_ARP_NOTES];
  Note sequence[1024];

  Pattern pattern = UP;
  uint8_t octaves = 1;
  uint8_t repeats = 1;

  uint8_t *control_notes = NULL;
  uint8_t num_notes = 0; // number of current notes on
  uint16_t num_sequence_notes = 0;
  uint8_t start_new_sequence;

  int8_t midi_callback_id = -1;
  int8_t timer_callback_id = -1;

  void insert_note(MidiEvent note_on); 
  void remove_note(MidiEvent note_off);
  void print_notes(void);
  void print_sequence(void);
}

#endif 
