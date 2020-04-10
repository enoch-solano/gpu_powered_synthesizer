#include "SynthADSR.h"

SynthADSR::SynthADSR(void) {
	reset();
	setAttackRate(0);
	setDecayRate(0);
	setReleaseRate(0);
	setSustainLevel(1.0);
	setTargetRatioA(0.3);
	setTargetRatioDR(0.0001);
}

SynthADSR::~SynthADSR(void) {
}

void SynthADSR::setAttackRate(float rate) {
	attackRate = rate;
	attackCoef = calcCoef(rate, targetRatioA);
	attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void SynthADSR::setDecayRate(float rate) {
	decayRate = rate;
	decayCoef = calcCoef(rate, targetRatioDR);
	decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void SynthADSR::setReleaseRate(float rate) {
	releaseRate = rate;
	releaseCoef = calcCoef(rate, targetRatioDR);
	releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

float SynthADSR::calcCoef(float rate, float targetRatio) {
	return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

void SynthADSR::setSustainLevel(float level) {
	sustainLevel = level;
	decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void SynthADSR::setTargetRatioA(float targetRatio) {
	if (targetRatio < 0.000000001)
		targetRatio = 0.000000001;  // -180 dB
	targetRatioA = targetRatio;
	attackCoef = calcCoef(attackRate, targetRatioA);
	attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void SynthADSR::setTargetRatioDR(float targetRatio) {
	if (targetRatio < 0.000000001)
		targetRatio = 0.000000001;  // -180 dB
	targetRatioDR = targetRatio;
	decayCoef = calcCoef(decayRate, targetRatioDR);
	releaseCoef = calcCoef(releaseRate, targetRatioDR);
	decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
	releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

float SynthADSR::get_atk()
{
	return attackRate;
}

float SynthADSR::get_dec()
{
	return decayRate;
}

float SynthADSR::get_stn()
{
	return sustainLevel;
}

float SynthADSR::get_rel()
{
	return releaseRate;
}

