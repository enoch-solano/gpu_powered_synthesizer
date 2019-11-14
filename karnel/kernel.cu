#include "kernel.h"
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <cuda.h>
#include "kernel.h"
#define M_PI 3.1415926535897931
//divide
#define THREADS_PER_SAMPLE 16
#define SAMPLES_PER_THREAD 1
#define SAMPLING_FREQ 44100
//#define SIMPLE 0
#define checkCUDAErrorWithLine(msg) checkCUDAError(msg, __LINE__)
float* dev_freqs, *dev_gains, *dev_vgains, *dev_buffer, *dev_tmp_buffer, *dev_target, *dev_angle;
float slideTime;
int numSamples, numSinusoids, numVoices;

/**
* Check for CUDA errors; print and exit if there was a problem.
*/
void checkCUDAError(const char *msg, int line = -1) {
  cudaError_t err = cudaGetLastError();
  if (cudaSuccess != err) {
    if (line >= 0) {
      fprintf(stderr, "Line %d: ", line);
    }
    fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString(err));
    exit(EXIT_FAILURE);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//************************************* my synth with voice/harmonics *************************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void my_vh_kernel(float *outBuffer, float *freqs, float *gains, float *vgains,
								float angle, int numSamples, int numSinusoids, int numVoices)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples) {
		// samples sine wave in discrete steps
		angle = angle + 2.f * M_PI * idx / 44100.f;
		float buff_val = 0.f;
		int numHarmonics = numSinusoids / numVoices;

		for (int i = 0; i < numVoices; i++) {
			for (int j = 0; j < numHarmonics; j++) {
				buff_val += vgains[i] * gains[i*numHarmonics + j] * __sinf(angle * freqs[i*numHarmonics + j]);
			}
		}

		// buffer to be sent to DAC
		outBuffer[idx] = buff_val;
	}
}

