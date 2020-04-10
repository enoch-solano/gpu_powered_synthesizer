#pragma once


#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <stdexcept>




namespace Additive {
	void initSynth(int numSinusoids, int numSamples, float* host_frequencies);
	void endSynth();
    void alloc_engine(float2* &h_freq_gains, float* &h_angles, float* &h_v_gains, 
    float* &h_tmp_buffer, float* &h_buffer, float* &h_adsr,int num_samples, int num_voices, int num_harms);
    void compute_sinusoid_gpu_simple(float * buffer, float angle);
    void realloc_engine(float* &h_tmp_buffer,float* &h_buffer, int prev_num_samples, int num_samples);
    void my_v_compute(float *buffer, float angle, float* h_buffer, float* h_v_gains, float2* h_freq_gains, float* h_adsr, int numSamples, int numSinusoids, int numVoices);
    void compute_sinusoid_hybrid(float* samples, float2* h_freq_gains, float* h_angles, float *h_v_gains, float* h_tmp_buffer, float* h_buffer, int numSinusoids, float time, float numSamples);
}
