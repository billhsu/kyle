#include "testApp.h"

void testApp::setup(){
	kParticles = 16;
	float size = 256;
	float maxVelocity = 32;
	for(int i = 0; i < kParticles * 1024; i++) {
		float x = ofGetWidth() / 2 + ofRandom(-size, size);
		float y = ofGetHeight() / 2 + ofRandom(-size, size);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		float mass = ofRandom(1, 5);
		Particle particle(x, y, mass, xv, yv);
		particleSystem.add(particle);
	}

	particleSystem.setExact(false);
	particleSystem.setGravitationalConstant(.01);
	particleSystem.setTimeStep(.1);
	particleSystem.setFriction(1);

	ofBackground(0, 0, 0);
}

void testApp::update(){
	particleSystem.update();
}

void testApp::draw(){
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 128);
	ofFill();
	particleSystem.draw();
	ofDisableAlphaBlending();

	ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 32);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
}

void testApp::keyPressed(int key){
	if(key == 'p')
		ofSaveFrame();
}

void testApp::mousePressed(int x, int y, int button){
	float mass = ofRandom(1, 20);
	Particle particle(x, y, mass);
	particleSystem.add(particle);
}
