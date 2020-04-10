using namespace std;
#include "engine.h"


Engine* Engine::engine = NULL;    


Engine::Engine(int num_samples) { 
          
          // adsr = new ADSR();
          
          // // initialize adsr settingsinclude cud
          // adsr->setAttackRate(.1 * SAMPLING_FREQUENCY);	// .1 seconds
          // adsr->setDecayRate(.3 * SAMPLING_FREQUENCY);	// .3 seconds
          // adsr->setReleaseRate(5 * SAMPLING_FREQUENCY);	// 5 seconds
          // adsr->setSustainLevel(.8);
          num_voices= NUM_VOICES_INIT;
          num_harms = NUM_HARMS_INIT;
          num_sinusoids= num_harms * num_voices;
          this->num_samples = num_samples;
         
          Additive::alloc_engine(h_freq_gains, h_angles,
           h_v_gains, h_tmp_buffer, h_buffer, freq_ratios, num_samples, num_voices, num_harms);
          for(int i = 0; i < num_voices; i++){
               h_v_gains[i]  = 1.0;
          }
          load_square_wave(0, 440);
          load_sawtooth(1,440);
          load_sinewave(2,440);
          
}      

void Engine::realloc_engine(int num_samples){
     
     Additive::realloc_engine(h_buffer, h_tmp_buffer,this->num_samples, num_samples);
}
Engine* Engine::getInstance(int num_samples){
     if(!Engine::engine) Engine::engine = new Engine(num_samples);
     else if (num_samples != Engine::engine->num_samples) {
         engine->realloc_engine(num_samples);
     }
     return Engine::engine;
}

void Engine::toggleMute(int v_idx, float voicelvl){
     if(h_v_gains[v_idx-1] != 0.0){
          h_v_gains[v_idx-1] = 0.0;
     } else {
          h_v_gains[v_idx-1] = voicelvl;
     }
}

Engine* Engine::getInstance(){
     if(!Engine::engine) Engine::engine = new Engine(128);
     return Engine::engine;
}

void Engine::process_adsr(void* outputBuffer){
   // adsr->batch_process(NUM_SAMPLES, (float*)outputBuffer);
}

void Engine::load_sawtooth(int v_idx, int f) {

     float L = 1;
     update_fundamental(v_idx, f);
     for (int i = 0; i < num_harms; i++) {
          h_freq_gains[v_idx*num_harms + i].y = (-1.f / (_PI * (i + 1)));
     }
}
void Engine::load_square_wave(int v_idx, int f) {
     update_fundamental(v_idx, f);
     for (int i = 0; i < num_harms; i+=2) {
          h_freq_gains[v_idx*num_harms + i].y = 1.f / (1.f + (i)); //gain
          
     }
}


void Engine::load_sinewave(int v_idx, int f) {
          h_freq_gains[v_idx*num_harms].y = 1.0; //gain
          h_freq_gains[v_idx*num_harms].x = f;   //freq
     
}
void Engine::update_freqs(){
     for (int i = 0; i < num_voices; i++){
        for (int j  = 0; j < num_harms; j++){
             h_freq_gains[i*num_harms + j].x = freq_ratios[i*num_harms +j]*fundamental_freqs[i];
        }
     }
             
}
void Engine::update_voice_gain(int v_idx, float gain){
     h_v_gains[v_idx] = gain;
}

void Engine::update_fundamental(int v_idx, float freq){
     h_freq_gains[v_idx * num_harms].y = 1.0f;
     for(int i = 0; i < num_harms; i++){
          h_freq_gains[v_idx * num_harms + i].x = freq*(i+1);
     }
}

void Engine::update_harmonics(int v_idx, int harmonic, float gain){
     h_freq_gains[v_idx*num_harms + harmonic].y = gain;
}

float Engine::get_freq(int v_idx, int harmonic){
     return h_freq_gains[v_idx*num_harms + harmonic].x;
}

float Engine::get_gain(int v_idx, int harmonic){
     return h_freq_gains[v_idx*num_harms + harmonic].y;
}

        
void Engine::gate_on(){
    // adsr->gate(ON_G);
}        

void Engine::gate_off(){
     //adsr->gate(OFF_G);
}
void Engine::tick(void* outputBuffer){
     Additive::compute_sinusoid_hybrid((float*)outputBuffer, h_freq_gains, h_angles, h_v_gains, h_tmp_buffer, h_buffer,num_sinusoids, time, this->num_samples);
     time += NUM_SAMPLES / 44100.f;
     
}

void Engine::simple_tick(void *outputBuffer, int num_Samples){
    // std::cout<< "tick" <<std::endl;
     Additive::my_v_compute((float*)outputBuffer, angle, 
     h_buffer,h_v_gains, h_freq_gains, this->num_samples, num_sinusoids, num_voices);
 //std::cout << "angle ion engine b4 add" << angle <<std::endl;
     // float a = 2.0f * 3.14f;
     // float b = a * this->num_samples;
     // float c = b * 1260.f;
     // float d = c / 44100.f;
     //std::cout << this->num_samples << std::endl;
     // std::cout << a<< std::endl;
     // std::cout << b<< std::endl;
     // std::cout << c<< std::endl;
     // std::cout << d<< std::endl;
     angle += MathConstants<float>::twoPi * this->num_samples  / 44100.f;
     //std::cout << "angle ion engine" << angle <<std::endl;
     //angle = fmod(angle,MathConstants<float>::twoPi);
}

