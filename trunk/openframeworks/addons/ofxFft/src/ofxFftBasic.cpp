#include "ofxFftBasic.h"

void ofxFftBasic::setup(int signalSize, fftWindowType windowType) {
	ofxFft::setup(signalSize, windowType);
	fftCfg = kiss_fftr_alloc(signalSize, 0, NULL, NULL);
	ifftCfg = kiss_fftr_alloc(signalSize, 1, NULL, NULL);
	windowedSignal = new float[signalSize];
}

ofxFftBasic::~ofxFftBasic() {
	kiss_fftr_free(fftCfg);
	kiss_fftr_free(ifftCfg);
	delete [] windowedSignal;
}

void ofxFftBasic::executeFft() {
	memcpy(windowedSignal, signal, sizeof(float) * signalSize);
	runWindow(windowedSignal);
	kiss_fft_cpx* cx_out = new kiss_fft_cpx[bins];
	kiss_fftr(fftCfg, windowedSignal, cx_out);
	for(int i = 0; i < bins; i++) {
		real[i] = cx_out[i].r;
		imag[i] = cx_out[i].i;
	}
	cartesianUpdated = true;
}

void ofxFftBasic::executeIfft() {
	kiss_fft_cpx* cx_in = new kiss_fft_cpx[bins];
	for(int i = 0; i < bins; i++) {
		cx_in[i].r = real[i];
		cx_in[i].i = imag[i];
	}
	kiss_fftri(ifftCfg, cx_in, signal);
	runInverseWindow(signal);
	signalUpdated = true;
}
