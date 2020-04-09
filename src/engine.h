#pragma once
#include "ADSR.h"
#include "lfo.h"
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
        float *samples;
        float *fundamental_freqs;
        float time;

        ADSR *adsr;

        LFO *gain_lfo;          // lfo to apply on gain
        LFO *freq_filter_lfo;   // lfo to filter freq
       
        int *freq_ratios;
        int num_samples;
        int num_sinusoids;
        int num_harms;
        int num_voices; 
        float angle = 0;

        int voice_mute[NUM_VOICES];
        float global_volume;
        int global_mute;

        Engine();  
        ~Engine();
        Engine(Engine const&){};
        Engine& operator=(Engine const&){};
        void update_freqs();

    public:
        // returns the singleton instance
        static Engine* getInstance();

        // output generator
        void tick(void* outputBuffer);
        void simple_tick(void* outputBuffer);

        // getters and setters for Engine
        void update_voice_gain(int v_idx, float gain);
        void update_harmonics(int v_idx, int harmonic, float gain);
        float get_freq(int v_idx, int harmonic);
        float get_gain(int v_idx, int harmonic);

        void update_global_gain(float gain);
        void toggle_global_mute();
        void toggle_voice_mute(int v_idx);

        // preset loading functionality
        void load_sinewave(int v_idx, int f);
        void load_square_wave(int v_idx, int f);
        void load_sawtooth(int v_idx, int f);

        // ADSR functionality
        void gate_on();
        void gate_off();
        void process_adsr(void *outputBuffer);
        
        void setAttackRate(float rate);
        void setDecayRate(float rate);
        void setReleaseRate(float rate);
        void setSustainLevel(float level);

        // LFO functionality
        void process_gain_lfo(void *outputBuffer, float angle);
        void set_gain_lfo_rate(float rate);
        void set_gain_lfo_level(float level);
};