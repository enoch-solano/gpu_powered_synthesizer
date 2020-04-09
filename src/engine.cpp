using namespace std;
#include "engine.h"

Engine* Engine::engine = NULL;    

/**
* Check for CUDA errors; print and exit if there was a problem.
*/
void checkCUDAError_(const char *msg, int line = -1) {
     cudaError_t err = cudaGetLastError();
     
     if (cudaSuccess != err) {
          if (line >= 0) {
               fprintf(stderr, "Line %d: ", line);
          }

          fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString(err));
          exit(EXIT_FAILURE);
     }
}

Engine::Engine() {
     adsr = new ADSR();
          
     // initialize adsr settings
     adsr->setAttackRate(.1 * SAMPLING_FREQUENCY);	// .1 seconds
     adsr->setDecayRate(.3 * SAMPLING_FREQUENCY);	     // .3 seconds
     adsr->setReleaseRate(.7 * SAMPLING_FREQUENCY);	// .7 seconds
     adsr->setSustainLevel(.8);

     gain_lfo = new LFO();
     gain_lfo->set_rate(10.f);
     gain_lfo->set_level(1.f);

     freq_filter_lfo = new LFO();

     num_voices = NUM_VOICES;
     num_harms = NUM_HARMS;
     num_sinusoids = NUM_HARMS * NUM_VOICES;
     num_samples = NUM_SAMPLES;
     
     cudaHostAlloc((void**)&h_freq_gains, sizeof(float2)*NUM_HARMS * NUM_VOICES, cudaHostAllocMapped);
     checkCUDAError_("h_freq_gains get Device Pointer", __LINE__);
     
     cudaHostAlloc((void**)&h_angles, sizeof(float2)*NUM_HARMS * NUM_VOICES, cudaHostAllocMapped);
     checkCUDAError_("h_angles get Device Pointer", __LINE__);
     
     cudaHostAlloc((void**)&h_v_gains, sizeof(float)*NUM_VOICES, cudaHostAllocMapped);
     checkCUDAError_("h_v_gains get Device Pointer", __LINE__);
     
     cudaHostAlloc((void**)&h_tmp_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
     checkCUDAError_("h_tmp_buffer get Device Pointer", __LINE__);
     
     cudaHostAlloc((void**)&h_buffer, sizeof(float)*num_samples, cudaHostAllocMapped);
     checkCUDAError_("h_buffer get Device Pointer", __LINE__);
     
     cudaHostAlloc((void**)&freq_ratios, sizeof(int)*NUM_HARMS*NUM_VOICES,cudaHostAllocMapped);
     checkCUDAError_("freq_ratios get Device Pointer", __LINE__);
     
     for(int i = 0; i < num_voices; i++){
          h_v_gains[i]  = 1.0;
          voice_mute[i] = 1;
     }

     global_mute = 1;
}

Engine* Engine::getInstance(){
     if (!engine) engine = new Engine();
     return engine;
}


// ------------ preset loading ------------ //

void Engine::load_sawtooth(int v_idx, int f) {
     float L = 1;

     for (int i = 0; i < NUM_HARMS; i++) {
          h_freq_gains[v_idx*NUM_HARMS + i].y = (-1.f / (_PI * (i + 1)));
          h_freq_gains[v_idx*NUM_HARMS + i].x = (i * _PI / L) * f;
     }
}

void Engine::load_square_wave(int v_idx, int f) {
     for (int i = 0; i < NUM_HARMS; i++) {
          h_freq_gains[v_idx*NUM_HARMS + i].y = 1.f / (1.f + (2 * i)); // gain
          h_freq_gains[v_idx*NUM_HARMS + i].x = (1.f + (2 * i)) * f;   // freq
          freq_ratios[v_idx*NUM_HARMS +i] = (1 + (2 * i));
     }
}

void Engine::load_sinewave(int v_idx, int f) {
          h_freq_gains[v_idx*NUM_HARMS].y = 1; // gain
          h_freq_gains[v_idx*NUM_HARMS].x = f;   // freq
     
}


// ------------ adsr functions ------------ //

void Engine::process_adsr(void* outputBuffer) {
    adsr->batch_process(NUM_SAMPLES, (float*)outputBuffer);
}

void Engine::gate_on()  { adsr->gate(ON_G); }
void Engine::gate_off() { adsr->gate(OFF_G); }

void Engine::setAttackRate(float rate)    { adsr->setAttackRate(rate * SAMPLING_FREQUENCY); }
void Engine::setDecayRate(float rate)     { adsr->setDecayRate(rate * SAMPLING_FREQUENCY); }
void Engine::setReleaseRate(float rate)   { adsr->setReleaseRate(rate * SAMPLING_FREQUENCY); }
void Engine::setSustainLevel(float level) { adsr->setSustainLevel(level); }


// ------------ lfo functions ------------- //

void Engine::process_gain_lfo(void *outputBuffer, float angle) {
    gain_lfo->batch_gain_process(NUM_SAMPLES, (float*)outputBuffer, angle);
}


// ----------- getters/setters ----------- //

void Engine::update_freqs() {
     for (int i = 0; i < NUM_VOICES; i++){
        for (int j  = 0; j < NUM_HARMS; j++){
             h_freq_gains[i*NUM_HARMS + j].x = freq_ratios[i*NUM_HARMS +j]*fundamental_freqs[i];
        }
     }
             
}

void Engine::update_voice_gain(int v_idx, float gain) {
     h_v_gains[v_idx] = gain;
     h_v_gains[v_idx] *= voice_mute[v_idx] * global_volume * global_mute;
}

void Engine::update_harmonics(int v_idx, int harmonic, float gain){
     h_freq_gains[v_idx*NUM_HARMS + harmonic].y = gain;
}

void Engine::update_global_gain(float gain) {
     global_volume = gain;
}

void Engine::toggle_global_mute() {
     global_mute = !global_mute;
}

void Engine::toggle_voice_mute(int v_idx) {
     voice_mute[v_idx] = !voice_mute[v_idx];
}

float Engine::get_freq(int v_idx, int harmonic){
     return h_freq_gains[v_idx*NUM_HARMS + harmonic].x;
}

float Engine::get_gain(int v_idx, int harmonic){
     return h_freq_gains[v_idx*NUM_HARMS + harmonic].y;
}


// ----------- tick functions ----------- //

void Engine::tick(void* outputBuffer){
     //Additive::update_freqs(freq_gains, freq_ratios, num_voices, num_harms, fundamental_freqs)
     Additive::compute_sinusoid_hybrid((float*)outputBuffer, h_freq_gains, h_angles, h_v_gains, h_tmp_buffer, h_buffer,num_sinusoids, time, num_samples);
     time += NUM_SAMPLES / 44100.f;
     
}

void Engine::simple_tick(void *outputBuffer)
{
     Additive::my_v_compute((float*)outputBuffer, angle, 
                             h_buffer, h_v_gains, h_freq_gains, 
                             num_samples, NUM_VOICES*NUM_HARMS, NUM_VOICES);
  
     process_adsr((float*) outputBuffer);
     process_gain_lfo((float*) outputBuffer, angle);

     angle += 2.0f * _PI * num_samples / 44100.f;
     angle = fmod(angle, 44100.f);
}

