#include "kernel.h"
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <cuda.h>
#include <iostream>
//divide
#define THREADS_PER_SAMPLE 16
#define SAMPLES_PER_THREAD 1
#define SAMPLING_FREQ 44100
//#define SIMPLE 0
#define checkCUDAErrorWithLine(msg) checkCUDAError(msg, __LINE__)
float *dev_buffer, *dev_tmp_buffer;
float slideTime;
int numSamples, numSinusoids, numVoices;
void printArraywNewLines(int n, float *a, bool abridged) {
    printf("    [ ");
    for (int i = 0; i < n; i++) {
        if (abridged && i + 2 == 15 && n > 16) {
            i = n - 2;
            printf("... ");
        }
        printf("%3f\n", a[i]);
    }
    printf("]\n");
}
void printArraywNewLines(int n, float2 *a, bool abridged) {
    printf("    [ ");
    for (int i = 0; i < n; i++) {
        if (abridged && i + 2 == 15 && n > 16) {
            i = n - 2;
            printf("... ");
        }
        printf("%3f, ", a[i].x);
        printf("%3f\n", a[i].y);
    }
    printf("]\n");
}
void printArray(int n, float2 *a, bool abridged) {
    printf("    [ ");
    for (int i = 0; i < n; i++) {
        if (abridged && i + 2 == 15 && n > 16) {
            i = n - 2;
            printf("... ");
        }
        printf("{%3f, ", a[i].x);
        printf("%3f},", a[i].y);
    }
    printf("]\n");
}

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

