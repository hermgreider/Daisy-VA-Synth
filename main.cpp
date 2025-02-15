/*
This code is a modification of OscPocketD/VASynth, created by Staffan Melin, staffan.melin@oscillator.se.
It was later modified by (Steven @moonfriendsynth) to work with the Daisy Pod.
I (Christopher @Nettech15) have modified it further to run on the Daisy Seed without the use of the Daisy Pod board.
Synth parameters are now controlled by a Miditech i2-61 midi keyboard.
Feel free to copy, modify, and improve this code to match your equipment and sound requirements.
*/

#include "daisy_seed.h"
#include "daisysp.h"

#include "main.h"
#include "vasynth.h"
#include "synthui.h"
#include "midihandlerreface.h"

using namespace daisy;
using namespace daisysp;

// globals
DaisySeed hardware;
float sysSampleRate;
float sysCallbackRate;
extern uint8_t preset_max;
extern VASynthSetting preset_setting[PRESET_MAX];

uint8_t param_;
float pitch_bend = 1.0f;
float master_tune = 0.0f;

UartHandler         uart;
char tx[201];

// sound
VASynth vasynth;
uint8_t gPlay = PLAY_ON;

// fx
DelayLine<float, DELAY_MAX> DSY_SDRAM_BSS delay_;

SynthUI synthUI;
MidiHandlerReface midiHandler;

// audio callback

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{    
    float voice_left, voice_right;
    
    for (size_t n = 0; n < size; n += 2)
    {    
        if (gPlay == PLAY_ON)
        {            
            vasynth.Process(&voice_left, &voice_right);
            
            out[n] = voice_left + in[n];
            out[n + 1] = voice_right + in[n + 1];
        } 
        else 
        {
            out[n] = 0;
            out[n + 1] = 0;        
        }        
    }
}

// For debug with Salae Logic Analyzer
void configureUART() {
    UartHandler::Config config;
    config.baudrate = 9600 ;
    config.periph   = UartHandler::Config::Peripheral::USART_1;
    config.stopbits      = UartHandler::Config::StopBits::BITS_1;
    config.parity        = UartHandler::Config::Parity::NONE;
    config.mode          = UartHandler::Config::Mode::TX_RX;
    config.wordlength    = UartHandler::Config::WordLength::BITS_8;
    config.pin_config.rx = Pin(PORTB, 7);  // (USART_1 RX) Daisy pin 15
    config.pin_config.tx = Pin(PORTB, 6);  // (USART_1 TX) Daisy pin 14

    uart.Init(config);
}

int main(void)
{
    // init hardware
    hardware.Init(true); // true = boost to 480MHz
    hardware.SetAudioBlockSize(1);
    hardware.StartLog();

    // configureUART();

    sysSampleRate = hardware.AudioSampleRate();
    sysCallbackRate = hardware.AudioCallbackRate();

    // setup vasynth initial values
    vasynth.Init();

    // load the default patch
    vasynth.First(0);

    // Initialize USB Midi 
    midiHandler.Init();

    // let everything settle
    System::Delay(100);
    
    // Stereo simulator
    delay_.Init();
    delay_.SetDelay(sysSampleRate * 0.01f);

    // Start calling the audio callback
    hardware.StartAudio(AudioCallback);

    synthUI.Init();
    hardware.PrintLine("hello");

    // Loop forever
    for(;;)
    {
        midiHandler.Refresh();
        synthUI.Refresh();
    }
}
