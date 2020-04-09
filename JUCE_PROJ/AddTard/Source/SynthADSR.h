#pragma once

//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the SynthADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#include <math.h>

class SynthADSR {
public:
	SynthADSR(void);
	~SynthADSR(void);

	float process(void);
	
	float getOutput(void);
	int getState(void);
	void gate(int on);

	void batch_process(int num, float *ouput);
	void process_SynthADSR(int num, float *buffer);

	void setAttackRate(float rate);
	void setDecayRate(float rate);
	void setReleaseRate(float rate);
	void setSustainLevel(float level);
	void setTargetRatioA(float targetRatio);
	void setTargetRatioDR(float targetRatio);
	
	void reset(void);

	enum envState {
		env_idle = 0,
		env_attack,
		env_decay,
		env_sustain,
		env_release
	};

protected:
	int state;
	float output;

	float attackRate;
	float decayRate;
	float releaseRate;

	float attackCoef;
	float decayCoef;
	float releaseCoef;
	
	float sustainLevel;
	
	float targetRatioA;
	float targetRatioDR;
	
	float attackBase;
	float decayBase;
	float releaseBase;

	float calcCoef(float rate, float targetRatio);
};

inline float SynthADSR::process() {
	switch (state) {
	case env_idle:
		break;
	case env_attack:
		output = attackBase + output * attackCoef;
		if (output >= 1.0) {
			output = 1.0;
			state = env_decay;
		}
		break;
	case env_decay:
		output = decayBase + output * decayCoef;
		if (output <= sustainLevel) {
			output = sustainLevel;
			state = env_sustain;
		}
		break;
	case env_sustain:
		break;
	case env_release:
		output = releaseBase + output * releaseCoef;
		if (output <= 0.0) {
			output = 0.0;
			state = env_idle;
		}
	}
	return output;
}

inline void SynthADSR::gate(int gate) {
	if (gate)
		state = env_attack;
	else if (state != env_idle)
		state = env_release;
}

inline int SynthADSR::getState() {
	return state;
}

inline void SynthADSR::reset() {
	state = env_idle;
	output = 0.0;
}

inline float SynthADSR::getOutput() {
	return output;
}

inline void SynthADSR::batch_process(int num, float *output) {
	for (int i = 0; i < num; i++) {
		output[i] *= this->process();
	}
}

inline void SynthADSR::process_SynthADSR(int num, float *buffer) {
	for (int i = 0; i < num; i++) {
		buffer[i] = this->process();
	}
}

