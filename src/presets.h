#pragma once

#include "voice_data.h"

void load_square_wave(int v_idx, voice_data& v_user_data) {
	float f = 220;

	for (int i = 0; i < NUM_HARMS; i++) {
		v_user_data.gains[v_idx*NUM_HARMS + i] = 1.f / (1.f + (2 * i));
		v_user_data.freqs[v_idx*NUM_HARMS + i] = (1.f + (2 * i)) * f;
	}

	Additive::updateFreqsVSynth(v_user_data.freqs);
	Additive::updateGainsVSynth(v_user_data.gains);
}

void load_sawtooth(int v_idx, voice_data& v_user_data) {
	float f = 220;
	float L = 1;

	for (int i = 0; i < NUM_HARMS; i++) {
		v_user_data.gains[v_idx*NUM_HARMS + i] = (-1.f / (_PI * (i + 1)));
		v_user_data.freqs[v_idx*NUM_HARMS + i] = (i * _PI / L) * f;
	}

	Additive::updateFreqsVSynth(v_user_data.freqs);
	Additive::updateGainsVSynth(v_user_data.gains);
}