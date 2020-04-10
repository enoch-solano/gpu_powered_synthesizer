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
    float m_freq, m_gain, m_vert_offset;
    int m_type;

public:
    LFO();
    ~LFO();
    LFO(LFO const&);

    void set_level(float gain)       { m_gain = gain; }
    void set_rate(float freq)        { m_freq = freq; }
    void set_type(int type)          { m_type = type; }
    void set_vert_offset(int offset) { m_vert_offset = offset; }

    float process(const float &angle);
    void batch_gain_process(int num, float *output, const float &angle);

    float sin_wave(const float &angle);
    float square_wave(const float &angle);
    float triangle_wave(const float &angle);
    float sawtooth_wave(const float &angle);

    float lerp(const float &x, const float &y, const float &t);
    float clamp(const float &v, const float &lo, const float &hi);
};

inline float LFO::lerp(const float &x, const float &y, const float &t) {
    return x + t*(y-x);
}

inline float LFO::clamp(const float &v, const float &lo, const float &hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

inline float LFO::sin_wave(const float &angle) {
    return std::sin(angle * m_freq) * m_gain + m_vert_offset;
}

inline float LFO::square_wave(const float &angle) {
    float duty = 0.5;   // 50% percent
    return ((TWO_PI * duty > std::fmod(angle * m_freq, TWO_PI)) ? 0.f : m_gain) + m_vert_offset;
}

inline float LFO::triangle_wave(const float &angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return (t < 0.5f ? this->lerp(0.f, m_gain, t * 2) : this->lerp(0.f, m_gain, (t - 0.5f) * 2)) + m_vert_offset;
}

inline float LFO::sawtooth_wave(const float &angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return (this->lerp(0.f, m_gain, t)) + m_vert_offset;
}

inline float LFO::process(const float &angle) {
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

inline void LFO::batch_gain_process(int num, float *output, const float &angle) {
	for (int i = 0; i < num; i++) {
		output[i] += process(angle + 2.f * M_PI * i / 44100.f);
        output[i] = clamp(output[i], -1.f, 1.f);
	}
}

