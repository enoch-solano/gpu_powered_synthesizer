#include "RtAudio.h"
#include "kernel.h"
#include <iostream>
#include <cstdlib>
#include "Sine.h"

//Sine *global_sine[100];
#define NUM_SINES 100
#define _PI 3.1415926535897931
#define NUM_SAMPLES 16
#define SAMPLING_FREQUENCY 44100

#define NUM_USER_PARAM 16

static float angle_m = 0;

//#define SIMPLE 1
//#define NOT_SIMPLE 1

struct udata {
	float freq[NUM_USER_PARAM];
	float gain[NUM_USER_PARAM];
} user_data;

void print_user_data(const udata &user_data) {
	printf("freq: ");
	for (int i = 0; i < NUM_USER_PARAM - 1; i++) {
		printf("%.2f, ", user_data.freq[i]);
	}

	printf("%.2f\n", user_data.freq[NUM_USER_PARAM - 1]);


	printf("gains: ");
	for (int i = 0; i < NUM_USER_PARAM - 1; i++) {
		printf("%.2f, ", user_data.gain[i]);
	}

	printf("%.2f\n", user_data.gain[NUM_USER_PARAM - 1]);
}

void print_harmonic(int v_idx, const v_udata &data) {
	printf("\nThe harmonics of voice (%d):\n", v_idx+1);

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

//////////////////////////////////////////////////////////////////////////////////////

int simple_callback(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
					double streamTime, RtAudioStreamStatus status, void *userData)
{
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	Additive::my_simple_compute((float*)outputBuffer, angle_m);

	angle_m += 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle_m = fmod(angle_m, 44100.f);

	return 0;
}

int voice_callback(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	// gets buffer data from GPU
	Additive::my_v_compute((float*)outputBuffer, angle_m);

	// increments angle (i.e. time) 
	angle_m += 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle_m = fmod(angle_m, 44100.f);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

int user_quit(char *input) {
	return strcmp(input, "quit") == 0 || strcmp(input, "Quit") == 0;
}

int get_int() {
	char user_in[256];
	scanf("%s", user_in);
	fflush(stdin);

	if (user_quit(user_in)) {
		return -1;
	}

	return atoi(user_in);
}

char get_char() {
	char user_in[256];
	scanf("%s", user_in);
	fflush(stdin);

	if (user_quit(user_in)) {
		return 'q';
	}

	return user_in[0];
}

void load_square_wave(int v_idx, v_udata& v_user_data) {
	float f = 220;

	for (int i = 0; i < NUM_HARMS; i++) {
		v_user_data.gains[v_idx*NUM_HARMS + i] = 1.f / (1.f + (2*i));
		v_user_data.freqs[v_idx*NUM_HARMS + i] = (1.f + (2*i)) * f;
	}

	Additive::updateFreqsVSynth(v_user_data.freqs);
	Additive::updateGainsVSynth(v_user_data.gains);
}

void load_sawtooth(int v_idx, v_udata& v_user_data) {
	float f = 220;
	float L = 1;

	for (int i = 0; i < NUM_HARMS; i++) {
		v_user_data.gains[v_idx*NUM_HARMS + i] = (-1.f / (_PI * (i+1)));
		v_user_data.freqs[v_idx*NUM_HARMS + i] = (i * _PI / L) * f;
	}

	Additive::updateFreqsVSynth(v_user_data.freqs);
	Additive::updateGainsVSynth(v_user_data.gains);
}

int select_preset(int v_idx, v_udata& v_user_data) {
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


int modify_voice_gain(int v_idx, v_udata& v_user_data) {
	printf(" | | Enter the value to update the amplitude of the voice: ");
	v_user_data.v_gains[v_idx] = get_int();

	Additive::updateVGainsVSynth(v_user_data.v_gains);

	return 1;
}


int modify_harmonic(int v_idx, v_udata& v_user_data) {
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
 
// #define SINGLE_VOICE 1
#ifdef SINGLE_VOICE

  // fills in user data with some freq and gains
  for (int i = 1; i <= 16; i++) {
	  if (i == 2) {
		  user_data.freq[i - 1] = 440.f;
	  } else {
		  user_data.freq[i - 1] = 0.f;
	  }
	  
	  user_data.gain[i - 1] = 1.f;
  }


  Additive::initSimpleSynth(NUM_USER_PARAM, NUM_SAMPLES, user_data.freq, user_data.gain);

  try {
	  dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32, sampleRate,
					 &bufferFrames, &simple_callback, (void*)&user_data);
	  dac.startStream();
  } catch (RtAudioError& e) {
	  e.printMessage();
	  exit(0);
  }

  int RUNNING = 1;
  enum input_mode { INVALID_MODE, FREQ_MODE, GAIN_MODE };

  while (RUNNING) {
	  input_mode curr_mode = INVALID_MODE;
	  int channel_num = -1;
	  int updated_val = -1;

	  // prompt user for input
	  while (curr_mode == INVALID_MODE) {
		  char user_in[256];
		  

		  printf("Enter desired mode : \n", user_in);
		  scanf("%s", user_in);
		  fflush(stdin);


		  if (user_quit(user_in))
		  {
			  RUNNING = 0;
			  break;
		  }
		  
		  if (strlen(user_in) > 1 || strlen(user_in) == 0) {
			  printf("invalid mode selected\n");
			  continue;
		  }

		  switch (user_in[0]) {
		  case 'f':
			  printf("freq_mode selected\n");
			  curr_mode = FREQ_MODE;
			  break;
		  case 'g':
			  printf("gain_mode selected\n");
			  curr_mode = GAIN_MODE;
			  break;
		  default:
			  printf("invalid mode selected\n");
			  break;
		  }
	  }

	  if (!RUNNING) {
		  printf("exiting program!\n");
		  break;
	  }

	  while (channel_num < 0) {
		  char user_in[256];

		  printf("Enter channel between 1 to 16 : \n", user_in);
		  scanf("%s", user_in);
		  fflush(stdin);

		  if (user_quit(user_in))
		  {
			  RUNNING = 0;
			  break;
		  }

		  channel_num = atoi(user_in);

		  if (channel_num < 1 || channel_num > NUM_USER_PARAM) {
			  printf("invalid channel\n");
			  channel_num = -1;
		  } else {
			  printf("selected channel %d\n", channel_num);
			  channel_num--;
		  }
	  }

	  if (!RUNNING) {
		  printf("exiting program!\n");
		  break;
	  }

	  while (updated_val < 0) {
		  char user_in[256];

		  printf("Enter the new value to update with: \n", user_in);
		  scanf("%s", user_in);
		  fflush(stdin);

		  if (user_quit(user_in))
		  {
			  RUNNING = 0;
			  break;
		  }

		  updated_val = atoi(user_in);
		  printf("selected value %d\n", updated_val);
	  }

	  if (!RUNNING) {
		  printf("exiting program!\n");
		  break;
	  }

	  // execute user request
	  switch (curr_mode) 
	  {
	  case FREQ_MODE:
		  user_data.freq[channel_num] = updated_val;
		  Additive::updateFreqsSimpleSynth(user_data.freq);
		  break;
	  case GAIN_MODE:
		  user_data.gain[channel_num] = updated_val;
		  Additive::updateGainsSimpleSynth(user_data.gain);
		  break;
	  }

	  print_user_data(user_data);
  }
  
#else

v_udata v_user_data;

for (int i = 0; i < NUM_VOICES; i++) {
	v_user_data.v_gains[i] = 1.f;

	for (int j = 0; j < NUM_HARMS; j++) {
		v_user_data.freqs[i*NUM_HARMS + j] = 0.f;
		v_user_data.gains[i*NUM_HARMS + j] = 1.f;
	}
}

Additive::initVSynth(NUM_SAMPLES, v_user_data);

print_harmonic(0, v_user_data);

try {
	dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32, sampleRate,
		&bufferFrames, &voice_callback, (void*)&v_user_data);
	dac.startStream();
}
catch (RtAudioError& e) {
	e.printMessage();
	exit(0);
}

//------ USER I/O ------//


int RUNNING = 1;
enum input_mode { QUIT,
				  INVALID_MODE,
				  H_MODE,			// modify harmonic 
				  V_MODE,			// modify voice gain
				  S_PRESET_MODE,	// select preset
				};

while (RUNNING) {
	input_mode curr_mode = INVALID_MODE;
	int voice_idx = -1;

	while (voice_idx < 0) {
		printf("Select a voice (enter a number between 1-%d): ", NUM_VOICES);
		voice_idx = get_int();

		if (voice_idx < 0) {
			// quiting
			RUNNING = 0;
			break;
		} else if (voice_idx == 0 || voice_idx > NUM_VOICES) {
			printf("Invalid voice selected. ");
			voice_idx = -1;
		} else {
			voice_idx--;	// makes sure it zero-indexes

			input_mode mode = INVALID_MODE;
			while (mode == INVALID_MODE && mode != QUIT) {
				printf(" | How would like to modify the voice? \n");
				printf(" |   > L -- to load a voice preset \n");
				printf(" |   > G -- to modify the amplitude of the voice \n");
				printf(" |   > H -- to modify a harmonic \n");


				char in_c = get_char();

				mode =  in_c == 'q' ? QUIT :
						in_c == 'L' || in_c == 'l' ? S_PRESET_MODE :
						in_c == 'G' || in_c == 'g' ? V_MODE :
						in_c == 'H' || in_c == 'h' ? H_MODE : INVALID_MODE;

				if (mode != QUIT || mode != INVALID_MODE) {
					if (mode == S_PRESET_MODE) {
						select_preset(voice_idx, v_user_data);
						print_harmonic(voice_idx, v_user_data);
					}
					else if (mode == V_MODE) {
						modify_voice_gain(voice_idx, v_user_data);
					}
					else if (mode == H_MODE) {
						modify_harmonic(voice_idx, v_user_data);
						print_harmonic(voice_idx, v_user_data);
					}
				}
			}

			if (mode == QUIT) {
				RUNNING = 0;
				break;
			}
		}
	}
}

#endif 

  try {
    // Stop the stream
    dac.stopStream();
  } catch (RtAudioError& e) {
    e.printMessage();
  }

  if (dac.isStreamOpen()) {
	  dac.closeStream();
  }


#if SIMPLE
  Additive::endSimpleSynth();
#else
  Additive::endVSynth();
#endif

  return 0;
}
