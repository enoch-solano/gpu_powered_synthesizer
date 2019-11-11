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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//************************************* my simple (interactive) synth *************************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

int simple_callback(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
					double streamTime, RtAudioStreamStatus status, void *userData)
{
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	Additive::my_simple_compute((float*)outputBuffer, angle_m);

	angle_m += 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle_m = fmod(angle_m, 44100.f);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  unsigned int i, j;
  double *buffer = (double *) outputBuffer;
  double *lastValues = (double *) userData;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;
  // Write interleaved audio data.
  for ( i=0; i<nBufferFrames; i++ ) {
    for ( j=0; j<2; j++ ) {
      *buffer++ = lastValues[j];
      lastValues[j] += 0.005 * (j+1+(j*0.1));
      if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
    }
  }
  return 0;
}

int additive(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
			 double streamTime, RtAudioStreamStatus status, void *UserData) 
{
	float *buffer = (float *)outputBuffer;
	Sine** sine = (Sine **) UserData;
    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
	for (unsigned int i = 0; i < nBufferFrames; i++) {
		float val = 0; 
		for (unsigned int j = 0; j < NUM_SINES; j++) {
			val += sine[j]->tick();
		}
		*buffer++ = val;
	}
	return 0;
}

int additive_sine(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
				  double streamTime, RtAudioStreamStatus status, void *userData) 
{
  unsigned int i, j;
  float *buffer = (float *) outputBuffer;
  float *freqs = (float *) userData;
  
  if ( status ) std::cout << "Stream underflow detected!" << std::endl;
  
  // Write interleaved audio data.
  for (i = 0; i < nBufferFrames; i++) {
	  double val = 0;
	  float angle = 2.0f * _PI * i / 44100;
	  for (j = 0; j < NUM_SINES; j++) {
		  val += sin(angle * freqs[j]) / NUM_SINES;
	  }

	  *buffer++ = val;
  }
  return 0;

}

int additive_gpu(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
				 double streamTime, RtAudioStreamStatus status, void *userData)
{
    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
	
	Additive::compute_sinusoid_gpu_simple((float*)outputBuffer, angle_m);

	angle_m += 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle_m = fmod(angle_m, 44100.f);

	return 0;
}

int additive_gpu_complex(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames, 
						 double streamTime, RtAudioStreamStatus status, void *userData) 
{
	static float time = 0;

    if ( status ) std::cout << "Stream underflow detected!" << std::endl;

    Additive::compute_sinusoid_hybrid((float*)outputBuffer, &time);
	time += NUM_SAMPLES / 44100.f;
//	time = fmod(time, 44100.f);
	return 0;
}

#define END_SECS 15
#define MAXSFREQ 3000
#define MAXEFREQ 12000
#define FUNDFREQ 43.654
//developed by jmccarty https://ccrma.stanford.edu/~jmccarty/220/ass5/main.html
void fill_THX(float* freq_start, float* freq_end, float* angle, float *gains, int numSinusoids) {
	float randFreq;
	for (int i=0; i<NUM_SINES; i++){
        //selects random values in range 0->1
        randFreq=(float)(rand()%MAXSFREQ)/(float)MAXSFREQ; 
        //set a starting freqeuncy within range fundamental to max start freq.
        //in order to have more LF components the randnum is squared
		freq_start[i] = randFreq * randFreq*(float)(MAXSFREQ - FUNDFREQ) + FUNDFREQ;
        //generate rand num for end freq.
        randFreq=(float)(rand()%MAXEFREQ)/(float)MAXEFREQ;
        //set end frequencies to ocatve multiples of fundamental
        freq_end[i]=(float)pow(2,(int)(floor(randFreq*9)))*FUNDFREQ;
        //set half of the end freqs to a fifths above fundamental
        if((i % 2) == 0){
			freq_end[i]*=1.5;
		}
    }
        //for 5% of the oscillators chose a random end freq.
        //this adds a little inharmonic content
    for (int i = (int)NUM_SINES*0.05; i>=0; i--) {
        randFreq=(float)(rand()%MAXEFREQ)/(float)MAXEFREQ;
        freq_end[i]=(randFreq*200.0+30.0)*FUNDFREQ;
    }
   
    //generate oscillators, lower frequency components have higher amp.
    for (int i=0; i < NUM_SINES;i++) {   
		if (i <= (int)NUM_SINES*0.05) {
			gains[i] = 0.3;
			//gains[i] = 0.03;
		}
		else if (freq_end[i] < 2.0*FUNDFREQ) {
			gains[i] = 4.0f;
			//gains[i] = 0.4f;
		}	
		else if (freq_end[i] < 6.0*FUNDFREQ) {
			gains[i] = 2.0f;
			//gains[i] = 0.2f;
		} 	
		else {
			gains[i] = 1.0;
			//gains[i] = 0.1f;
		}

    }
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

  Sine* sine[NUM_SINES];
  for (int i = 1; i <= NUM_SINES; i++) {
	  Sine* newsine = new Sine();
	  newsine->setSamplingRate(sampleRate);
	  newsine->setFrequency(440 + i* 10);
	  sine[i-1] = newsine;
  }

  float freqs[NUM_SINES];
  for (int i = 1; i <= NUM_SINES; i++) {
	  freqs[i-1] =   440 + i*10;
  }
 
#if SIMPLE
  Additive::initSynth(NUM_SINES, bufferFrames, freqs);
  try {
    dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, 
		            &bufferFrames, &additive_gpu, (void*)&freqs );
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }

  char input;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get(input);

#elif NOT_SIMPLE
  float freqs_start[NUM_SINES];
  float freqs_end[NUM_SINES];
  float angles[NUM_SINES];
  float gains[NUM_SINES];
  float slideTime = END_SECS * 0.75; // SAMPLING_FREQUENCY * END_SECS * 0.75;

  fill_THX(freqs_start, freqs_end, angles, gains, NUM_SINES);

  Additive::initSynth_THX(NUM_SINES, bufferFrames, freqs_start, freqs_end, angles, gains, slideTime);

  try {
    dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, 
		            &bufferFrames, &additive_gpu_complex, (void*)&freqs );
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }

  char input;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get(input);
#elif SINGLE_VOICE
  v_udata v_user_data;

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

		  if (!strcmp(user_in, "quit") || !strcmp(user_in, "Quit"))
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

		  if (!strcmp(user_in, "quit") || !strcmp(user_in, "Quit"))
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

		  if (!strcmp(user_in, "quit") || !strcmp(user_in, "Quit"))
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

#endif // 

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
  Additive::endSynth();
#elif NOT_SIMPLE
  Additive::endSynth_THX();
#else
  Additive::endSimpleSynth();
#endif

  return 0;
}
