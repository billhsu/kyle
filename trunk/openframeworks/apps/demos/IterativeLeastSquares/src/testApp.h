#pragma once

#include "ofMain.h"
#include "ofxLeastSquares.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void randomize();
	void keyPressed(int);
	void mousePressed(int, int, int);
	void mouseDragged(int, int, int);

	vector<float> makeInput(float);
	vector<float> makeOutput(float);
	void drawMap(float, float);

	int n;
	int inputCount, outputCount;

	vector<ofPoint> data;

	ofxLeastSquares ls;
};
