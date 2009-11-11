#include "testApp.h"

void testApp::setup(){
	kParticles = 8;
	float padding = 256;
	float maxVelocity = .5;
	for(int i = 0; i < kParticles * 1024; i++) {
		float x = ofRandom(padding, ofGetWidth() - padding);
		float y = ofRandom(padding, ofGetHeight() - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		Particle particle(x, y, xv, yv);
		particleSystem.add(particle);
	}

	particleSystem.setTimeStep(1);

	ofBackground(0, 0, 0);
}

void testApp::update(){
	particleSystem.setupForces();

	// apply per-particle forces
	for(int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
		// global force on other particles
		particleSystem.addRepulsionForce(cur, 3, 1);
		// forces on this particle
		cur.bounceOffWalls(0, 0, ofGetWidth(), ofGetHeight());
		cur.addDampingForce();
	}
	// single global forces
	particleSystem.addAttractionForce(ofGetWidth() / 2, ofGetHeight() / 2, 1500, 0.01);
	particleSystem.addRepulsionForce(mouseX, mouseY, 100, 2);
	particleSystem.update();
}

void testApp::draw(){
	ofSetColor(255, 255, 255);
	ofFill();
	particleSystem.draw();

	ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 32);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
}

void testApp::keyPressed(int key){
	if(key == 'p')
		ofSaveFrame();
}

void testApp::mousePressed(int x, int y, int button){
}
