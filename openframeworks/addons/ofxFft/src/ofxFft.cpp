#include "ofxFft.h"

#include "ofxFftBasic.h"
#include "ofxFftw.h"

ofxFft* ofxFft::create(int signalSize, fftWindowType windowType, fftImplementation implementation) {
	ofxFft* fft;
	if(implementation == OF_FFT_BASIC) {
		fft = new ofxFftBasic();
	} else if(implementation == OF_FFT_FFTW) {
		fft = new ofxFftw();
	}
	fft->setup(signalSize, windowType);
	return fft;
}

void ofxFft::setup(int signalSize, fftWindowType windowType) {
	this->signalSize = signalSize;
	this->bins = signalSize / 2;

	signalNormalized = true;
	signal = new float[signalSize];

	cartesianUpdated = true;
	cartesianNormalized = true;
	real = new float[bins];
	imag = new float[bins];

	polarUpdated = true;
	polarNormalized = true;
	amplitude = new float[bins];
	phase = new float[bins];

	clear();

	window = new float[signalSize];
	inverseWindow = new float[signalSize];
	setWindowType(windowType);
}

int ofxFft::getBinSize() {
	return bins;
}

int ofxFft::getSignalSize() {
	return signalSize;
}

void ofxFft::setWindowType(fftWindowType windowType) {
	this->windowType = windowType;
	if(windowType == OF_FFT_RECTANGULAR) {
		for(int i = 0; i < signalSize; i++)
			window[i] = 1; // only used for windowSum
	} else if(windowType == OF_FFT_BARTLETT) {
		for (int i = 0; i < signalSize / 2; i++) {
			window[i] = (i / (signalSize / 2));
			window[i + (signalSize / 2)] = (1.0 - (i / (signalSize / 2)));
		}
	} else if(windowType == OF_FFT_HANN) {
		for(int i = 0; i < signalSize; i++)
			window[i] = .5 * (1 - cos((TWO_PI * i) / (signalSize - 1)));
	} else if(windowType == OF_FFT_HAMMING) {
		for(int i = 0; i < signalSize; i++)
			window[i] = .54 - .46 * cos((TWO_PI * i) / (signalSize - 1));
	} else if(windowType == OF_FFT_SINE) {
		for(int i = 0; i < signalSize; i++)
			window[i] = sin((PI * i) / (signalSize - 1));
	}

	windowSum = 0;
	for(int i = 0; i < signalSize; i++)
		windowSum += window[i];

	for(int i = 0; i < signalSize; i++)
		inverseWindow[i] = 1. / window[i];
}

ofxFft::~ofxFft() {
	delete [] signal;
	delete [] real;
	delete [] imag;
	delete [] amplitude;
	delete [] phase;
	delete [] window;
	delete [] inverseWindow;
}

void ofxFft::draw(float x, float y) {
	draw(x, y, getWidth(), getHeight());
}

void ofxFft::draw(float x, float y, float width, float height) {
	ofPushStyle();
	ofPushMatrix();

	ofTranslate(x, y);
	ofNoFill();
	ofRect(0, 0, width, height);
	ofTranslate(0, height);
	ofScale(width / bins, -height);
	ofBeginShape();
	getAmplitude();
	for (int i = 0; i < bins; i++)
		ofVertex(i, amplitude[i]);
	ofEndShape();

	ofPopMatrix();
	ofPopStyle();
}

float ofxFft::getWidth() {
	return bins;
}

float ofxFft::getHeight() {
	return bins / 2;
}

void ofxFft::clear() {
	memset(signal, 0, sizeof(float) * signalSize);
	memset(real, 0, sizeof(float) * bins);
	memset(imag, 0, sizeof(float) * bins);
	memset(amplitude, 0, sizeof(float) * bins);
	memset(phase, 0, sizeof(float) * bins);
}

void ofxFft::copySignal(float* signal) {
	memcpy(this->signal, signal, sizeof(float) * signalSize);
}

void ofxFft::copyReal(float* real) {
	memcpy(this->real, real, sizeof(float) * bins);
}

