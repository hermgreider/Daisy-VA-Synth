#include "daisy_seed.h"
#include "oled_ssd1327.h"

#include "main.h"
#include "vasynth.h"
#include "midihandlerreface.h"
#include "synthui.h"

using namespace daisy;

// globals
extern DaisySeed hardware;
extern VASynth vasynth;
extern uint8_t reface_mode;

// OLED display
using MyOledDisplay = OledDisplay<SSD13274WireSpi128x128Driver>;

MyOledDisplay display;

// String mappings
string reface_modes[] = { "Oscillators", "Filter     ", "Amplifier   ", "LFOPWM     ", "Effects    ", "Arpeggiator"};
string waves[] = { "Tri   ", "Saw   ", "", "Square", "", "PSaw  " };
string fx_states[] = { "None   ", "Chorus  ", "Flanger ", "Autowah  ", "Phaser  ", "Reverb  " };

uint8_t current_reface_mode = 100;

// Save last refresh time
uint32_t last_second = 0;

// Indicates the display should update on next refresh
uint8_t update_display = 1;

char buf[201];

void SynthUI::Init()
{
    ConfigureOLED();
    WriteString("Startup", 0, 0, Font_7x10);
    display.Update();
}

void SynthUI::ConfigureOLED() 
{
    /** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    disp_cfg.driver_config.transport_config.pin_config.dc    = Pin(PORTB, 4);
    disp_cfg.driver_config.transport_config.pin_config.reset = Pin(PORTB, 15);

    /** And Initialize */
    display.Init(disp_cfg);
    display.Fill(false);
}

void SynthUI::WriteString(string str, uint8_t x, uint8_t y) {} 
void SynthUI::WriteBuf(char * buf, uint8_t x, uint8_t y) {} 

void SynthUI::WriteString(string str, uint8_t x, uint8_t y, FontDef font) 
{
    display.SetCursor(x, y);
    display.WriteString(str.c_str(), font, true);
}

void SynthUI::WriteBuf(char * buf, uint8_t x, uint8_t y, FontDef font) 
{
    display.SetCursor(x, y);
    display.WriteString(buf, font, true);
}

uint32_t refresh_count = 0;

void TestOLED1()
{
    display.Fill(false);
    display.SetCursor(0, 0);
    display.WriteString("Oscillators", Font_7x10, true);
    display.SetCursor(0, 22);
    display.WriteString("57", Font_11x18, true);
    display.SetCursor(0, 41);
    display.WriteString("Mix%", Font_6x8, true);

    display.SetCursor(28, 22);
    display.WriteString("1-SAW", Font_7x10, true);
    display.SetCursor(28, 40);
    display.WriteString("2-SQUARE", Font_7x10, true);
    display.SetCursor(90, 40);
    display.WriteString("100", Font_6x8, true);
    display.SetCursor(110, 40);
    display.WriteString("%PW", Font_6x8, true);

    display.SetCursor(0, 70);
    display.WriteString("Detune", Font_6x8, true);
    display.SetCursor(0, 80);
    display.WriteString("230", Font_7x10, true);

    display.SetCursor(50, 70);
    display.WriteString("Octaves", Font_6x8, true);
    display.SetCursor(50, 80);
    display.WriteString("+1", Font_7x10, true);
}

