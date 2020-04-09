#pragma once

#include <iostream>
#include <cstdlib>

#include "engine.h"

/*
  returns whether or not user input is quitting
*/
int user_quit(char *input) {
	return strcmp(input, "quit") == 0 ||
		strcmp(input, "Quit") == 0 ||
		strcmp(input, "Q") == 0 ||
		strcmp(input, "q") == 0;
}

int user_exit(char *input) {
	return strcmp(input, "E") == 0 ||
		strcmp(input, "e") == 0;
}

/*
  returns the integer typed by the user
*/
int get_int() {
	char user_in[256];

	scanf("%s", user_in);

	fflush(stdin);

	if (user_quit(user_in)) {
		return -1;
	}

	if (user_exit(user_in)) {
		return -2;
	}

	return atoi(user_in);
}

/*
  returns the integer typed by the user
*/
float get_float() {
	char user_in[256];
	scanf("%s", user_in);
	fflush(stdin);

	if (user_quit(user_in)) {
		return -1;
	}

	if (user_exit(user_in)) {
		return -2;
	}

	return atof(user_in);
}

/*
  returns the first charcter typed by a user
*/
char get_char() {
	char user_in[256];
	scanf("%s", user_in);
	fflush(stdin);

	if (user_quit(user_in)) {
		return 'q';
	}

	if (user_exit(user_in)) {
		return 'e';
	}

	return user_in[0];
}

/*
  prompts the user to select a preset and fulfills said command
*/
int select_preset(int v_idx, Engine *synth) {
	printf(" | | Select a preset: \n");
	printf(" | |   > 1 -- square wave \n");
	printf(" | |   > 2 -- sawtooth wave \n");
	printf(" | |   > 3 -- sine wave \n");


	int preset = 0;

	while (preset == 0) {
		preset = get_int();

		switch (preset) {
		case 1:
			synth->load_square_wave(v_idx, 200);
			break;
		case 2:
			synth->load_sawtooth(v_idx, 200);
			break;
		case 3: 
			synth->load_sinewave(v_idx, 440);
			break;
		default:
			preset = 0;
		}
	}

	return 1;
}


/*
  prints the harmonics of a voice
*/
void print_harmonics(int v_idx, Engine *synth) {
	printf("\nThe harmonics of voice (%d):\n", v_idx + 1);

	printf("freq: ");
	for (int i = 0; i < NUM_HARMS - 1; i++) {
		printf("%.1f,\t", synth->get_freq(v_idx, i));
	}

	printf("%.1f\n", synth->get_freq(v_idx, NUM_HARMS - 1));


	printf("amps: ");
	for (int i = 0; i < NUM_HARMS - 1; i++) {
		printf("%.1f,\t", synth->get_gain(v_idx, i));
	}

	printf("%.1f\n\n", synth->get_gain(v_idx, NUM_HARMS -1));
}

// ---- TERMINAL I/O FUNCTIONS ---- //

/*
  updates voice gains in the GPU
*/
int modify_voice_gain(int v_idx, Engine*engine) {
	printf(" | | Enter the value to update the amplitude of the voice: ");
	float gain = get_float();

	engine->update_voice_gain(v_idx, gain);

	return 1;
}

/*
  updates harmonic gains and freqs in the GPU
*/
int modify_harmonic(int v_idx, Engine*engine) {
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

int voice_modification_mode(Engine*engine, bool &run) {
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