void ofxFft::copyImaginary(float* imag) {
	if(imag == NULL)
		memset(this->imag, 0, sizeof(float) * bins);
	else
		memcpy(this->imag, imag, sizeof(float) * bins);
}

void ofxFft::copyAmplitude(float* amplitude) {
	memcpy(this->amplitude, amplitude, sizeof(float) * bins);
}

void ofxFft::copyPhase(float* phase) {
	if(phase == NULL)
		memset(this->phase, 0, sizeof(float) * bins);
	else
		memcpy(this->phase, phase, sizeof(float) * bins);
}

void ofxFft::prepareSignal() {
	if(!signalUpdated)
		updateSignal();
	if(!signalNormalized)
		normalizeSignal();
}

void ofxFft::updateSignal() {
	prepareCartesian();
	executeIfft();
	signalUpdated = true;
	signalNormalized = false;
}

void ofxFft::normalizeSignal() {
	float normalizer = (float) windowSum / (2 * signalSize);
	for (int i = 0; i < signalSize; i++)
		signal[i] *= normalizer;
	signalNormalized = true;
}

float* ofxFft::getSignal() {
	prepareSignal();
	return signal;
}

void ofxFft::clampSignal() {
	prepareSignal();
	for(int i = 0; i < signalSize; i++) {
		if(signal[i] > 1)
			signal[i] = 1;
		else if(signal[i] < -1)
			signal[i] = -1;
	}
}

void ofxFft::prepareCartesian() {
	if(!cartesianUpdated) {
		if(!polarUpdated)
			executeFft();
		else
			updateCartesian();
	}
	if(!cartesianNormalized)
		normalizeCartesian();
}

float* ofxFft::getReal() {
	prepareCartesian();
	return real;
}

float* ofxFft::getImaginary() {
	prepareCartesian();
	return imag;
}

void ofxFft::preparePolar() {
	if(!polarUpdated)
		updatePolar();
	if(!polarNormalized)
		normalizePolar();
}

float* ofxFft::getAmplitude() {
	preparePolar();
	return amplitude;
}

float* ofxFft::getPhase() {
	preparePolar();
	return phase;
}

void ofxFft::updateCartesian() {
	for(int i = 0; i < bins; i++) {
		real[i] = cosf(phase[i]) * amplitude[i];
		imag[i] = sinf(phase[i]) * amplitude[i];
	}
	cartesianUpdated = true;
	cartesianNormalized = polarNormalized;
}

void ofxFft::normalizeCartesian() {
	float normalizer = 2. / windowSum;
	for(int i = 0; i < bins; i++) {
		real[i] *= normalizer;
		imag[i] *= normalizer;
	}
	cartesianNormalized = true;
}

void ofxFft::updatePolar() {
	prepareCartesian();
	for(int i = 0; i < bins; i++) {
		amplitude[i] = cartesianToAmplitude(real[i], imag[i]);
		phase[i] = cartesianToPhase(real[i], imag[i]);
	}
	polarUpdated = true;
	polarNormalized = cartesianNormalized;
}

void ofxFft::normalizePolar() {
	float normalizer = 2. / windowSum;
	for(int i = 0; i < bins; i++)
		amplitude[i] *= normalizer;
	polarNormalized = true;
}

void ofxFft::clearUpdates() {
	cartesianUpdated = false;
	polarUpdated = false;
	cartesianNormalized = false;
	polarNormalized = false;
	signalUpdated = false;
	signalNormalized = false;
}

void ofxFft::setSignal(float* signal) {
	clearUpdates();
	copySignal(signal);
	signalUpdated = true;
	signalNormalized = true;
}

void ofxFft::setCartesian(float* real, float* imag) {
	clearUpdates();
	copyReal(real);
	copyImaginary(imag);
	cartesianUpdated = true;
	cartesianNormalized = true;
}

void ofxFft::setPolar(float* amplitude, float* phase) {
	clearUpdates();
	copyAmplitude(amplitude);
	copyPhase(phase);
	polarUpdated = true;
	polarNormalized = true;
}
