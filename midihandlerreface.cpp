#include "daisy_seed.h"

#include "main.h"
#include "vasynth.h"
#include "synthui.h"
#include "midihandlerreface.h"

using namespace daisy;

extern VASynth vasynth;
extern SynthUI synthUI;

MidiUsbHandler midi;
uint8_t reface_mode = 0; // Osc (0), Performance (1), VCF (51), VCA (76), LFO/PWM (102), Effects (127)

void MidiHandlerReface::Init()
{
    MidiUsbHandler::Config midi_cfg;
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    midi.Init(midi_cfg);

}

void MidiHandlerReface::Refresh()
{        
    // handle MIDI Events
    midi.Listen();
    while(midi.HasEvents())
    {
        HandleMidiMessage(midi.PopEvent());
    }    
}

// MIDI Messages for Yamaha Reface CP. To use CC, enable MIDI CC mode with POWER OFF, Hold low E, POWER ON
void MidiHandlerReface::HandleMidiMessage(MidiEvent m)
{
    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
        
            if ((vasynth.midi_channel_ == MIDI_CHANNEL_ALL) || (p.channel == vasynth.midi_channel_))
            {
                vasynth.NoteOn(p.note+1, p.velocity);
                // hardware.SetLed(true);
            }
            break;
        }
        case NoteOff:
        {
            NoteOnEvent p = m.AsNoteOn();

            if ((vasynth.midi_channel_ == MIDI_CHANNEL_ALL) || (p.channel == vasynth.midi_channel_))
            {
                vasynth.NoteOff(p.note+1);
                // hardware.SetLed(false);
            }            
            break;
        } 
        case ControlChange:
        {
            ControlChangeEvent p = m.AsControlChange();

            // snprintf(tx, 100, "CC %d val %d\n", p.control_number, p.value);

            switch(p.control_number)
            {
                case 80:
                    // Osc (Value 0), Filter (25), VCF (51), VCA (76), LFO/PWM (102), Effects (127)
                    if (p.value == 0) {
                        reface_mode = OSC;
                    }
                    else if (p.value == 25) {
                        reface_mode = PERF;
                    }
                    else if (p.value == 51) {
                        reface_mode = VCF;
                    }
                    else if (p.value == 76) {
                        reface_mode = VCA;
                    }
                    else if (p.value == 102) {
                        reface_mode = LFOPWM;
                    }
                    else if (p.value == 127) {
                        reface_mode = ARP;
                    }
                    synthUI.UpdateDisplay();

                    break;

                case 81:
                    // Reface Drive (81)
                    if (reface_mode == OSC) {
                        // Osc Mix
                        vasynth.osc_mix_ = ((float)p.value / 127.0f);
                    }
                    else if (reface_mode == PERF) {
                        // Mod Wheel (LFO Amp)
                        vasynth.lfo_amp_ = ((float)p.value / 127.0f);
                        vasynth.SetLFO();
                    }
                    else if (reface_mode == VCF) {
                        // VCF Env amount
                        vasynth.eg_f_amount_ = ((float)p.value / 127.0f);
                    }
                    else if (reface_mode == VCA) {
						// Unused
                    }
                    else if (reface_mode == LFOPWM) {
                        // VCA/VCF LFO rate
                        vasynth.vcavcflfo_freq_ = ((float)p.value / 127.0f);
                        vasynth.SetVCAVCFLFO();
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 17: 
					// Reface Tremelo Switch

                    if (reface_mode == VCF) {
                        // Velecity Select
						if (p.value < 64) {
							vasynth.vel_select_ = 1;
						}
						else {
							vasynth.vel_select_ = 2;
						}
					}

                case 18: 
                    // Reface Tremelo Depth (18): 
                    
                    if (reface_mode == OSC) {
                        // Osc1 Waveform: Tri (<5), Saw (>40), Square (>80), PolyBLEP (>120)
                        if (p.value < 5) {
                            vasynth.waveform_ = WAVE_TRI;
                        }
                        else if (p.value < 40) {
                            vasynth.waveform_ = WAVE_SAW;
                        }
                        else if (p.value < 80) {
                            vasynth.waveform_ = WAVE_SQUARE;
                        }
                        else {
                            vasynth.waveform_ = WAVE_POLYBLEP_SAW;
                        }
                        vasynth.SetWaveform();
                    }
                    else if (reface_mode == PERF) {
                        // Filter Cutoff
                        vasynth.filter_cutoff_ = ((float)p.value * (18000.0f / 127.0f));
                        vasynth.SetFilter();
                    }
                    else if (reface_mode == VCF) {
                        // VCF Attack
                        vasynth.eg_f_attack_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == VCA) {
						// VCA Attack
                        vasynth.eg_a_attack_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == LFOPWM) {
                        // VCA/VCF LFO Amp level
                        vasynth.vcavcflfo_amp_ = ((float)p.value / 127.0f);
                        vasynth.SetVCAVCFLFO();
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 19: 
                    // Reface Tremelo Rate (19): 

                    if (reface_mode == OSC) {
                        // Osc2 Waveform: Tri (<5), Saw (>40), Square (>80), PolyBLEP (>120)
                        if (p.value < 5) {
                            vasynth.osc2_waveform_ = WAVE_TRI;
                        }
                        else if (p.value < 40) {
                            vasynth.osc2_waveform_ = WAVE_SAW;
                        }
                        else if (p.value < 80) {
                            vasynth.osc2_waveform_ = WAVE_SQUARE;
                        }
                        else {
                            vasynth.osc2_waveform_ = WAVE_POLYBLEP_SAW;
                        }
                        vasynth.SetWaveform();
                    }
                    else if (reface_mode == PERF) {
                        // Resonance
                        vasynth.filter_res_ = ((float)p.value / 127.0f);
                        vasynth.SetFilter();
                    }
                    else if (reface_mode == VCF) {
                        // VCF Decay
                        vasynth.eg_f_decay_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == VCA) {
                        // VCF Decay
                        vasynth.eg_a_decay_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == LFOPWM) {
						// VCA/VCF LFO Waveform Select
                        if (p.value < 5) {
                            vasynth.vcavcflfo_waveform_ = WAVE_TRI;
                        }
                        else if (p.value < 40) {
                            vasynth.vcavcflfo_waveform_ = WAVE_SAW;
                        }
                        else if (p.value < 80) {
                            vasynth.vcavcflfo_waveform_ = WAVE_SQUARE;
                        }
                        else {
                            vasynth.vcavcflfo_waveform_ = WAVE_POLYBLEP_SAW;
                        }
                        vasynth.SetVCAVCFLFO();
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 86:
                    // Reface Chorus Depth (86)
                    if (reface_mode == OSC) {
                        // Osc2 De-Tune
                        vasynth.osc2_detune_ = ((float)p.value / 255.0f);
                    }
                    else if (reface_mode == PERF) {
                        // Pitch Bend
                        PitchBendEvent p = m.AsPitchBend();
                        vasynth.PitchBend(p.value);    
                    }
                    else if (reface_mode == VCF) {
                        // VCF Sustain
                        vasynth.eg_f_sustain_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == VCA) {
                        // VCA Sustain
                        vasynth.eg_a_sustain_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == LFOPWM) {
						// PWM LFO rate
                        vasynth.pwmlfo_freq_ = ((float)p.value / 127.0f);
                        vasynth.SetPWMLFO();						
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 87:
                    // Reface Chorus Speed (87)
                    if (reface_mode == OSC) {
                        // Osc2 Scale
                        vasynth.osc2_transpose_ = (1.0f + ((float)p.value / 127.0f));
                    }
                    else if (reface_mode == PERF) {
                        // Unused
                    }
                    else if (reface_mode == VCF) {
                        // VCF release
                        vasynth.eg_f_release_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == VCA) {
                        // VCF Release
                        vasynth.eg_a_release_ = ((float)p.value / 127.0f);
                        vasynth.SetEG();
                    }
                    else if (reface_mode == LFOPWM) {
                        // PWM LFO amp level
                        vasynth.pwmlfo_amp_ = ((float)p.value / 511.0f);
                        vasynth.SetPWMLFO();						
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 89:
                    // Delay Depth (89): 
                    if (reface_mode == OSC) {
                        // Osc1 PW
                        vasynth.osc_pw_ = ((float)p.value / 255.0f);
                    }
                    else if (reface_mode == PERF) {
                        // Unused
                    }
                    else if (reface_mode == VCF) {
                        // VCF Key Follow Level
                        vasynth.vcf_kbd_follow_= ((float)p.value / 127.0f);
                    }
                    else if (reface_mode == VCA) {
                        // ENV Key Follow Level
                        vasynth.env_kbd_follow_ = ((float)p.value / 127.0f);
                    }
                    else if (reface_mode == LFOPWM) {
                        // PWM2 LFO rate
                        vasynth.pwm2lfo_freq_ = ((float)p.value / 127.0f);
                        vasynth.SetPWM2LFO();						
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 90:
                    // Delay Time (90): 
                    if (reface_mode == OSC) {
                        // Osc2 PW
                        vasynth.osc2_pw_ = ((float)p.value / 255.0f);                    
                    }
                    else if (reface_mode == PERF) {
                        // Unused
                    }
                    else if (reface_mode == VCF) {
                        // Unused
                    }
                    else if (reface_mode == VCA) {
                    }
                    else if (reface_mode == LFOPWM) {
                        // PWM2 LFO amp level
                        vasynth.pwm2lfo_amp_ = ((float)p.value / 511.0f);
                        vasynth.SetPWM2LFO();						
                    }

                    synthUI.UpdateDisplay();
                    break;

                case 91:
                    // Reverb (91)
                    if (reface_mode == LFOPWM) {
						// Mod Wheel LFO rate
                        vasynth.lfo_freq_ = ((float)p.value / 127.0f);
                        vasynth.SetLFO();
                    }

                    synthUI.UpdateDisplay();
                    break;

            }
        }
        
        default:
            break;
    }
}
