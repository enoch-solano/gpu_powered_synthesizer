#include "RtAudio.h"
#include "kernel.h"
#include <iostream>
#include <cstdlib>
#include "presets.h"
#include "voice_data.h"
#include "user_io_helper.h"
#include "constants.h"
#include "engine.h"
#include <stdio.h>




bool run = false;
Engine* synth = NULL;
int a = 0;
int num = 0;
/*
  callback function called by RtAudio that fills in outputBuffer
*/
int voice_callback(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	// need to offset each voice harmonic, and add to the buffer only if the button
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	// gets buffer data from GPU
	 if(run){
	 synth->simple_tick(outputBuffer);
	// 	for (int i =0 ; i < 256; i++){
	// 	float* buff = (float*)outputBuffer;
	// 	std::cout << num++ <<" " << buff[i] << std::endl;
	// }a++;
	 }


	// if (a ==5){
	// 	exit(0);
	// }


	return 0;
}

/*
  updates voice gains in the GPU
*/
int modify_voice_gain(int v_idx, Engine*engine) {
	printf(" | | Enter the value to update the amplitude of the voice: ");
	float gain = get_int();

	engine->update_voice_gain(v_idx, gain);

	return 1;
}


/*
  updates harmonic gains and freqs in the GPU
*/
int modify_harmonic(int v_idx,Engine*engine) {
	int h_idx = -1;
	printf(" | | Select a harmonic (enter a number between 1-%d): ", NUM_HARMS);
	while (h_idx < 0) {
		h_idx = get_int();

		if (h_idx == 0 || h_idx > NUM_HARMS) {
			printf(" | | Invalid harmonic selected. ");
			printf("Select a harmonic (enter a number between 1-%d): ", NUM_HARMS);
			h_idx = -1;
		}
	}

	h_idx--;	// makes sure it's zero indexed

	printf(" | | | Enter the value to update the amplitude of the harmonic: ");
	float gain = get_float();
    engine->update_harmonics(v_idx, h_idx, gain);

	return 1;
}



enum input_mode {
	EXIT,
	QUIT,
	INVALID,
	HARMONIC_MOD,		// modify harmonic 
	VOICE_MOD,			// modify voice gain
	SEL_PRESET			// select preset
};

int voice_modification_mode(Engine*engine) {
	input_mode curr_mode = INVALID;
	int voice_idx = -1;

	while (voice_idx < 0) {
		printf("\nSelect a voice (enter a number between 1-%d): ", NUM_VOICES);
		voice_idx = get_int();
   
		if (voice_idx == -1) {
			// quiting
			return -1;
		}
		else if (voice_idx == -2) {
			return -2;
		}
		else if (voice_idx == 0 || voice_idx > NUM_VOICES) {
			printf("Invalid voice selected. ");
			voice_idx = -1;
		}
		else {
			voice_idx--;	// makes sure it zero-indexes

			input_mode mode = INVALID;
			while (mode == INVALID) {
				printf(" | How would like to modify the voice? \n");
				printf(" |   > L -- to load a voice preset \n");
				printf(" |   > G -- to modify the amplitude of the voice \n");
				printf(" |   > H -- to modify a harmonic \n");


				char in_c = get_char();

				mode = in_c == 'q' ? QUIT :
					in_c == 'e' ? EXIT :
					in_c == 'L' || in_c == 'l' ? SEL_PRESET :
					in_c == 'G' || in_c == 'g' ? VOICE_MOD :
					in_c == 'H' || in_c == 'h' ? HARMONIC_MOD : INVALID;

				switch (mode) {
				case QUIT:
					return -1;
				case EXIT:
					return -2;

				case SEL_PRESET:
					select_preset(voice_idx, engine);
					print_harmonics(voice_idx, engine);
					run = true;
					break;
				case VOICE_MOD:
					modify_voice_gain(voice_idx, engine);
					break;
				case HARMONIC_MOD:
					modify_harmonic(voice_idx, engine);
					print_harmonics(voice_idx, engine);
					break;
				}
			}
		}
	}

	return 1;
}

int main() {
     RtAudio dac;
     if ( dac.getDeviceCount() < 1 ) {
         std::cout << "\nNo audio devices found!\n";
         exit( 0 );
     }

     RtAudio::StreamParameters parameters;
     parameters.deviceId = 0;//dac.getDefaultOutputDevice();
     parameters.nChannels = 1;

     unsigned int sampleRate = SAMPLING_FREQUENCY;
     unsigned int bufferFrames = NUM_SAMPLES; // 256 sample frames

    for (unsigned int i = 0; i < dac.getDeviceCount(); i++){
			RtAudio::DeviceInfo info = dac.getDeviceInfo(i);
			if(info.probed == true) std::cout<<"device"<<i<<" = " << info.name <<std::endl;
		}


     synth = Engine::getInstance();

     // print_harmonics(0, user_data);

     try {
	      dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32, sampleRate,
	    	&bufferFrames, &voice_callback);
	      dac.startStream();
     }
     catch (RtAudioError& e) {
	      e.printMessage();
	      exit(0);
     }

	//------ USER I/O ------//

	/*
		button to cycle through options (in voice harmonics)
		> voice gain modification	-- detected by which knob you're turning
		> load a preset			-- select voice by turning knob (wait until it stops?)
			> select preset through a button cycle
		> modify harmonics

		button to cycle through options (in harmonics modification)
		> harm gain modification
		> harm freq modification
		> double click to exit
	*/


     int RUNNING = 1;
     int USER_MODE = 0;

     while (RUNNING) {
	     switch (USER_MODE) {
	     case 0:
		     RUNNING = -1 != voice_modification_mode(synth);
		     USER_MODE = 1;
		     break;
	     case 1:
		     
		     USER_MODE = 0;
		     break;
	     } 
     }

     try {
         // Stop the stream
         dac.stopStream();
     } catch (RtAudioError& e) {
         e.printMessage();
     }

     if (dac.isStreamOpen()) {
	     dac.closeStream();
     }




    return 0;
}
