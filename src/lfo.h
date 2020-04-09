#pragma once
#include <string>
#include <cmath>

#define NUM_LFO_WAVES 2

#define SINE_WAVE_LFO   0
#define SQUARE_WAVE_LFO 1

class LFO {
private:
    float m_freq, m_gain;
    int m_type;

public:
    LFO();
    ~LFO();
    LFO(LFO const&);

    void set_level(float gain)   { m_gain = gain; }
    void set_rate(float freq)    { m_freq = freq; }
    void set_type(int type) { m_type = type; }

    float process(float angle);
    void batch_gain_process(int num, float *output, float angle);

    float sin_wave(float angle);
    float square_wave(float angle);
};

inline float LFO::sin_wave(float angle) {
    return (std::sin(angle * m_freq) + 0.5f) * m_gain;
}

inline float LFO::square_wave(float angle) {
    return (std::sin(angle * m_freq) + 0.5f) * m_gain;
}

inline float LFO::process(float angle) {
    float output = 0;

    switch (m_type) {
	case SINE_WAVE_LFO:
        output = sin_wave(angle);
		break;
	case SQUARE_WAVE_LFO:
        output = square_wave(angle);
		break;
	}

	return output;
}

inline void LFO::batch_gain_process(int num, float *output, float angle) {
	for (int i = 0; i < num; i++) {
		output[i] *= process(angle + 2.f * M_PI * i / 44100.f);
	}
}
