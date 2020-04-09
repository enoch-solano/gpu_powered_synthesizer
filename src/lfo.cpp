#include "lfo.h"

LFO::LFO() 
    : m_freq(0.f), m_gain(0.f), m_type(SINE_WAVE_LFO) {}

LFO::~LFO() {}

LFO::LFO(LFO const &lfo)
    : m_freq(lfo.m_freq), m_gain(lfo.m_gain), m_type(lfo.m_type) {}