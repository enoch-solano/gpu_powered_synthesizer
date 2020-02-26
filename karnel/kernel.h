#pragma once

#include <cuda.h>
#include <cuda_runtime.h>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <stdexcept>

#include "../src/voice_data.h"


namespace Additive {
	void initSynth(int numSinusoids, int numSamples, float* host_frequencies);
	void endSynth();
    void compute_sinusoid_gpu_simple(float * buffer, float angle);
    void my_v_compute(float *buffer, float angle, float* h_buffer, float* h_v_gains, float2* h_freq_gains, int numSamples, int numSinusoids, int numVoices);
    void compute_sinusoid_hybrid(float* samples, float2* h_freq_gains, float* h_angles, float *h_v_gains, float* h_tmp_buffer, float* h_buffer, int numSinusoids, float time, float numSamples);
}
