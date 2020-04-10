#include "lfo.h"

LFO::LFO() 
    : m_freq(0.f), m_gain(0.f), m_type(SINE_WAVE_LFO), m_vert_offset(0.f) {}

LFO::~LFO() {}

LFO::LFO(LFO const &lfo)
    : m_freq(lfo.m_freq), m_gain(lfo.m_gain), m_type(lfo.m_type), m_vert_offset(lfo.m_vert_offset) {}
