#pragma once

#include "ofxFft.h"
#include "fftw3.h"

class ofxFftw : public ofxFft{
public:
	ofxFftw();
	void setup(int signalSize, fftWindowType windowType);
	~ofxFftw();
private:
	float *fftIn, *fftOut, *ifftIn, *ifftOut;
	fftwf_plan fftPlan, ifftPlan;
protected:
	void executeFft(float* input);
	void executeIfft(float* input);
	void executeIfft(float* real, float* imag);
};
