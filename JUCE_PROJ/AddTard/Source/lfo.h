#pragma once
#include <string>
#include <cmath>

#define NUM_LFO_WAVES 4

#define SINE_WAVE_LFO     0
#define SQUARE_WAVE_LFO   1
#define TRIANGLE_WAVE_LFO 2
#define SAWTOOTH_WAVE_LFO 3

#define TWO_PI 2.f * M_PI

class LFO {
public:
    float m_freq, m_gain;
    int m_type;

public:
    LFO();
    ~LFO();
    LFO(LFO const&);

    void set_level(float gain) { m_gain = gain; }
    void set_rate(float freq)  { m_freq = freq; }
    void set_type(int type)    { m_type = type; }

    float process(float angle);
    void batch_gain_process(int num, float *output, float angle);

    float sin_wave(float angle);
    float square_wave(float angle);
    float triangle_wave(float angle);
    float sawtooth_wave(float angle);

    float lerp(float x, float y, float t);
};

inline float LFO::lerp(float x, float y, float t) {
    return x + t*(y-x);
}

inline float LFO::sin_wave(float angle) {
    return (std::sin(angle * m_freq) + 0.5f) * m_gain;
}

inline float LFO::square_wave(float angle) {
    float duty = 0.5;   // 50% percent
    return (TWO_PI * duty > std::fmod(angle * m_freq, TWO_PI)) ? 0.f : m_gain;
}

inline float LFO::triangle_wave(float angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return t < 0.5f ? this->lerp(0.f, m_gain, t * 2) : this->lerp(0.f, m_gain, (t - 0.5f) * 2);
}

inline float LFO::sawtooth_wave(float angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return this->lerp(0.f, m_gain, t);
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
	case TRIANGLE_WAVE_LFO:
        output = triangle_wave(angle);
		break;
	case SAWTOOTH_WAVE_LFO:
        output = sawtooth_wave(angle);
		break;
	}

	return output;
}

inline void LFO::batch_gain_process(int num, float *output, float angle) {
	for (int i = 0; i < num; i++) {
		output[i] *= process(angle + 2.f * M_PI * i / 44100.f);
	}
}

