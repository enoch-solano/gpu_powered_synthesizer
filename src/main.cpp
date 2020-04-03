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

//---needed for sockets---//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string>
//------------------------//

#include "single_include/nlohmann/json.hpp"
using JSON = nlohmann::json;


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
	if (run){
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

#define BUFF_QTSOCK_OFFSET 4 /*offset for qt reading in*/
#define MAX_BUF_LEN 1024

// Function designed for chat between client and server. 
void connect_with_qt(int sockfd) 
{ 
    char buff[MAX_BUF_LEN]; 
    // infinite loop for chat 
    while (1) { 
		// send server message to client 
        char *msg_to_qt = "thanks!";
		write(sockfd, msg_to_qt, sizeof(msg_to_qt));

		bzero(buff, MAX_BUF_LEN); 
		int num_bytes_read = read(sockfd, buff, sizeof(buff));
		
		std::string msg_in;
		msg_in.append(buff+BUFF_QTSOCK_OFFSET, num_bytes_read-BUFF_QTSOCK_OFFSET);
		int msg_len = std::stoi(msg_in.substr(0, msg_in.find("_")));
		
		int bytes_remaining = msg_len - (msg_in.length() - msg_in.find("_") - 1);
		
		std::string msg = msg_in.substr(msg_in.find("_")+1);
		
		// read remaing buffer
		while (bytes_remaining >= 0) {
			bzero(buff, MAX_BUF_LEN);
			num_bytes_read = read(sockfd, buff, sizeof(buff));
			bytes_remaining -= num_bytes_read;
			msg.append(buff, num_bytes_read);
		}

		printf("message sent: %s, message received: %s\n", msg_to_qt, msg.c_str());
		JSON board_info = JSON::parse(msg);
		std::cout << board_info["adsr"].dump(4) << std::endl;

		synth->update_voice_gain(1, 0.05);
		synth->load_sinewave(1, 440);
    } 
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

	

	// set up code for socket
	char *socket_path = "/tmp/socket_for_synth_eng";

	struct sockaddr_un addr;
	int sockfd, connfd;

	// get file descriptor of socket
	if ( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
	unlink(socket_path);

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(-1);
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen error");
		exit(-1);
	}


	while (1) {
		if ( (connfd = accept(sockfd, NULL, NULL)) == -1) {
			perror("accept error");
			continue;
		} 

		// Function for chatting between client and server 
		connect_with_qt(connfd); 
	}
	
	// After chatting close the socket 
	close(sockfd);

    // while (RUNNING) {
	//      switch (USER_MODE) {
	//      case 0:
	// 	     RUNNING = -1 != voice_modification_mode(synth);
	// 	     USER_MODE = 1;
	// 	     break;
	//      case 1:
		     
	// 	     USER_MODE = 0;
	// 	     break;
	//      } 
    // }

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
