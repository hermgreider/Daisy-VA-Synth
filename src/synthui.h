#pragma once
#ifndef SYNTHUI_H
#define SYNTHUI_H

#include "daisy_seed.h"

using namespace std;

class SynthUI
{
	public:

    SynthUI() {}
    ~SynthUI() {}

	void Init();
	void ConfigureOLED();
	void WriteString(string str, uint8_t x, uint8_t y, FontDef font);
	void WriteBuf(char * buf, uint8_t x, uint8_t y, FontDef font);
	void WriteString(string str, uint8_t x, uint8_t y);
	void WriteBuf(char * buf, uint8_t x, uint8_t y);
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
