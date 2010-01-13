#pragma once

#include "ofxFft.h"
#include "ofTypes.h"

class ofxFftBasic : public ofxFft {
public:
	void setup(int signalSize, fftWindowType windowType);
	~ofxFftBasic();
protected:
	void executeFft();
	void executeIfft();
private:
	float* windowedSignal;
	void FFT(int bins, bool InverseTransform, float *RealIn, float *ImagIn, float *RealOut, float *ImagOut);
	void RealFFT(int bins, float *RealIn, float *RealOut, float *ImagOut);
};
