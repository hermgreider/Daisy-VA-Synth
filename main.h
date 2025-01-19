#pragma once

#include "daisysp.h"

/* Defines ------------------------------------------------------------------*/
#define VOICES_MAX 5
#define FILTER_CUTOFF_MAX 18000.0f

// play status
#define PLAY_OFF 0
#define PLAY_ON 1

// delay
#define DELAY_MAX static_cast<size_t>(48000 * DELAY_MAX_S)
#define DELAY_MAX_S 1.0f // delay max in seconds

