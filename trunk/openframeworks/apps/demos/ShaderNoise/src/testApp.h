#pragma once

#include "ofMain.h"
#include "ofxFBOTexture.h"
#include "ofxShader.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void setupNoise();

	ofxFBOTexture fbo;
	ofxShader shader;
	int width, height;
};
