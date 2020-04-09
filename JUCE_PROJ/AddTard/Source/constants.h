#pragma once

#define NUM_VOICES_INIT 4/* number of voices */
#define NUM_HARMS_INIT 64/* number of harmonics in a voice */

#define NUM_SINES 100
#define _PI 3.1415926535897931
#define NUM_SAMPLES 256
#define SAMPLING_FREQUENCY 44100

#define NUM_USER_PARAM 16

#define E_KEY 0x45
#define Q_KEY 0x51

#define ON_G 1
#define OFF_G 0

//#define WINDOWS_MACHINE
#define JETSON_NANO

/* time variable used by sine waves */
static float angle_m = 0;

typedef struct float2 {
    float x;
    float y;
}float2;