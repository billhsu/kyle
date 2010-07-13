#include "testApp.h"

void testApp::setup() {
	width = ofGetWidth();
	height = ofGetHeight();

	fbo.allocate(width, height);

	setupNoise();
}

void testApp::setupNoise() {
	shader.setup("noise");
	shader.begin();
	const int octaves = 12;
	const float dropoff = .5;
	float total = 0;
	float weights[octaves];
	float scaling[octaves];
	for(int i = 0; i < octaves; i++) {
		weights[i] = 1. / exp2f(dropoff * i);
		scaling[i] = exp2f(i);
		total += weights[i];
	}
	shader.setUniform1v("weights", weights, octaves);
	shader.setUniform1v("scaling", scaling, octaves);
	shader.setUniform("normalization", 1.f / total);
	shader.setUniform("seed", ofRandom(8, 32));
	shader.end();
}

void testApp::update() {
	fbo.begin();
	shader.begin();

	float n = 32 * ofDist(mouseX, mouseY, width / 2, height / 2) / width;

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(-n, -n);
	glVertex2f(0, 0);

	glTexCoord2f(-n, n);
	glVertex2f(0, height);

	glTexCoord2f(n, -n);
	glVertex2f(width, 0);

	glTexCoord2f(n, n);
	glVertex2f(width, height);

	glEnd();

	shader.end();
	fbo.end();
}

void testApp::draw() {
	ofSetColor(255, 255, 255);
	fbo.draw(0, 0, width, height);

	ofSetColor(255, 0, 0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
