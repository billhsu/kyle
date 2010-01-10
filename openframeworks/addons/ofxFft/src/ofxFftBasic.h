#pragma once

#include "ofxFft.h"
#include "ofTypes.h"

class ofxFftBasic : public ofxFft {
public:
	void setup(int signalSize, fftWindowType windowType);
	~ofxFftBasic();
private:
	void FFT(int bins, bool InverseTransform, float *RealIn, float *ImagIn, float *RealOut, float *ImagOut);
	void RealFFT(int bins, float *RealIn, float *RealOut, float *ImagOut);
	void FastPowerSpectrum(int bins, float *In, float *Out);
	void powerSpectrum(int start, int half, float *data, int bins, float *magnitude, float *phase, float *power, float *avg_power);
	void inversePowerSpectrum(int start, int half, int bins, float *finalOut, float *magnitude, float *phase);
protected:
	void executeFft(float* input);
	void executeIfft(float* input);
	void executeIfft(float* real, float* imag);
};
