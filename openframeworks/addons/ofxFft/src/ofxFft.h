#pragma once

#include "ofMain.h"

enum fftWindowType {
	OF_FFT_RECTANGULAR,
	OF_FFT_BARTLETT,
	OF_FFT_HANN,
	OF_FFT_HAMMING,
	OF_FFT_SINE};

enum fftImplementation {
	OF_FFT_BASIC,
	OF_FFT_FFTW};

enum fftMode {
	OF_FFT_CARTESIAN,
	OF_FFT_POLAR};

class ofxFft : public ofBaseDraws {
public:
	// create and destroy fft
	static ofxFft* create(
		int signalSize = 512,
		fftWindowType windowType = OF_FFT_HAMMING,
		fftImplementation implementation = OF_FFT_BASIC);
	virtual ~ofxFft();

	// forward fft
	float* fft(float* input, fftMode mode = OF_FFT_POLAR);
	int getBinSize();
	float* getReal();
	float* getImaginary();
	float* getAmplitude();
	float* getPhase();
	// float* getPowerSpectrum();
	// float* getPower();

	// inverse fft
	float* ifft(float* input);
	float* ifft(float* a, float* b, fftMode mode = OF_FFT_POLAR);
	int getSignalSize();
	float* getSignal();

	// ofBaseDraws
	void draw(float x, float y);
	void draw(float x, float y, float width, float height);
	float getWidth();
	float getHeight();

protected:
	virtual void setup(int bins, fftWindowType windowType);
	virtual void executeFft(float* input) = 0;
	virtual void executeIfft(float* input) = 0;
	virtual void executeIfft(float* real, float* imag) = 0;

	void clear();

	// time domain data and methods
	fftWindowType windowType;
	float windowSum;
	float *window;

	bool signalNormalized;
	float *signal;

	void setSignal(float* signal);
	void setWindowType(fftWindowType windowType);
	inline void runWindow(float* signal) {
		if(windowType != OF_FFT_RECTANGULAR)
			for(int i = 0; i < signalSize; i++)
				signal[i] *= window[i];
	}

	// frequency domain data and methods
	int signalSize, bins;

	float *real, *imag;
	bool cartesianUpdated, cartesianNormalized;
	void checkCartesian();
	void updateCartesian();
	void normalizeCartesian();
	void setReal(float* real);
	void setImaginary(float* imag);

	float *amplitude, *phase;
	bool polarUpdated, polarNormalized;
	void checkPolar();
	void updatePolar();
	void normalizePolar();
	void setAmplitude(float* amplitude);
	void setPhase(float* phase);

	void clearUpdates();

	inline float cartesianToAmplitude(float x, float y) {
		return sqrtf(x * x + y * y);
	}

	inline float cartesianToPhase(float x, float y) {
		return atan2f(y, x);
	}
};
