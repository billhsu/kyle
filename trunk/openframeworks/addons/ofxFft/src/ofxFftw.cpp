#include "ofxFftw.h"

ofxFftw::ofxFftw() :
	fftPlan(NULL) {
}

void ofxFftw::setup(int signalSize, fftWindowType windowType) {
	ofxFft::setup(signalSize, windowType);

	// more info on setting up a forward r2r fft here:
	// http://www.fftw.org/fftw3_doc/Real_002dto_002dReal-Transforms.html
	fftIn = (float*) fftwf_malloc(sizeof(float) * signalSize);
	fftOut = (float*) fftwf_malloc(sizeof(float) * signalSize);
	fftPlan = fftwf_plan_r2r_1d(signalSize, fftIn, fftOut, FFTW_R2HC,
	                            FFTW_DESTROY_INPUT | FFTW_MEASURE);

	// the difference between setting up an r2r ifft and fft
	// is using the flag/kind FFTW_HC2R instead of FFTW_R2HC:
	// http://www.fftw.org/fftw3_doc/Real_002dto_002dReal-Transform-Kinds.html
	ifftIn = (float*) fftwf_malloc(sizeof(float) * signalSize);
	ifftOut = (float*) fftwf_malloc(sizeof(float) * signalSize);
	ifftPlan = fftwf_plan_r2r_1d(signalSize, ifftIn, ifftOut, FFTW_HC2R,
	                             FFTW_DESTROY_INPUT | FFTW_MEASURE);
}

void ofxFftw::executeFft(float* input) {
	memcpy(fftIn, input, sizeof(float) * signalSize);
	runWindow(fftIn);
	fftwf_execute(fftPlan);
	// explanation of halfcomplex format:
	// http://www.fftw.org/fftw3_doc/The-Halfcomplex_002dformat-DFT.html
	setReal(fftOut); // will only copy the first half
	imag[0] = 0;
	for (int i = 1; i < bins; i++)
		imag[i] = fftOut[signalSize - i];
}

void ofxFftw::executeIfft(float* input) {
	// directly copy amplitude as real component
	memcpy(ifftIn, input, sizeof(float) * bins);
	// assume the phase is 0
	memset(&(ifftIn[bins]), 0, sizeof(float) * bins);
	fftwf_execute(ifftPlan);
	setSignal(ifftOut);
}

void ofxFftw::executeIfft(float* real, float* imag) {
	memcpy(ifftIn, real, sizeof(float) * bins);
	for (int i = 1; i < signalSize; i++)
		ifftIn[signalSize - i] = imag[i];
	fftwf_execute(ifftPlan);
	setSignal(ifftOut);
}

ofxFftw::~ofxFftw() {
	if (fftPlan != NULL) {
		fftwf_destroy_plan(fftPlan);
		fftwf_free(fftIn);
		fftwf_free(fftOut);

		fftwf_destroy_plan(ifftPlan);
		fftwf_free(ifftIn);
		fftwf_free(ifftOut);
		fftwf_cleanup();
	}
}
