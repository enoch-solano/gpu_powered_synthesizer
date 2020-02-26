#pragma once
#include "ADSR.h"
#include "constants.h"
#include "kernel.h"
#include <iostream>

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

         ADSR *adsr;
       
         int *freq_ratios;
         int num_samples;
         int num_sinusoids;
         int num_harms;
         int num_voices; 
         float angle = 0;
       
         Engine();  
         ~Engine();
         Engine(Engine const&){};
         Engine& operator=(Engine const&){};
         void update_freqs();
    public: 
	     void process_adsr(void* outputBuffer);

         static Engine* getInstance();
         void load_sawtooth(int v_idx, int f);
         void load_square_wave(int v_idx, int f);
         void tick(void* outputBuffer);
         void simple_tick(void* outputBuffer);
         void update_voice_gain(int v_idx, float gain);
         void update_harmonics(int v_idx, int harmonic, float gain);
         float get_freq(int v_idx, int harmonic);
         float get_gain(int v_idx, int harmonic);
         void gate_off();
         void gate_on();
         void load_sinewave(int v_idx, int f);
};