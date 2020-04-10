#include "lfo.h"

LFO::LFO() 
    : m_freq(0.f), m_gain(0.f), m_type(SINE_WAVE_LFO),
      m_adsr(new SynthADSR()), m_enable_adsr(false){}

LFO::~LFO() {
    delete m_adsr;
}

LFO::LFO(LFO const &lfo)
    : m_freq(lfo.m_freq), m_gain(lfo.m_gain), m_type(lfo.m_type), 
      m_adsr(lfo.m_adsr), m_enable_adsr(lfo.m_enable_adsr) {}

