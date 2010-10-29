#pragma once

#include "ofMain.h"
#include "ofxShader.h"
#include "ofxFbo.h"
#include "ofxVectorMath.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	list<ofxVec3f> points;
	ofxVec3f position;
	ofxVec3f velocity;
	float erratic, jerk, size;
	
	ofxVec3f avg;
	
	ofxShader dofShader;
};