void Additive::my_v_compute(float *buffer, float angle) {
	int threadsPerBlock = 256;
	int blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;

	my_vh_kernel <<< blocksPerGrid, threadsPerBlock >>> (dev_buffer, dev_freqs, dev_gains, dev_vgains,
															angle, numSamples, numSinusoids, numVoices);

	// updates the buffer with dev_buffer computed in GPU
	cudaMemcpy(buffer, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
}

void Additive::initVSynth(int numSample, const v_udata& v_user_data) {
	// initializes global variables with appropriate values
	numSamples = numSample;
	numSinusoids = NUM_VOICES * NUM_HARMS;
	numVoices = NUM_VOICES;

	// allocates memory in GPU
	cudaMalloc((void**)&dev_freqs, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_freqs malloc failed");
	cudaMalloc((void**)&dev_gains, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_gains malloc failed");
	cudaMalloc((void**)&dev_vgains, numVoices * sizeof(float));
	checkCUDAErrorWithLine("dev_vgains malloc failed");
	cudaMalloc((void**)&dev_buffer, numSamples * sizeof(float));
	checkCUDAErrorWithLine("dev_buffer malloc failed");

	// copy memory from CPU to GPU
	cudaMemcpy(dev_freqs, v_user_data.freqs, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_freqs memcpy failed");
	cudaMemcpy(dev_gains, v_user_data.gains, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_gains memcpy failed");
	cudaMemcpy(dev_vgains, v_user_data.v_gains, numVoices * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_vgains memcpy failed");

	cudaDeviceSynchronize();
}
void Additive::updateFreqsVSynth(float *freqs) {
	// copy memory from CPU to GPU
	cudaMemcpy(dev_freqs, freqs, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("frequencies memcpy failed");
}
void Additive::updateGainsVSynth(float *gains) {
	// copy memory from CPU to GPU
	cudaMemcpy(dev_gains, gains, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("gains memcpy failed");
}
void Additive::updateVGainsVSynth(float *v_gains) {
	// copy memory from CPU to GPU
	cudaMemcpy(dev_vgains, v_gains, numVoices * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("voice gains memcpy failed");
}
void Additive::endVSynth() {
	cudaFree(dev_freqs);
	cudaFree(dev_gains);
	cudaFree(dev_vgains);
	cudaFree(dev_buffer);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//************************************* my simple (interactive) synth *************************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void my_simple_kernel(float *outBuffer, float *freqs, float *gains, 
								 float angle, int numSamples, int numSinusoids)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples) {
		// samples sine wave in discrete steps
		angle = angle + 2.f * M_PI * idx / 44100.f;
		float buff_val = 0.f;

		for (int i = 0; i < 16 /*NUM_SINUSOIDS*/; i++) {
			buff_val += gains[i] * __sinf(angle * freqs[i]);
		}

		// buffer to be sent to DAC
		outBuffer[idx] = buff_val;
	}
}

void Additive::my_simple_compute(float *buffer, float angle)
{
	int threadsPerBlock = 256;
	int blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;

	my_simple_kernel <<< blocksPerGrid, threadsPerBlock >>> (dev_buffer, dev_freqs, dev_gains,
															 angle, numSamples, numSinusoids);

	// updates the buffer with buffer computed in GPU
	cudaMemcpy(buffer, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
}

void Additive::initSimpleSynth(int numSinusoid, int numSample, float *init_freqs, float *init_gains)
{
	// initializes global variables with appropriate values
	numSamples = numSample;
	numSinusoids = numSinusoid;

	// allocates memory in GPU
	cudaMalloc((void**)&dev_freqs, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_freqs malloc failed");
	cudaMalloc((void**)&dev_gains, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_gains malloc failed");
	cudaMalloc((void**)&dev_buffer, numSamples * sizeof(float));
	checkCUDAErrorWithLine("dev_buffer malloc failed");

	// copy memory from CPU to GPU
	cudaMemcpy(dev_freqs, init_freqs, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_freqs memcpy failed");
	cudaMemcpy(dev_gains, init_gains, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_gains memcpy failed");

	cudaDeviceSynchronize();
}

void Additive::updateFreqsSimpleSynth(float *freqs) {
	// copy memory from CPU to GPU
	cudaMemcpy(dev_freqs, freqs, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("initial frequencies memcpy failed");
}

void Additive::updateGainsSimpleSynth(float *gains) {
	// copy memory from CPU to GPU
	cudaMemcpy(dev_gains, gains, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_gains memcpy failed");
}


void Additive::endSimpleSynth() {
	cudaFree(dev_freqs);
	cudaFree(dev_buffer);
	cudaFree(dev_gains);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Additive::initSynth(int numSinusoid, int numSample, float* host_frequencies) {
	
	numSamples = numSample;
	numSinusoids = numSinusoid;
	cudaMalloc((void**)&dev_freqs, numSinusoids * sizeof(float));
	cudaMalloc((void**)&dev_buffer, numSamples * sizeof(float));
	cudaMemcpy(dev_freqs, host_frequencies, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	
	cudaDeviceSynchronize();
}

void Additive::initSynth_THX(int numSinusoid, int numSample, float* host_start_freq, float* host_end_freq, float* host_angle, float*  host_gains, float slide) {
	numSamples = numSample;
	numSinusoids = numSinusoid;
	slideTime = slide;

	cudaMalloc((void**)&dev_freqs, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_freqs malloc failed");
	cudaMemcpy(dev_freqs, host_start_freq, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_freqs memcpy failed");
	cudaMalloc((void**)&dev_buffer, numSamples * sizeof(float));
	checkCUDAErrorWithLine("dev_buffer malloc failed");
	cudaMalloc((void**)&dev_tmp_buffer, numSamples *THREADS_PER_SAMPLE* sizeof(float));
	checkCUDAErrorWithLine("dev_tmp_buffer malloc failed");
	cudaMalloc((void**)&dev_gains, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_gains malloc failed");
	cudaMemcpy(dev_gains, host_gains, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("dev_gains memcpy failed");
	cudaMalloc((void**)&dev_angle, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_angle malloc failed");
	//cudaMemcpy(dev_angle, host_angle, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	cudaMalloc((void**)&dev_target, numSinusoids * sizeof(float));
	checkCUDAErrorWithLine("dev_target malloc failed");
	cudaMemcpy(dev_target, host_end_freq, numSinusoids * sizeof(float), cudaMemcpyHostToDevice);
	checkCUDAErrorWithLine("target frequencies copy failed");

	cudaDeviceSynchronize();
}

void Additive::endSynth_THX() {
	cudaFree(dev_freqs);
	cudaFree(dev_buffer);
	cudaFree(dev_tmp_buffer);
	cudaFree(dev_gains);
	cudaFree(dev_angle);
	cudaFree(dev_target);
}
void Additive::endSynth() {
	cudaFree(dev_buffer);
	cudaFree(dev_freqs);
}

__global__ void sin_kernel_simple(float *outBuffer, float *frequencies, float angle, int numSamples, int numSinusoids) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples) {
		angle = angle + 2.0f * M_PI * idx / 44100.f;
		float val = 0.0f;
		for (int i = 0; i < numSinusoids; i++) {
			val +=  0.1 * __sinf((angle * frequencies[i]));
		}

		outBuffer[idx] = val;
	}
}

void Additive::compute_sinusoid_gpu_simple(float* buffer, float angle) {
	int threadsPerBlock = 256; 
	int blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;

	sin_kernel_simple <<< blocksPerGrid, threadsPerBlock >>> (dev_buffer, dev_freqs, angle, numSamples, numSinusoids);
//	sin_kernel_simple <<< 1, 256 >>> (dev_buffer, dev_freqs, angle, numSamples, numSinusoids);
	
	cudaMemcpy(buffer, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
}

__device__ float ramp_kern(float currentTime, float slideTime, float f0, float f1){
	float integral;
	if (currentTime < slideTime) {
		float k = (f1-f0) / slideTime;
		integral = currentTime * (f0 + k * currentTime / 2.0f);
	} else {
		integral = f0 * slideTime + (f1 - f0) * slideTime / 2.0f;
		integral += (currentTime - slideTime) * f1;
	}
	return integral * 2.0f * M_PI;
}

#define imin( a, b ) ( ((a) < (b)) ? (a) : (b) )

__global__ void sin_kernel_fast(float* buffer, float* frequencies, float* targetFrequencies, 
								float* angles, float* gains, int numThreadsPerBlock, int numSinusoids,
								float time, float slideTime, int numSamples) 
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples * THREADS_PER_SAMPLE) {
		//determine how many sineWaves are to be computed in each thread based on how many threads it takes to compute a sample
		int maxSinePerBlock = (numSinusoids + THREADS_PER_SAMPLE - 1) / THREADS_PER_SAMPLE;
		int sinBlock = idx / numThreadsPerBlock;
		int sampleIdx = idx - sinBlock * numThreadsPerBlock; // modulo function but GPUs are trash at modulo so don't use it
		float val[SAMPLES_PER_THREAD];
		for (int j = 0; j < SAMPLES_PER_THREAD; j++) {
			val[j] = 0.0f;
		}
	    float gain, freq0, freq1, angle, angleStart;
	    int firstSine = sinBlock * maxSinePerBlock;
		int lastSine = imin(numSinusoids, firstSine + maxSinePerBlock);
		//compute samples for maxSinePerBlock
		for (int i = firstSine; i < lastSine; i++) {
			angleStart = 0; 
			freq0 = frequencies[i];
			freq1 = targetFrequencies[i];
			gain = gains[i];
			for (int j = 0; j < SAMPLES_PER_THREAD; j++) {
				angle = ramp_kern(time + (sampleIdx * SAMPLES_PER_THREAD + j) / SAMPLING_FREQ, slideTime, freq0, freq1);
				val[j] += __sinf(angleStart + angle) * gain / numSinusoids;
			}
		}
		for (int i = 0; i < SAMPLES_PER_THREAD; i++) {
			buffer[idx * SAMPLES_PER_THREAD + i] = val[i];
		}

	}

}


__global__ void sum_blocks(float* tmp_buffer, float* buffer, int numSamples) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples) {
		float sum = 0;
		for (int i = 0; i < THREADS_PER_SAMPLE; i++) {
			sum += tmp_buffer[idx + i * numSamples];
		}
		buffer[idx] = sum;
	}

}

void Additive::compute_sinusoid_hybrid(float* buffer, float * time){
	int threadsPerBlock = 256; 
	int numThreadsPerBlock = numSamples / SAMPLES_PER_THREAD;
	int numThreads = THREADS_PER_SAMPLE * numThreadsPerBlock;
	int blocksPerGrid = (numThreads + threadsPerBlock - 1) / threadsPerBlock;

	
	sin_kernel_fast <<<blocksPerGrid, threadsPerBlock >>>(dev_tmp_buffer, dev_freqs, dev_target, dev_angle, dev_gains, numThreadsPerBlock, numSinusoids, *time, slideTime, numSamples);
	//checkCUDAErrorWithLine("sin_kernel_fast failed");
	blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;
	sum_blocks <<<blocksPerGrid, threadsPerBlock >> >(dev_tmp_buffer, dev_buffer, numSamples);
	//checkCUDAErrorWithLine("sum_blocks failed");
	cudaMemcpy(buffer, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
}
