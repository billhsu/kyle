/**********************************************************************

	This is a truncated version of a free FFT implementation, which
	originally held the following notes.

	- - --

  This class is a C++ wrapper for original code
  written by Dominic Mazzoni in September 2000

  This file contains a few FFT routines, including a real-FFT
  routine that is almost twice as fast as a normal complex FFT,
  and a power spectrum routine which is more convenient when
  you know you don't care about phase information.  It now also
  contains a few basic windowing functions.

  Some of this code was based on a free implementation of an FFT
  by Don Cross, available on the web at:

    http://www.intersrv.com/~dcross/fft.html

  The basic algorithm for his code was based on Numerical Recipes
  in Fortran.  I optimized his code further by reducing array
  accesses, caching the bit reversal table, and eliminating
  float-to-double conversions, and I added the routines to
  calculate a real FFT and a real power spectrum.

  Note: all of these routines use single-precision floats.
  I have found that in practice, floats work well until you
  get above 8192 samples.  If you need to do a larger FFT,
  you need to use doubles.

**********************************************************************/

#include "ofxFftBasic.h"

int **gFFTBitTable = NULL;
const int MaxFastBits = 16;

int IsPowerOfTwo(int x) {
	if (x < 2)
		return false;
	if (x & (x - 1))             /* Thanks to 'byang' for this cute trick! */
		return false;
	return true;
}

int NumberOfBitsNeeded(int PowerOfTwo) {
	int i;

	if (PowerOfTwo < 2) {
		cout << "Error: FFT called with size " << PowerOfTwo << endl;
		exit(1);
	}

	for (i = 0;; i++)
		if (PowerOfTwo & (1 << i))
			return i;
}

int ReverseBits(int index, int NumBits) {
	int i, rev;

	for (i = rev = 0; i < NumBits; i++) {
		rev = (rev << 1) | (index & 1);
		index >>= 1;
	}

	return rev;
}

inline int FastReverseBits(int i, int NumBits) {
	if (NumBits <= MaxFastBits)
		return gFFTBitTable[NumBits - 1][i];
	else
		return ReverseBits(i, NumBits);
}

void ofxFftBasic::FFT(int bins, bool InverseTransform, float *RealIn, float *ImagIn, float *RealOut, float *ImagOut) {
	int NumBits;                 /* Number of bits needed to store indices */
	int i, j, k, n;
	int BlockSize, BlockEnd;

	double angle_numerator = 2.0 * PI;
	float tr, ti;

	if (!IsPowerOfTwo(bins)) {
		cout << bins << " is not a power of two." << endl;
		return;
	}

	if (InverseTransform)
		angle_numerator = -angle_numerator;

	NumBits = NumberOfBitsNeeded(bins);

	// Do simultaneous data copy and bit-reversal ordering into outputs...
	for (i = 0; i < bins; i++) {
		j = FastReverseBits(i, NumBits);
		RealOut[j] = RealIn[i];
		ImagOut[j] = (ImagIn == NULL) ? 0.0 : ImagIn[i];
	}

	// Do the FFT itself...
	BlockEnd = 1;
	for (BlockSize = 2; BlockSize <= bins; BlockSize <<= 1) {

		double delta_angle = angle_numerator / (double) BlockSize;

		float sm2 = sin(-2 * delta_angle);
		float sm1 = sin(-delta_angle);
		float cm2 = cos(-2 * delta_angle);
		float cm1 = cos(-delta_angle);
		float w = 2 * cm1;
		float ar0, ar1, ar2, ai0, ai1, ai2;

		for (i = 0; i < bins; i += BlockSize) {
			ar2 = cm2;
			ar1 = cm1;

			ai2 = sm2;
			ai1 = sm1;

			for (j = i, n = 0; n < BlockEnd; j++, n++) {
				ar0 = w * ar1 - ar2;
				ar2 = ar1;
				ar1 = ar0;

				ai0 = w * ai1 - ai2;
				ai2 = ai1;
				ai1 = ai0;

				k = j + BlockEnd;
				tr = ar0 * RealOut[k] - ai0 * ImagOut[k];
				ti = ar0 * ImagOut[k] + ai0 * RealOut[k];

				RealOut[k] = RealOut[j] - tr;
				ImagOut[k] = ImagOut[j] - ti;

				RealOut[j] += tr;
				ImagOut[j] += ti;
			}
		}

		BlockEnd = BlockSize;
	}
}

