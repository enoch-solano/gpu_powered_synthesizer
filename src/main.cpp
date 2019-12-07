#include "RtAudio.h"
#include "kernel.h"
#include <iostream>
#include <cstdlib>
#include "Sine.h"

#include "presets.h"
#include "voice_data.h"
#include "user_io_helper.h"
#include "constants.h"

#include <stdio.h>

#ifdef WINDOWS_MACHINE
#include <windows.h>
#endif

#include "ADSR.h"

/*
  callback function called by RtAudio that fills in outputBuffer
*/
int voice_callback(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	// need to offset each voice harmonic, and add to the buffer only if the button
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	// gets buffer data from GPU
	Additive::my_v_compute((float*)outputBuffer, angle_m);

	// increments angle (i.e. time) 
	angle_m += 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle_m = fmod(angle_m, 44100.f);

	return 0;
}

/*
  updates voice gains in the GPU
*/
int modify_voice_gain(int v_idx, voice_data& v_user_data) {
	printf(" | | Enter the value to update the amplitude of the voice: ");
	v_user_data.v_gains[v_idx] = get_int();

	Additive::updateVGainsVSynth(v_user_data.v_gains);

	return 1;
}


/*
  updates harmonic gains and freqs in the GPU
*/
int modify_harmonic(int v_idx, voice_data& v_user_data) {
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
	v_user_data.gains[v_idx*NUM_HARMS + h_idx] = get_int();

	printf(" | | | Enter the value to update the frequency of the harmonic: ");
	v_user_data.freqs[v_idx*NUM_HARMS + h_idx] = get_int();
	printf("\n");

	Additive::updateFreqsVSynth(v_user_data.freqs);
	Additive::updateGainsVSynth(v_user_data.gains);

	return 1;
}

#ifdef WINDOWS_MACHINE
int play_mode_func(/*voice_data& v_user_data*/) {
	DWORD        mode;          /* Preserved console mode */
	INPUT_RECORD event;         /* Input event */
	BOOL         done = FALSE;  /* Program termination flag */

	/* Get the console input handle */
	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);

	/* Preserve the original console mode */
	GetConsoleMode(hstdin, &mode);

	/* Set to no line-buffering, no echo, no special-key-processing */
	SetConsoleMode(hstdin, 0);

	/* Give the user instructions */
	printf(
		"\nCurrently in play mode.\n"
		"Press any key as many times as you like.\n"
		"Press e to exit and modify a voice.\n\n"
	);

	int button_was_pressed = 0;

	WORD key_pressed;

	while (!done)
	{
		if (WaitForSingleObject(hstdin, 0) == WAIT_OBJECT_0)  /* if kbhit */
		{
			DWORD count;  /* ignored */

			/* Get the input event */
			ReadConsoleInput(hstdin, &event, 1, &count);


			/* *first* key press event */
			if ((event.EventType == KEY_EVENT) && 
				event.Event.KeyEvent.bKeyDown && 
				!button_was_pressed) 
			{
				printf("Button pressed!\n");
				button_was_pressed = 1;

				key_pressed = event.Event.KeyEvent.wVirtualKeyCode;
				done = key_pressed == E_KEY;
			}

			/* key release event */
			if ((event.EventType == KEY_EVENT) && 
				!event.Event.KeyEvent.bKeyDown && 
				event.Event.KeyEvent.wVirtualKeyCode == key_pressed) 
			{
				printf("Button released!\n");
				button_was_pressed = 0;
			}
		}
	}

	/* All done! */
	SetConsoleMode(hstdin, mode);

	return key_pressed != E_KEY;
}
#endif


enum input_mode {
	EXIT,
	QUIT,
	INVALID,
	HARMONIC_MOD,		// modify harmonic 
	VOICE_MOD,			// modify voice gain
	SEL_PRESET			// select preset
};

int voice_modification_mode(voice_data& v_user_data) {
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

				switch (in_c) {
				case QUIT:
					return -1;
				case EXIT:
					return -2;

				case SEL_PRESET:
					select_preset(voice_idx, v_user_data);
					print_harmonics(voice_idx, v_user_data);
					break;
				case VOICE_MOD:
					modify_voice_gain(voice_idx, v_user_data);
					break;
				case HARMONIC_MOD:
					modify_harmonic(voice_idx, v_user_data);
					print_harmonics(voice_idx, v_user_data);
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
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 1;
  parameters.firstChannel = 0;

  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = NUM_SAMPLES; // 256 sample frames
 
voice_data user_data;

for (int i = 0; i < NUM_VOICES; i++) {
	user_data.v_gains[i] = 1.f;

	for (int j = 0; j < NUM_HARMS; j++) {
		user_data.freqs[i*NUM_HARMS + j] = 0.f;
		user_data.gains[i*NUM_HARMS + j] = 1.f;
	}
}

Additive::initVSynth(NUM_SAMPLES, user_data);

// print_harmonics(0, user_data);

try {
	dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32, sampleRate,
		&bufferFrames, &voice_callback, (void*)&user_data);
	dac.startStream();
}
catch (RtAudioError& e) {
	e.printMessage();
	exit(0);
}

play_mode_func();

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
		RUNNING = -1 != voice_modification_mode(user_data);
		USER_MODE = 1;
		break;
	case 1:
		play_mode_func();
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


  Additive::endVSynth();

  return 0;
}
