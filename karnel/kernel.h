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

	void initSynth_THX(int numSinusoid, int numSample, float* host_start_freq, float* host_end_freq,
					   float* host_angle, float*  host_gains, float slide);
	void endSynth_THX();

	void compute_sinusoid_hybrid(float* buffer, float* time);

	void initSimpleSynth(int numSinusoid, int numSample, float *init_freqs, float *init_gains);
	void updateFreqsSimpleSynth(float *freqs);
	void updateGainsSimpleSynth(float *gains);
	void endSimpleSynth();
	void my_simple_compute(float *buffer, float angle);


	void initVSynth(int numSample, const voice_data& v_user_data);
	void updateFreqsVSynth(float *freqs);
	void updateGainsVSynth(float *gains);
	void updateVGainsVSynth(float *v_gains);
	void endVSynth();
	void my_v_compute(float *buffer, float angle);
}
