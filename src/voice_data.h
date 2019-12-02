#pragma once

#include "constants.h"

struct voice_data {
	float freqs[NUM_VOICES * NUM_HARMS];	// contains freqs for every harmonic of every voice
	float gains[NUM_VOICES * NUM_HARMS];	// contains corresponding gain for every harmonic

	float v_gains[NUM_VOICES];				// gain for overall voice
};
