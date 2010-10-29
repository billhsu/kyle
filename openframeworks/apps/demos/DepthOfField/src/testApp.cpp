#include "testApp.h"

void testApp::setup() {
	ofBackground(0, 0, 0);
	position.set(0, 0, 0);
	erratic = 20;
	jerk = .4;
	size = 4;
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	dofShader.setup("DOFLine");
	dofShader.printActiveUniforms();
	dofShader.printActiveAttributes();
	ofEnableAlphaBlending();
}

void testApp::update() {
	float t = ofGetElapsedTimef() * erratic;
	ofxVec3f acceleration(ofSignedNoise(t), ofSignedNoise(t + 234981), ofSignedNoise(t + 219282));
	acceleration.normalize();
	velocity += acceleration * jerk;
	velocity.normalize();
	position += velocity * size;
	points.push_back(position);
	if(points.size() > 1000000)
		points.pop_front();
}

void testApp::draw() {
	ofSetColor(255, 255, 255);
	
	glPushMatrix();
	
	glTranslatef(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	ofRotateY(ofGetElapsedTimef() * 20);
	ofRotateX(ofGetElapsedTimef() * 30);
	glTranslatef(-avg.x, -avg.y, -avg.z);
	
	dofShader.begin();
	dofShader.setUniform1f("lineWidth", 1);
	dofShader.setUniform1f("focusDistance", mouseX * 2);
	dofShader.setUniform1f("aperture", .03);
	
	GLint sideLocation = dofShader.getAttributeLocation("side");
	GLint nextLocation = dofShader.getAttributeLocation("next");
	
	glBegin(GL_TRIANGLE_STRIP);
	
	list<ofxVec3f>::iterator curItr, nextItr;
	curItr = points.begin();
	nextItr = points.begin();
	nextItr++;
	
	avg.set(0, 0, 0);
	
	while(nextItr != points.end()) {
		ofxVec3f& cur = *curItr;
		ofxVec3f& next = *nextItr;
		
		dofShader.setAttribute1f(sideLocation, -.5);
		dofShader.setAttribute3f(nextLocation, next.x, next.y, next.z);
		glVertex3f(cur.x, cur.y, cur.z);
		
		dofShader.setAttribute1f(sideLocation, +.5);
		dofShader.setAttribute3f(nextLocation, next.x, next.y, next.z);
		glVertex3f(cur.x, cur.y, cur.z);
		
		avg += cur;
		
		curItr++;
		nextItr++;
	}
	glEnd();
	
	avg /= (points.size() - 1);
	
	dofShader.end();
	
	glPopMatrix();
	
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
