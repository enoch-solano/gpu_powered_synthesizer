#pragma once

#include "constants.h"
#include "kernel/kernel.h"
#include <iostream>
#include <JuceHeader.h>
#include "SynthADSR.h"

class Engine {
     private:
	    static Engine *engine;
        
         float2 *h_freq_gains;    // contains freqs for every harmonic of every voice
         float *h_angles;
         float *h_v_gains;      // gain for overall voice
         float *h_buffer;
         float *h_tmp_buffer;
         float* samples;
         float *fundamental_freqs;
         float time;

	SynthADSR *adsr;
       
         int *freq_ratios;
         int num_samples;
         int num_sinusoids;
         int num_harms;
         int num_voices; 
         float angle = 0;

       
         Engine(int num_samples);  
         ~Engine();
         Engine(Engine const&){};
         Engine& operator=(Engine const&){};
         void update_freqs();
         void realloc_engine(int num_samples);
    public: 
    void update_fundamental(int v_idx, float freq);
	     void process_adsr(void* outputBuffer);
          void toggleMute(int v_idx, float voicelvl);
static Engine* getInstance();
         static Engine* getInstance(int num_samples);
         void load_sawtooth(int v_idx, int f);
         void load_square_wave(int v_idx, int f);
         void tick(void* outputBuffer);
         void simple_tick(void* outputBuffer, int num_Samples);
         void update_voice_gain(int v_idx, float gain);
         void update_harmonics(int v_idx, int harmonic, float gain);
         float get_freq(int v_idx, int harmonic);
         float get_gain(int v_idx, int harmonic);
         void gate_off();
         void gate_on();
         void load_sinewave(int v_idx, int f);
};
