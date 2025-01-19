#include "daisy_seed.h"
#include "oled_ssd1327.h"
#include <string>

#include "main.h"
#include "vasynth.h"
#include "midihandlerreface.h"
#include "synthui.h"

// using namespace std;
using namespace daisy;

// globals
extern DaisySeed hardware;
extern VASynth vasynth;
extern uint8_t reface_mode;

// OLED display
using MyOledDisplay = OledDisplay<SSD1327I2c128x128Driver>;
MyOledDisplay display;

// String mappings
string reface_modes[] = { "Oscillators", "Performance", "VCF        ", "VCA        ", "LFOPWM     ", "Arpeggiator"};
string waves[] = { "Tri   ", "Saw   ", "", "Square", "", "PSaw  " };

uint8_t current_reface_mode = 100;

// Save last refresh time
uint32_t last_second = 0;

// Indicates the display should update on next refresh
uint8_t update_display = 1;

char buf[201];


void SynthUI::Init()
{
    ConfigureOLED();
    WriteString("Startup", 0, 0);
    display.Update();
}

void SynthUI::ConfigureOLED() 
{
   /** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    disp_cfg.driver_config.transport_config.i2c_address               = 0x3D; // man, this was hard to find!
    disp_cfg.driver_config.transport_config.i2c_config.periph         = I2CHandle::Config::Peripheral::I2C_1;
    disp_cfg.driver_config.transport_config.i2c_config.speed          = I2CHandle::Config::Speed::I2C_1MHZ;
    disp_cfg.driver_config.transport_config.i2c_config.mode           = I2CHandle::Config::Mode::I2C_MASTER;
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl = Pin(PORTB, 8);  
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda = Pin(PORTB, 9);

    /** And Initialize */
    display.Init(disp_cfg);
    display.Fill(false);
}

void SynthUI::WriteString(string str, uint8_t col, uint8_t row) 
{
    display.SetCursor(col, row * 15 + 1);
    display.WriteString(str.c_str(), Font_7x10, true);
}

void SynthUI::WriteBuf(char * buf, uint8_t col, uint8_t row) 
{
    display.SetCursor(col, row * 15 + 1);
    display.WriteString(buf, Font_7x10, true);
}

uint32_t refresh_count = 0;

void SynthUI::Refresh() 
{
    uint32_t now = System::GetNow();
    if (last_second + 1000 < now) {
    	last_second = now - (now % 1000);

        if (update_display == 0) {
            return;
        }
        update_display = 0;

        // snprintf(buf, 200, "%ld\n", refresh_count++);
        // WriteBuf(buf, 100, 0);

        if (reface_mode != current_reface_mode) {
            current_reface_mode = reface_mode;
            DrawUI();
        }

        UpdateValues();
        display.Update();
    }
}

void SynthUI::DrawUI()
{
    display.Fill(false);
    WriteString(GetRefaceMode(), 0, 0);
    if (reface_mode == OSC) {
        WriteString("Mix", 12, 1);
        WriteString("O1", 12, 2);
        WriteString("O2", 12, 3);
        WriteString("Detune", 12, 4);
        WriteString("Scale", 12, 5);
        WriteString("O1 PW", 12, 6);
        WriteString("O2 PW", 12, 7);
    }
    else if (reface_mode == PERF) {
        WriteString("Mod", 12, 1);
        WriteString("Pitch", 12, 2);
        WriteString("Cutoff", 12, 3);
        WriteString("Reso", 12, 4);
    }
    else if (reface_mode == VCF) {
        WriteString("Amount", 12, 1);
        WriteString("Attack", 12, 2);
        WriteString("Decay", 12, 3);
        WriteString("VelSel", 12, 4);
        WriteString("Sus", 12, 5);
        WriteString("Rel", 12, 6);
        WriteString("KeyFol", 12, 7);
    }
    else if (reface_mode == VCA) {
        WriteString("n/a", 12, 1);
        WriteString("Attack", 12, 2);
        WriteString("Decay", 12, 3);
        WriteString("Sus", 12, 4);
        WriteString("Rel", 12, 5);
        WriteString("EnvFol", 12, 6);
    }
    else if (reface_mode == LFOPWM) {
        WriteString("FiAmRate", 12, 1);
        WriteString("FiAmLVL", 12, 2);
        WriteString("FiAmWave", 12, 3);
        WriteString("PWMrate", 12, 4);
        WriteString("PWMLVL", 12, 5);
        WriteString("PWM2ra", 12, 6);
        WriteString("PMW2LVL", 12, 7);
        WriteString("Modrate", 12, 8);
    }
    else if (reface_mode == ARP) {
        WriteString("BPM", 12, 1);
        WriteString("Pattern", 12, 2);
        WriteString("Octaves", 12, 3);
        WriteString("Swing", 12, 4);
        WriteString("Length", 12, 5);
    }
}

