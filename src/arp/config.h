#pragma once
#ifndef ARP_CONFIG_
#define ARP_CONFIG_

#include "daisy_seed.h"

using namespace daisy;

enum Pattern {
    UP = 0, DOWN = 1, UPDOWN = 2, RANDOM = 3, LAST_PATTERN = 4
};

const bool arp_debug = false;

namespace arpeggiator {

class Config
{
public:
    Pattern pattern = UP;
    uint8_t max_octaves = 3;
    uint8_t octaves = 2;
    uint16_t bpm = 300;
    bool hold = false;
    uint8_t swing = 0;
    uint8_t channel = 0;
};
}

#endif