#pragma once

#include "ofMain.h"
#include "ofxLeastSquares.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void mousePressed(int, int, int);
	void mouseDragged(int, int, int);

	vector<float> makeInput(float, float);
	vector<float> makeOutput(float, float);
	void drawMap(float, float);

	int gdiv;
	int xdiv, ydiv, n;
	int inputCount, outputCount;

	vector<ofPoint> reference, measured;

	ofxLeastSquares ls;
};