void SynthUI::UpdateValues() 
{
    if (reface_mode == OSC) {
        snprintf(buf, 200, "%.3f\n", vasynth.osc_mix_);
        WriteBuf(buf, 60, 1);

        WriteString(waves[vasynth.waveform_-1], 60, 2);
        WriteString(waves[vasynth.osc2_waveform_-1], 60, 3);
        
        snprintf(buf, 200, "%.3f\n", vasynth.osc2_detune_);
        WriteBuf(buf, 60, 4);

        snprintf(buf, 200, "%.3f\n", vasynth.osc2_transpose_);
        WriteBuf(buf, 60, 5);

        snprintf(buf, 200, "%.3f\n", vasynth.osc_pw_);
        WriteBuf(buf, 60, 6);

        snprintf(buf, 200, "%.3f\n", vasynth.osc2_pw_);
        WriteBuf(buf, 60, 7);
    }
    else if (reface_mode == PERF) {
        // Mod
        snprintf(buf, 200, "%.3f\n", vasynth.lfo_amp_);
        WriteBuf(buf, 60, 1);

        // Pitch

        snprintf(buf, 200, "%.3f\n", vasynth.filter_cutoff_);
        WriteBuf(buf, 60, 3);

        snprintf(buf, 200, "%.3f\n", vasynth.filter_res_);
        WriteBuf(buf, 60, 4);
    }
    else if (reface_mode == VCF) {
        snprintf(buf, 200, "%.3f\n", vasynth.eg_f_amount_);
        WriteBuf(buf, 60, 1);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_f_attack_);
        WriteBuf(buf, 60, 2);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_f_decay_);
        WriteBuf(buf, 60, 3);

        snprintf(buf, 200, "%s\n", (vasynth.vel_select_ >= 2) ? "on " : "off");
        WriteBuf(buf, 60, 4);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_f_sustain_);
        WriteBuf(buf, 60, 5);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_f_release_);
        WriteBuf(buf, 60, 6);

        snprintf(buf, 200, "%.3f\n", vasynth.vcf_kbd_follow_);
        WriteBuf(buf, 60, 7);
    }
    else if (reface_mode == VCA) {
        snprintf(buf, 200, "%.3f\n", vasynth.eg_a_attack_);
        WriteBuf(buf, 60, 2);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_a_decay_);
        WriteBuf(buf, 60, 3);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_a_sustain_);
        WriteBuf(buf, 60, 4);

        snprintf(buf, 200, "%.3f\n", vasynth.eg_a_release_);
        WriteBuf(buf, 60, 5);

        snprintf(buf, 200, "%.3f\n", vasynth.env_kbd_follow_);
        WriteBuf(buf, 60, 6);
    }
    else if (reface_mode == LFOPWM) {

        snprintf(buf, 200, "%.3f\n", vasynth.vcavcflfo_freq_);
        WriteBuf(buf, 70, 1);

        snprintf(buf, 200, "%.3f\n", vasynth.vcavcflfo_amp_);
        WriteBuf(buf, 70, 2);

        WriteString(waves[vasynth.vcavcflfo_waveform_-1], 60, 3);

        snprintf(buf, 200, "%.3f\n", vasynth.pwmlfo_freq_);
        WriteBuf(buf, 70, 4);

        snprintf(buf, 200, "%.3f\n", vasynth.pwmlfo_amp_);
        WriteBuf(buf, 70, 5);

        snprintf(buf, 200, "%.3f\n", vasynth.pwm2lfo_freq_);
        WriteBuf(buf, 70, 6);

        snprintf(buf, 200, "%.3f\n", vasynth.pwm2lfo_amp_);
        WriteBuf(buf, 70, 7);

        snprintf(buf, 200, "%.3f\n", vasynth.lfo_freq_);
        WriteBuf(buf, 70, 8);

    }
}

void SynthUI::UpdateDisplay() 
{
    update_display = 1;
}

void SynthUI::NoteOn(uint8_t note) 
{
    string notename = MidiNoteToName(note);
    WriteString(notename, 10, 10);
}

string SynthUI::GetRefaceMode() 
{
    return reface_modes[current_reface_mode];
}

string SynthUI::MidiNoteToName(uint8_t midi_note) {
    const string noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    int octave = (midi_note / 12) - 1;
    int noteIndex = midi_note % 12;

    return noteNames[noteIndex] + to_string(octave);
}

