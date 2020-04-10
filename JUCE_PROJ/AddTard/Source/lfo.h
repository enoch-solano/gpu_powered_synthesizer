#pragma once
#include <string>
#include <cmath>
#include "SynthADSR.h"
#include "constants.h"

#define NUM_LFO_WAVES 4

#define SINE_WAVE_LFO     0
#define SQUARE_WAVE_LFO   1
#define TRIANGLE_WAVE_LFO 2
#define SAWTOOTH_WAVE_LFO 3

#define TWO_PI 2.f * M_PI

class LFO {
private:
    float m_freq, m_gain;
    int m_type;

    SynthADSR *m_adsr;
    bool m_enable_adsr;

public:
    LFO();
    ~LFO();
    LFO(LFO const&);

    void set_level(float gain)       { m_gain = gain; }
    void set_rate(float freq)        { m_freq = freq; }
    void set_type(int type)          { m_type = type; }

    float process(const float &angle);
    void batch_gain_process(int num, float *output, const float &angle);

    float sin_wave(const float &angle);
    float square_wave(const float &angle);
    float triangle_wave(const float &angle);
    float sawtooth_wave(const float &angle);

    float lerp(const float &x, const float &y, const float &t);
    float clamp(const float &v, const float &lo, const float &hi);

    // ADSR functionality
    void toggle_adsr() { m_enable_adsr = !m_enable_adsr; }
    void gate_on()  { m_adsr->gate(ON_G); }
    void gate_off() { m_adsr->gate(OFF_G); }

    void setAttackRate(float rate)    { m_adsr->setAttackRate(rate * SAMPLING_FREQUENCY); }
    void setDecayRate(float rate)     { m_adsr->setDecayRate(rate * SAMPLING_FREQUENCY); }
    void setReleaseRate(float rate)   { m_adsr->setReleaseRate(rate * SAMPLING_FREQUENCY); }
    void setSustainLevel(float level) { m_adsr->setSustainLevel(level); }
};

inline float LFO::lerp(const float &x, const float &y, const float &t) {
    return x + t*(y-x);
}

inline float LFO::clamp(const float &v, const float &lo, const float &hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

inline float LFO::sin_wave(const float &angle) {
    return std::sin(angle * m_freq) * m_gain;
}

inline float LFO::square_wave(const float &angle) {
    float duty = 0.5;   // 50% percent
    return ((TWO_PI * duty > std::fmod(angle * m_freq, TWO_PI)) ? 0.f : m_gain);
}

inline float LFO::triangle_wave(const float &angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return (t < 0.5f ? this->lerp(-m_gain, m_gain, t * 2) : this->lerp(-m_gain, m_gain, (t - 0.5f) * 2));
}

inline float LFO::sawtooth_wave(const float &angle) {
    float t = std::fmod(angle * m_freq, TWO_PI);
    t /= TWO_PI;
    return (this->lerp(-m_gain, m_gain, t));
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
		output[i] += process(angle + 2.f * M_PI * i / 44100.f) * (m_enable_adsr ? m_adsr->process() : 1.f);
        output[i] = clamp(output[i], -1.f, 1.f);
	}
}

