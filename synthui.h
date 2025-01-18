#pragma once
#ifndef SYNTHUI_H
#define SYNTHUI_H

// #include <string.h>
#include "daisy_seed.h"

using namespace std;

class SynthUI
{
	public:

    SynthUI() {}
    ~SynthUI() {}

	void Init();
	void ConfigureOLED();
	void WriteString(string buf, uint8_t row, uint8_t col);
	void WriteBuf(char * buf, uint8_t row, uint8_t col);
	void Refresh();
	void NoteOn(uint8_t note);
	void NoteOff(uint8_t note);
	string GetRefaceMode();
	void DrawUI();
    void UpdateValues();
	void UpdateDisplay();
	string MidiNoteToName(uint8_t midiNote);
};

#endif
