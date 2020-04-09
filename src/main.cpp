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

#ifdef USING_QT_GUI
#include "qt_user_io.hpp"
#endif

bool run = true;
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
	synth->simple_tick(outputBuffer);

	return 0;
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
		if (info.probed == true) std::cout<<"device"<<i<<" = " << info.name <<std::endl;
	}

	synth = Engine::getInstance();

	try {
		dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32, sampleRate,
		&bufferFrames, &voice_callback);
		dac.startStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
		exit(0);
	}


	QtUI ui(synth);
	ui.run();

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
