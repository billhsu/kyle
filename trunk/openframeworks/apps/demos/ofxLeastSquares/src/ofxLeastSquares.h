#pragma once

#include "ofxOpenCv.h"

/*
	Should use * for most code, and pass vector::iterators to them for speed.
*/

/*
	ofxLeastSquares generates a mapping function that models an unknown	system
	based on example input and output measurements. There are 4 steps:

	1 setup() how many input and output variables there are
	2 add() your example input and output data
	3 update() the mapping function based on all the data
	4 map() new input to modeled output
*/

class ofxLeastSquares {
protected:
	int inputCount, outputCount;
	vector< vector<float> > inputData, outputData;
	CvMat* mapMat;
public:
	ofxLeastSquares() :
			mapMat(NULL) {
	}
	~ofxLeastSquares() {
		if(mapMat != NULL)
			cvReleaseMat(&mapMat);
	}
	/*
		1 First you set up the least squares system by defining how many input
		and output variables are involved.
	*/
	void setup(int inputCount, int outputCount) {
		this->inputCount = inputCount;
		this->outputCount = outputCount;
		mapMat = cvCreateMat(inputCount, outputCount, CV_32FC1);
	}
	/*
		2 Then you add data one entry at a time: pairs of input and output
		measurements.
	*/
	void add(vector<float> input, vector<float> output) {
		inputData.push_back(input);
		outputData.push_back(output);
	}
	void add(const float* input, const float* output) {
		vector<float> inputVec, outputVec;
		inputVec.resize(inputCount);
		outputVec.resize(outputCount);
		copy(input, input + inputCount, inputVec.begin());
		copy(output, output + outputCount, outputVec.begin());
		add(inputVec, outputVec);
	}
	/*
		3 Once you've added all your data, you need to solve the least squares
		system. You can do this repeatedly as you add more data.
	*/
	void update() {
		int n = inputData.size();

		CvMat* inputMat = cvCreateMat(n, inputCount, CV_32FC1);
		CvMat* outputMat = cvCreateMat(n, outputCount, CV_32FC1);

		float* inputPtr = (float*) cvPtr1D(inputMat, 0);
		float* outputPtr = (float*) cvPtr1D(outputMat, 0);

		for(int i = 0; i < n; i++) {
			for(int j = 0; j < inputCount; j++)
				*(inputPtr++) = inputData[i][j];
			for(int j = 0; j < outputCount; j++)
				*(outputPtr++) = outputData[i][j];
		}

		cvSolve(inputMat, outputMat, mapMat, CV_SVD);

		cvReleaseMat(&inputMat);
		cvReleaseMat(&outputMat);
	}
	/*
		4 Finally, use the model to map a new input vector into an output vector.
	*/
	vector<float> map(vector<float> input) {
		CvMat* inMat = cvCreateMat(1, inputCount, CV_32FC1);
		float* inPtr = (float*) cvPtr1D(inMat, 0);
		for(int i = 0; i < inputCount; i++)
			*(inPtr++) = input[i];

		CvMat* outMat = cvCreateMat(1, outputCount, CV_32FC1);
		float* outPtr = (float*) cvPtr1D(outMat, 0);
		cvMatMul(inMat, mapMat, outMat);

		vector<float> result;
		result.resize(outputCount);
		for(int i = 0; i < outputCount; i++)
			result[i] = *(outPtr++);

		cvReleaseMat(&inMat);
		cvReleaseMat(&outMat);

		return result;
	}
	vector<float> map(const float* input) {
	}
	/*
		For those times when you need to start over.
	*/
	void clear() {
		inputData.clear();
		outputData.clear();
	}
};
