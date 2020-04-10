#pragma once

#include "constants.h"
#include "kernel/kernel.h"
#include <iostream>
#include <JuceHeader.h>
#include "SynthADSR.h"
#include "lfo.h"

class Engine {
    private:
        static Engine *engine;

        float2 *h_freq_gains;    // contains freqs for every harmonic of every voice
        float *h_angles;
        float *h_v_gains;      // gain for overall voice
        float *h_buffer;
        float *h_tmp_buffer;
        float *h_adsr;
        float *samples;
        float *fundamental_freqs;
        float time;
        bool v_on[4];
        SynthADSR *adsr[4];
        LFO *gain_lfo;
        int enable_gain_lfo;

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
        // returns the singleton instance
        static Engine* getInstance();
        static Engine* getInstance(int num_samples);

        // output generator
        void tick(void* outputBuffer);
        void simple_tick(void* outputBuffer, int num_Samples);

        void load_sinewave(int v_idx, int f);
        void load_sawtooth(int v_idx, int f);
        void load_square_wave(int v_idx, int f);

        void update_fundamental(int v_idx, float freq);
        void update_voice_gain(int v_idx, float gain);
        void update_harmonics(int v_idx, int harmonic, float gain);
        void toggleMute(int v_idx, float voicelvl);
        float get_freq(int v_idx, int harmonic);
        float get_gain(int v_idx, int harmonic);
        bool get_mute(int v_idx);


        // ADSR functionality
        void gate_on();
        void gate_off();
        void process_adsr(void *outputBuffer);
        void get_adsr(int v_idx, float* curr_adsr);
        void set_adsr(int v_idx, float* curr_adsr);

        
        // LFO functionality
        void process_gain_lfo(void *outputBuffer, float angle);
        void set_gain_lfo_rate(float rate);
        void set_gain_lfo_level(float level);
        void set_gain_lfo_type(int type);
        
        void toggle_gain_lfo();
};
