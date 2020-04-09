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
          num_voices = NUM_VOICES;
          num_harms = NUM_HARMS;
          num_sinusoids = NUM_HARMS * NUM_VOICES;
          this->num_samples = num_samples;
         
          Additive::alloc_engine(h_freq_gains, h_angles,
           h_v_gains, h_tmp_buffer, h_buffer, freq_ratios, num_samples);
          for(int i = 0; i < num_voices; i++){
               h_v_gains[i]  = 1.0;
          }
          load_sinewave(0, 440);
}      

void Engine::realloc_engine(int num_samples){
     
     Additive::realloc_engine(h_buffer, h_tmp_buffer,this->num_samples, num_samples);
}
Engine* Engine::getInstance(int num_samples){
     if(!engine) engine = new Engine(num_samples);
     else if (num_samples != engine->num_samples) {
         engine->realloc_engine(num_samples);
     }
     return engine;
}

void Engine::process_adsr(void* outputBuffer){
   // adsr->batch_process(NUM_SAMPLES, (float*)outputBuffer);
}

void Engine::load_sawtooth(int v_idx, int f) {

     float L = 1;

     for (int i = 0; i < NUM_HARMS; i++) {
          h_freq_gains[v_idx*NUM_HARMS + i].y = (-1.f / (_PI * (i + 1)));
          h_freq_gains[v_idx*NUM_HARMS + i].x = (i * _PI / L) * f;
     }
}
void Engine::load_square_wave(int v_idx, int f) {
    

     for (int i = 0; i < NUM_HARMS; i++) {
          h_freq_gains[v_idx*NUM_HARMS + i].y = 1.f / (1.f + (2 * i)); //gain
          h_freq_gains[v_idx*NUM_HARMS + i].x = (1.f + (2 * i)) * f;   //freq
          freq_ratios[v_idx*NUM_HARMS +i] = (1 + (2 * i));
     }
}

void Engine::load_sinewave(int v_idx, int f) {
    

     
          h_freq_gains[v_idx*NUM_HARMS].y = 1.0; //gain
          h_freq_gains[v_idx*NUM_HARMS].x = f;   //freq
     
}
void Engine::update_freqs(){
     for (int i = 0; i < NUM_VOICES; i++){
        for (int j  = 0; j < NUM_HARMS; j++){
             h_freq_gains[i*NUM_HARMS + j].x = freq_ratios[i*NUM_HARMS +j]*fundamental_freqs[i];
        }
     }
             
}
void Engine::update_voice_gain(int v_idx, float gain){
     h_v_gains[v_idx] = gain;
}

void Engine::update_harmonics(int v_idx, int harmonic, float gain){
     h_freq_gains[v_idx*NUM_HARMS + harmonic].y = gain;
}

float Engine::get_freq(int v_idx, int harmonic){
     return h_freq_gains[v_idx*NUM_HARMS + harmonic].x;
}

float Engine::get_gain(int v_idx, int harmonic){
     return h_freq_gains[v_idx*NUM_HARMS + harmonic].y;
}
        
void Engine::gate_on(){
    // adsr->gate(ON_G);
}        

void Engine::gate_off(){
     //adsr->gate(OFF_G);
}
void Engine::tick(void* outputBuffer){
     //Additive::update_freqs(freq_gains, freq_ratios, num_voices, num_harms, fundamental_freqs)
     Additive::compute_sinusoid_hybrid((float*)outputBuffer, h_freq_gains, h_angles, h_v_gains, h_tmp_buffer, h_buffer,num_sinusoids, time, this->num_samples);
     time += NUM_SAMPLES / 44100.f;
     
}

void Engine::simple_tick(void *outputBuffer, int num_Samples){
     std::cout<< "tick" <<std::endl;
     Additive::my_v_compute((float*)outputBuffer, angle, 
     h_buffer,h_v_gains, h_freq_gains, this->num_samples, NUM_VOICES * NUM_HARMS, NUM_VOICES);
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

