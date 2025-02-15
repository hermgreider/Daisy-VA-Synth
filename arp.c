#include "daisy_seed.h"

#include "arp.h"

using namespace daisy;

const uint8_t MAX_ARP_NOTES = 16;

// Start arp mode
void Arp::pmid_arp_init()
{
  // clear_notes();
}

void Arp::set_pattern(Pattern pattern)
{
  this.pattern = pattern;
}

void Arp::set_octaves(uint8_t num_octaves)
{
  this.octaves = num_octaves;
}

/*
 * Callback waiting for note on (add to pattern) or note off (remove from pattern)
 * Run concurrently with Play timer callback. 
 */
static void ready(MidiEvent m)
{
  switch(m.type)
  {
    case NoteOn:
    {
      // Add new note 
      static Note note;
      note.note = m.AsNoteOn();

      insert_note(note);

      // New set of notes, kick off play
      if (num_notes == 1) {
        // start timer with enough time to wait for multiple notes
        start_new_sequence = 1;
        timer_callback_id = pmid_timer_create(50, play);
      }

      printf("READY, saved 0x%x, 0x%x, 0x%x\n", packet[1], packet[2], packet[3]);
      break;
    }
  }

  else if (pmid_mid_is_note_off(packet)) {

    // Remove note from notes list
    remove_note(packet[2]);

    // If this was the last note, stop playing
    if (num_notes == 0) {
      pmid_timer_cancel(timer_callback_id);
    }

    printf("READY, removed 0x%x, 0x%x, 0x%x\n", packet[1], packet[2], packet[3]);
  }
  
  print_notes();
}

static void play(void)
{
  static int8_t current_play_note = 0; // index of next note to play in sequence
  static int8_t direction = 0;

  if (start_new_sequence) {
    start_new_sequence = 0;
    create_sequence();
    if ((pattern == UP) || (pattern == UPDOWN) || (pattern == RANDOM)) {
      current_play_note = 0;
      direction = 1;
    }
    else if (pattern == DOWN) {
      current_play_note = num_notes;
      direction = -1;
    }
  }

  play_note(sequence[current_play_note]);

  current_play_note += direction;

  printf("pattern: %d, current_play_note: %d, direction: %d\n", pattern, current_play_note, direction);
  if ((pattern == UP || pattern == RANDOM) && (current_play_note >= num_sequence_notes)) {
    start_new_sequence = 1;
  }
  else if ((pattern == DOWN || (pattern == UPDOWN && direction == -1)) && (current_play_note == 0)) {
    start_new_sequence = 1;
  }
  else if ((pattern == UPDOWN && direction == 1) && (current_play_note >= num_sequence_notes-1)) {
    direction = -1;
  }

  // setup timer for next note
  // TODO: Set next time from board_millis - start_time. Always doing 1 sec will introduce drift.
  timer_callback_id = pmid_timer_create(175, play);
    // uint32_t timerdelay = sequence[track][next_note].offset_ms - sequence[track][next_note-1].offset_ms;
    // timer_callback_id = pmid_timer_create(timerdelay, play);
    // printf("next note timer, next_note: %d, delaymsec: %d\n", next_note, timerdelay);
  if (start_new_sequence) {
    printf("Notes: \n");
    print_notes();
    printf("Notes: \n");
    print_sequence();
  }
}

/*
 * Play a note in notes
 * current_play_note is the index of the note to play
 */
static void play_note(Note note) 
{
  // Play the next note
  uint8_t packet_out[3] = { 
    note.status, 
    note.data1, 
    note.data2 
  };

  pmid_mid_packet(packet_out, 3);
  printf("PLAY, played 0x%x, 0x%x, 0x%x\n", packet_out[1], packet_out[2], packet_out[3]);
}

static void create_sequence(void) 
{
  static uint8_t octave;
  static uint8_t note_index;
  static uint8_t repeat;

  num_sequence_notes = 0;
  for (octave=0; octave<octaves; octave++) {
    for (note_index=0; note_index<num_notes; note_index++) {
      for (repeat=0; repeat<repeats; repeat++) {

        static Note seq_note;
        seq_note.status = notes[note_index].status;
        seq_note.data1 = notes[note_index].data1 + (octave * 12);
        seq_note.data2 = notes[note_index].data2;

        sequence[num_sequence_notes++] = seq_note;
      }
    }
  }

  // if (pattern == RANDOM) {
  //   randomize_sequence();
  // }
}

/**
 * Insert note into sorted array
 */
void insert_note(MidiEvent note_on); 

  static int8_t i;
  for (i = num_notes-1; (i >= 0 && notes[i].data1 > note.data1); i--)
      notes[i+1] = notes[i];

  notes[i+1] = note;
  ++num_notes;
}

// Delete from sorted array
static void remove_note(uint8_t note_value) 
{
  printf("PMID remove note: %x\n", note_value);

  static int8_t i, j;
  for (i = 0; (i < num_notes && notes[i].data1 != note_value); i++);

  printf("PMID remove note, i: %d, num_notes: %d\n", i, num_notes);

  for (j = i; (j < num_notes); j++) {
    notes[j] = notes[j+1];
  }

  --num_notes;
}

static void print_notes(void) 
{
  if (num_notes == 0) {
    printf("No notes on");
    return;
  }

  printf("Print notes, num_notes: %d\n", num_notes);
  for (uint8_t i=0; i<MAX_ARP_NOTES; i++)
  // for (uint8_t i=0; i<num_notes; i++)
  {
    printf("  notes[%d] = %x\n", i, notes[i].data1);
  }
}

static void print_sequence() 
{
  if (num_sequence_notes == 0) {
    printf("No notes in sequence");
    return;
  }
  for (uint8_t i=0; i<num_sequence_notes; i++)
  {
    printf("  sequence[%d] = %x\n", i, sequence[i].data1);
  }
}

static void clear_notes_old(void) 
{
  for (uint8_t i=0; i<MAX_ARP_NOTES; i++)
  {
    notes[i].data1 = 0xff;
  }
}

// Basic insertion sort on the notes array
static void sort_notes_old(void) 
{
  static int8_t i = 0;
  static int8_t j = 0;
  static Note temp;
  
  i=0; j=0;
  while (i < MAX_ARP_NOTES)
  {
    temp = notes[i];
    j = i - 1;
    while (j >= 0 && (notes[j].data1 > temp.data1))
    {
      notes[j+1] = notes[j];
      --j;
    }
    notes[j+1] = temp;
    ++i;
  }
}