void TestOLED()
{
    display.Fill(false);
    display.SetCursor(0, 0);
    display.WriteString("Filter", Font_7x10, true);

    display.SetCursor(0, 18);
    display.WriteString("Level%", Font_6x8, true);
    display.SetCursor(0, 28);
    display.WriteString("100", Font_7x10, true);

    display.SetCursor(40, 18);
    display.WriteString("Cutoff", Font_6x8, true);
    display.SetCursor(40, 28);
    display.WriteString("3900", Font_7x10, true);

    display.SetCursor(80, 18);
    display.WriteString("Resonance", Font_6x8, true);
    display.SetCursor(80, 28);
    display.WriteString(".233", Font_7x10, true);

    display.SetCursor(0, 60);
    display.WriteString("Att", Font_6x8, true);
    display.SetCursor(0, 72);
    display.WriteString("210", Font_7x10, true);

    display.SetCursor(32, 60);
    display.WriteString("Dec", Font_6x8, true);
    display.SetCursor(32, 72);
    display.WriteString("340", Font_7x10, true);

    display.SetCursor(64, 60);
    display.WriteString("Sus", Font_6x8, true);
    display.SetCursor(64, 72);
    display.WriteString("30", Font_7x10, true);

    display.SetCursor(96, 60);
    display.WriteString("Rel", Font_6x8, true);
    display.SetCursor(96, 72);
    display.WriteString("110", Font_7x10, true);

    display.SetCursor(0, 90);
    display.WriteString("Velocity", Font_6x8, true);
    display.SetCursor(0, 102);
    display.WriteString("on", Font_7x10, true);

    display.SetCursor(64, 90);
    display.WriteString("KeyFollow", Font_6x8, true);
    display.SetCursor(64, 102);
    display.WriteString("off", Font_7x10, true);
}

void SynthUI::Refresh() 
{
    uint32_t now = System::GetNow();
    if (last_second + 300 < now) {
    	last_second = now - (now % 1000);

        if (update_display == 0) {
            return;
        }
        update_display = 0;

        if (reface_mode != current_reface_mode) {
            current_reface_mode = reface_mode;
            
            //temp
            vasynth.osc2_waveform_ = WAVE_SQUARE;

            DrawUI();
        }

        UpdateValues();
        display.Update();
    }
}

void SynthUI::DrawUI()
{
    display.Fill(false);
    WriteString(GetRefaceMode(), 0, 0, Font_7x10);
    if (reface_mode == OSC) {
        WriteString("1-", 0, 22, Font_11x18);
        WriteString("2-", 0, 45, Font_11x18);
        WriteString("Mix%", 0, 80, Font_6x8);
        WriteString("Detune", 42, 80, Font_6x8);
        WriteString("Octaves", 84, 80, Font_6x8);
    }
    else if (reface_mode == VCF) {
        WriteString("Cutoff", 0, 40, Font_6x8);
        WriteString("Resonance", 60, 40, Font_6x8);

        WriteString("Velocity", 0, 90, Font_6x8);
        WriteString("KeyFollow", 64, 90, Font_6x8);

        // Temp
        WriteString("Moog", 0, 18, Font_7x10);

        WriteString("3900", 0, 52, Font_11x18);
        WriteString("233", 60, 52, Font_11x18);

        WriteString("on", 0, 102, Font_7x10);
        WriteString("off", 64, 102, Font_7x10);
    }
    else if (reface_mode == ENV) {
        WriteString("Type", 0, 18, Font_6x8);

        WriteString("Cutoff", 40, 18, Font_6x8);
        WriteString("Resonance", 80, 18, Font_6x8);

        WriteString("Att", 0, 60, Font_6x8);
        WriteString("Dec", 32, 60, Font_6x8);
        WriteString("Sus", 64, 60, Font_6x8);
        WriteString("Rel", 96, 60, Font_6x8);

        WriteString("Velocity", 0, 90, Font_6x8);
        WriteString("KeyFollow", 64, 90, Font_6x8);

        // Temp
        WriteString("100", 0, 28, Font_7x10);
        WriteString("3900", 40, 28, Font_7x10);
        WriteString("233", 80, 28, Font_7x10);

        WriteString("210", 0, 72, Font_7x10);
        WriteString("340", 32, 72, Font_7x10);
        WriteString("30", 64, 72, Font_7x10);
        WriteString("110", 96, 72, Font_7x10);

        WriteString("on", 0, 102, Font_7x10);
        WriteString("off", 64, 102, Font_7x10);
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
        WriteString("FX", 12, 2);
        // WriteString("BPM", 12, 1);
        // WriteString("Pattern", 12, 2);
        // WriteString("Octaves", 12, 3);
        // WriteString("Swing", 12, 4);
        // WriteString("Length", 12, 5);
    }
    else if (reface_mode == FX) {
    }
}

