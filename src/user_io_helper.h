#pragma once

#include <iostream>
#include <cstdlib>

#include "voice_data.h"

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
int select_preset(int v_idx, voice_data& v_user_data) {
	printf(" | | Select a preset: \n");
	printf(" | |   > 1 -- square wave \n");
	printf(" | |   > 2 -- sawtooth wave \n");

	int preset = 0;

	while (preset == 0) {
		preset = get_int();

		switch (preset) {
		case 1:
			load_square_wave(v_idx, v_user_data);
			break;
		case 2:
			load_sawtooth(v_idx, v_user_data);
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
void print_harmonics(int v_idx, const voice_data &data) {
	printf("\nThe harmonics of voice (%d):\n", v_idx + 1);

	printf("freq: ");
	for (int i = 0; i < NUM_HARMS - 1; i++) {
		printf("%.1f,\t", data.freqs[v_idx*NUM_HARMS + i]);
	}

	printf("%.1f\n", data.freqs[v_idx*NUM_HARMS + NUM_HARMS - 1]);


	printf("amps: ");
	for (int i = 0; i < NUM_HARMS - 1; i++) {
		printf("%.1f,\t", data.gains[v_idx*NUM_HARMS + i]);
	}

	printf("%.1f\n\n", data.gains[v_idx*NUM_HARMS + NUM_HARMS - 1]);
}