void Additive::alloc_engine(float2* &h_freq_gains, float* &h_angles, float* &h_v_gains, float* &h_tmp_buffer,
 float* &h_buffer,float* &h_adsr, int num_samples, int num_voices, int num_harms){
	 cudaHostAlloc((void**)&h_freq_gains, sizeof(float2)*num_harms * num_voices, cudaHostAllocMapped);
          checkCUDAError("h_freq_gains get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_angles, sizeof(float)*num_harms*num_voices, cudaHostAllocMapped);
          checkCUDAError("h_angles get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_v_gains, sizeof(float)*num_voices, cudaHostAllocMapped);
          checkCUDAError("h_v_gains get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_tmp_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
          checkCUDAError("h_tmp_buffer get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
          checkCUDAError("h_buffer get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_adsr, sizeof(float)*num_voices * num_samples,cudaHostAllocMapped);
          checkCUDAError("h_Adsrs get Device Pointer", __LINE__);
	
}

void Additive::realloc_engine(float* &h_tmp_buffer,float* &h_buffer, int prev_num_samples, int num_samples){
          float tmp_buffer[num_samples];
		  float buffer[num_samples];
          cudaMemcpy(tmp_buffer, h_tmp_buffer, sizeof(float)*prev_num_samples, cudaMemcpyHostToHost);
		  checkCUDAError("memcpy realloc firs tmp_buffer", __LINE__);
		  cudaMemcpy(buffer, h_buffer, sizeof(float)*prev_num_samples, cudaMemcpyHostToHost); 
		  checkCUDAError("memcpy realloc first buffer", __LINE__);
		  cudaFree(h_tmp_buffer);
		  cudaFree(h_buffer);
          cudaHostAlloc((void**)&h_tmp_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
          checkCUDAError("h_tmp_buffer get Device Pointer", __LINE__);
          cudaHostAlloc((void**)&h_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
          checkCUDAError("h_buffer get Device Pointer", __LINE__);
          
		  cudaMemcpy(h_tmp_buffer,tmp_buffer, sizeof(float)*prev_num_samples, cudaMemcpyHostToHost);
		      checkCUDAError("memcpy realloc tmp_buffer", __LINE__);
		  cudaMemcpy(h_buffer,buffer,  sizeof(float)*prev_num_samples, cudaMemcpyHostToHost); 
		      checkCUDAError("memcpy reallox buffer", __LINE__);

}
__global__ void my_vh_kernel(float *outBuffer, float2 *freq_gains, float *vgains, float* adsr, float angle, int numSamples, int numSinusoids, int numVoices)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < numSamples) {
		// samples sine wave in discrete steps
		angle = angle + 2.f * M_PI * idx / 44100.f;
		
		float buff_val = 0.f;
		int numHarmonics = numSinusoids / numVoices;

		for (int i = 0; i < numVoices; i++) {
			for (int j = 0; j < numHarmonics; j++) {
				float gain = vgains[i] * adsr[i * idx] * freq_gains[i*numHarmonics + j].y;
				buff_val += gain * (__sinf(angle * freq_gains[i*numHarmonics + j].x) + 0.5f);
				//printf("idx %d buff val: %f\n", idx, buff_val);
			}
		}

		// buffer to be sent to DAC
		outBuffer[idx] = buff_val;
	}
}

void Additive::my_v_compute(float *buffer, float angle, 
	float* h_buffer, float* h_v_gains,
	 float2* h_freq_gains, float* h_adsr, int numSamples,
		int numSinusoids, int numVoices) 
	{
		//static int count = 0;
		//std::cout << "frequency" << std::endl;
		//printArray(1, h_freq_gains, 0);
		int threadsPerBlock = numSamples;
		int blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;
		float *dev_buffer, *dev_v_gains, *dev_adsr;
		float2* dev_freq_gains;
		cudaHostGetDevicePointer((void**)&dev_freq_gains, (void*)h_freq_gains, 0);
		checkCUDAError("dev_freq_gains get Device Pointer", __LINE__);

		//cudaHostGetDevicePointer((void**)&dev_angles, (void*)h_angles, 0);
		cudaHostGetDevicePointer((void**)&dev_v_gains, (void*)h_v_gains, 0);
		checkCUDAError("dev_v_gains get Device Pointer", __LINE__);
	
		cudaHostGetDevicePointer((void**)&dev_buffer, (void*)h_buffer, 0);
		checkCUDAError("dev_buffer get Device Pointer", __LINE__);

		cudaHostGetDevicePointer((void**)&dev_adsr, (void*)h_adsr, 0);
		checkCUDAError("dev_freq_gains get Device Pointer", __LINE__);

		
		my_vh_kernel <<< blocksPerGrid, threadsPerBlock >>> (dev_buffer, dev_freq_gains, dev_v_gains, dev_adsr,
										angle, numSamples, numSinusoids, numVoices);
		checkCUDAError("vhkernel error", __LINE__);
		cudaStreamSynchronize(NULL);
				//std::cout << "reyeet"<<std::endl;
		#ifdef KERNELDEBUG
           float *debug_arr1;
          cudaMallocManaged((void**)&debug_arr1, sizeof(float) * numSamples, cudaMemAttachHost);
          checkCUDAError("Error debugging output after cufftshift (malloc)", __LINE__);
          cudaMemcpy(debug_arr1,dev_buffer, sizeof(float) *numSamples,cudaMemcpyDeviceToHost);
          checkCUDAError("Error debugging output after cufftshift (memcpy)", __LINE__);
          printf("out\n");
          printArraywNewLines(numSamples, debug_arr1, 0);
          cudaFree(debug_arr1);
		#endif
    //std::cout << "yeet" << std::endl;
		// updates the buffer with dev_buffer computed in GPU
		cudaMemcpy(buffer, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
		checkCUDAError("memcpy error", __LINE__);
		#ifdef DEBUGCOPYM
		std::cout << "kernel" << std::endl;
		  float *debug_arr1;
          cudaMallocManaged((void**)&debug_arr1, sizeof(float) * numSamples, cudaMemAttachHost);
          checkCUDAError("Error debugging output after cufftshift (malloc)", __LINE__);
         memcpy(debug_arr1,buffer, sizeof(float) *numSamples);
          checkCUDAError("Error debugging output after cufftshift (memcpy)", __LINE__);
          printf("out\n");
          printArraywNewLines(numSamples, debug_arr1, 1);
          cudaFree(debug_arr1);
		#endif
		
		
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

__global__ void sin_kernel_fast(float2* freq_gains, float* buffer, 
								float* angles, int numThreadsPerBlock, int numSinusoids,
								float time, int numSamples) 
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
	    float gain, freq0, angle, angleStart;
	    int firstSine = sinBlock * maxSinePerBlock;
		int lastSine = imin(numSinusoids, firstSine + maxSinePerBlock);
		//compute samples for maxSinePerBlock
		for (int i = firstSine; i < lastSine; i++) {
			angleStart = angles[i]; 
			freq0 = freq_gains[i].x;
			gain = freq_gains[i].y;
			for (int j = 0; j < SAMPLES_PER_THREAD; j++) {
			  angle = angleStart + time + (sampleIdx*SAMPLES_PER_THREAD+j) / SAMPLING_FREQ;
				val[j] += __sinf(angle * freq0) * gain / numSinusoids;
			}
			angles[i] = fmod(angle, 44100.f);
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



void Additive::compute_sinusoid_hybrid(float* samples, float2* h_freq_gains, float* h_angles, float *h_v_gains, float* h_tmp_buffer, float* h_buffer, int numSinusoids, float time, float numSamples){
	int threadsPerBlock = 256; 
	int numThreadsPerBlock = numSamples / SAMPLES_PER_THREAD;
	int numThreads = THREADS_PER_SAMPLE * numThreadsPerBlock;
	int blocksPerGrid = (numThreads + threadsPerBlock - 1) / threadsPerBlock;
	float2* dev_freqs_gains;
	float* dev_buffer,* dev_tmp_buffer, *dev_angles, *dev_v_gains;
	cudaHostGetDevicePointer((void**)&dev_freqs_gains, (void*)h_freq_gains, 0);
	checkCUDAError("dev_freq_gains get Device Pointer", __LINE__);
	cudaHostGetDevicePointer((void**)&dev_angles, (void*)h_angles, 0);
	checkCUDAError("dev_angles get Device Pointer", __LINE__);
	cudaHostGetDevicePointer((void**)&dev_v_gains, (void*)h_v_gains, 0);
	checkCUDAError("dev_v_gains get Device Pointer", __LINE__);
	cudaHostGetDevicePointer((void**)&dev_tmp_buffer, (void*)h_tmp_buffer, 0);
	checkCUDAError("dev_tmp_buffer get Device Pointer", __LINE__);
	cudaHostGetDevicePointer((void**)&dev_buffer, (void*)h_buffer, 0);
	checkCUDAError("dev_buffer get Device Pointer", __LINE__);


	sin_kernel_fast <<<blocksPerGrid, threadsPerBlock >>>(dev_freqs_gains, dev_tmp_buffer, dev_angles, numThreadsPerBlock, numSinusoids, time, numSamples);
	//checkCUDAErrorWithLine("sin_kernel_fast failed");
	blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;
	sum_blocks <<<blocksPerGrid, threadsPerBlock >> >(dev_tmp_buffer, dev_buffer, numSamples);
	//checkCUDAErrorWithLine("sum_blocks failed");
	cudaMemcpy(samples, dev_buffer, numSamples * sizeof(float), cudaMemcpyDeviceToHost);
}