void SynthUI::UpdateValues() 
{
    if (reface_mode == OSC) {
        snprintf(buf, 200, "%.4s\n", waves[vasynth.waveform_-1].c_str());
        WriteBuf(buf, 22, 22, Font_11x18);
        snprintf(buf, 200, "%.4s\n", waves[vasynth.osc2_waveform_-1].c_str());
        WriteBuf(buf, 22, 45, Font_11x18);

        if (vasynth.waveform_ == WAVE_SQUARE) {
            snprintf(buf, 200, "%3.0f\n", vasynth.osc_pw_ * 100.0f);
            WriteBuf(buf, 80, 30, Font_7x10);
            WriteString("%PW", 105, 30, Font_7x10);
        }
        else {
            WriteString("              ", 80, 30, Font_7x10);
        }

        if (vasynth.osc2_waveform_ == WAVE_SQUARE) {
            snprintf(buf, 200, "%3.0f\n", vasynth.osc2_pw_ * 100.0f);
            WriteBuf(buf, 80, 53, Font_7x10);
            WriteString("%PW", 105, 53, Font_7x10);
        }
        else {
            WriteString("              ", 80, 53, Font_7x10);
        }

        snprintf(buf, 200, "%-3.0f\n", vasynth.osc_mix_ * 100.0f);
        WriteString(buf, 0, 92, Font_7x10);
    
        snprintf(buf, 200, "%-3.0f\n", vasynth.osc2_detune_ * 1000.0f);
        WriteBuf(buf, 42, 92, Font_7x10);
    
        snprintf(buf, 200, "%1.0f\n", (vasynth.osc2_transpose_ - 1));
        WriteBuf(buf, 84, 92, Font_7x10);
    }
    // else if (reface_mode == PERF) {
    //     // Mod
    //     snprintf(buf, 200, "%.3f\n", vasynth.lfo_amp_);
    //     WriteBuf(buf, 60, 1);

    //     snprintf(buf, 200, "%.3f\n", vasynth.filter_cutoff_);
    //     WriteBuf(buf, 60, 2);

    //     snprintf(buf, 200, "%.3f\n", vasynth.filter_res_);
    //     WriteBuf(buf, 60, 3);

    //     // Pitch

    // }
    else if (reface_mode == VCF) {
        snprintf(buf, 200, "%3.1f %%\n", vasynth.eg_f_amount_*100.0);
        WriteBuf(buf, 60, 1);

        snprintf(buf, 200, "%3.0f ms\n", vasynth.eg_f_attack_*1000.0);
        WriteBuf(buf, 60, 2);

        snprintf(buf, 200, "%3.0f ms\n", vasynth.eg_f_decay_*1000.0);
        WriteBuf(buf, 60, 3);

        snprintf(buf, 200, "%s\n", (vasynth.vel_select_ >= 2) ? "on " : "off");
        WriteBuf(buf, 60, 4);

        snprintf(buf, 200, "%3.0f ms\n", vasynth.eg_f_sustain_*1000.0);
        WriteBuf(buf, 60, 5);

        snprintf(buf, 200, "%3.0f ms\n", vasynth.eg_f_release_*1000.0);
        WriteBuf(buf, 60, 6);

        snprintf(buf, 200, "%3.3f\n", vasynth.vcf_kbd_follow_);
        WriteBuf(buf, 60, 7);
    }
    else if (reface_mode == VCA) {
        snprintf(buf, 200, "%.0f ms\n", vasynth.eg_a_attack_*1000.0);
        WriteBuf(buf, 60, 2);

        snprintf(buf, 200, "%.0f ms\n", vasynth.eg_a_decay_*1000.0);
        WriteBuf(buf, 60, 3);

        snprintf(buf, 200, "%.0f ms\n", vasynth.eg_a_sustain_*1000.0);
        WriteBuf(buf, 60, 4);

        snprintf(buf, 200, "%.0f ms\n", vasynth.eg_a_release_*1000.0);
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
    else if (reface_mode == ARP) {
        WriteString(fx_states[vasynth.current_fx], 60, 2);
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