/*
 * Real Fast Fourier Transform
 *
 * This function was based on the code in Numerical Recipes in C.
 * In Num. Rec., the inner loop is based on a single 1-based array
 * of interleaved real and imaginary numbers.  Because we have two
 * separate zero-based arrays, our indices are quite different.
 * Here is the correspondence between Num. Rec. indices and our indices:
 *
 * i1  <->  real[i]
 * i2  <->  imag[i]
 * i3  <->  real[n/2-i]
 * i4  <->  imag[n/2-i]
 */

void ofxFftBasic::RealFFT(int bins, float *RealIn, float *RealOut, float *ImagOut) {
	int Half = bins / 2;
	int i;

	float theta = PI / Half;

	float *tmpReal = new float[Half];
	float *tmpImag = new float[Half];

	for (i = 0; i < Half; i++) {
		tmpReal[i] = RealIn[2 * i];
		tmpImag[i] = RealIn[2 * i + 1];
	}

	FFT(Half, 0, tmpReal, tmpImag, RealOut, ImagOut);

	float wtemp = float (sin(0.5 * theta));

	float wpr = -2.0 * wtemp * wtemp;
	float wpi = float (sin(theta));
	float wr = 1.0 + wpr;
	float wi = wpi;

	int i3;

	float h1r, h1i, h2r, h2i;

	for (i = 1; i < Half / 2; i++) {

		i3 = Half - i;

		h1r = 0.5 * (RealOut[i] + RealOut[i3]);
		h1i = 0.5 * (ImagOut[i] - ImagOut[i3]);
		h2r = 0.5 * (ImagOut[i] + ImagOut[i3]);
		h2i = -0.5 * (RealOut[i] - RealOut[i3]);

		RealOut[i] = h1r + wr * h2r - wi * h2i;
		ImagOut[i] = h1i + wr * h2i + wi * h2r;
		RealOut[i3] = h1r - wr * h2r + wi * h2i;
		ImagOut[i3] = -h1i + wr * h2i + wi * h2r;

		wr = (wtemp = wr) * wpr - wi * wpi + wr;
		wi = wi * wpr + wtemp * wpi + wi;
	}

	RealOut[0] = (h1r = RealOut[0]) + ImagOut[0];
	ImagOut[0] = h1r - ImagOut[0];

	delete[]tmpReal;
	delete[]tmpImag;
}

void ofxFftBasic::setup(int signalSize, fftWindowType windowType) {
	ofxFft::setup(signalSize, windowType);

	gFFTBitTable = new int *[MaxFastBits];
	int len = 2;
	for (int b = 1; b <= MaxFastBits; b++) {
		gFFTBitTable[b - 1] = new int[len];
		for (int i = 0; i < len; i++)
			gFFTBitTable[b - 1][i] = ReverseBits(i, b);
		len <<= 1;
	}

	windowedSignal = new float[signalSize];
}

ofxFftBasic::~ofxFftBasic() {
	// delete the column arrays
	for (int b = 0; b < MaxFastBits; b++)
		delete [] gFFTBitTable[b];
	delete [] gFFTBitTable;

	delete [] windowedSignal;
}

void ofxFftBasic::executeFft() {
	memcpy(windowedSignal, signal, sizeof(float) * signalSize);
	runWindow(windowedSignal);
	RealFFT(signalSize, windowedSignal, real, imag);
	cartesianUpdated = true;
}

void ofxFftBasic::executeIfft() {
	FFT(signalSize, true, real, imag, signal, windowedSignal);
	runInverseWindow(signal);
	signalUpdated = true;
}
