#include "testApp.h"

void testApp::setup() {
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();

	n = 500;

	inputCount = 2;
	outputCount = 1;

	ls.setup(inputCount, outputCount);

	randomize();
}

void testApp::randomize() {
	data.clear();

	float m = .5;
	float b = 100;
	for(int i = 0; i < n; i++) {
		float x = ofRandom(0, ofGetWidth());
		ofPoint cur(x, m * x + b);
		if(ofRandom(0, 1) < .25) {
			// shot noise
			cur.y = ofRandom(0, ofGetHeight());
		} else {
			// gaussian noise
			int g = 10;
			float sn = ofRandom(-g, g) + ofRandom(-g, g) + ofRandom(-g, g);
			cur.y += sn;
		}
		data.push_back(cur);
	}
}

void testApp::update() {
	ls.clear();

	for(int i = 0; i < n; i++) {
		ofPoint& pt = data[i];
		ls.add(makeInput(pt.x), makeOutput(pt.y));
	}

	ls.removeOutliers(ofMap(mouseX, 0, ofGetWidth(), 4, .1));
}

void testApp::draw() {
	ofBackground(0, 0, 0);

	vector< vector<float> > map = ls.getMap();
	ofSetColor(255, 255, 255);
	ofDrawBitmapString(
		ofToString(map[0][0]) + " " + ofToString(map[1][0]),
		10, 15);

	const vector<float>& error = ls.getError();
	const vector<bool>& outliers = ls.getOutliers();
	for(int i = 0; i < n; i++) {
		ofPoint& pt = data[i];
		vector<float> rep = ls.map(makeInput(pt.x));

		if(outliers[i])
			ofSetColor(255, 0, 0, 128);
		else
			ofSetColor(0, 0, 255, 128);
		ofLine(pt.x, pt.y, pt.x, rep[0]);
		ofFill();
		//ofCircle(pt.x, pt.y, 2);

		ofNoFill();
		//ofCircle(pt.x, pt.y, error[i]);
	}

	ofSetColor(255, 255, 255, 128);
	ofLine(0, 100, ofGetWidth(), ofGetWidth() * .5 + 100);

	ofSetColor(255, 0, 255, 128);
	vector<float> a = ls.map(makeInput(0));
	vector<float> b = ls.map(makeInput(ofGetWidth()));
	ofLine(0, a[0], ofGetWidth(), b[0]);
}

vector<float> testApp::makeInput(float x) {
	vector<float> in;
	in.resize(inputCount);
	in[0] = x;
	in[1] = 1;
	return in;
}

vector<float> testApp::makeOutput(float y) {
	vector<float> out;
	out.resize(outputCount);
	out[0] = y;
	return out;
}

void testApp::keyPressed(int key) {
	randomize();
}

void testApp::mousePressed(int x, int y, int button) {
	mouseDragged(x, y, button);
}

void testApp::mouseDragged(int x, int y, int button) {
	float mind = 0;
	int mini = 0;
	ofPoint mouse(x, y);
	for(int i = 0; i < n; i++) {
		ofPoint& cur = data[i];
		float curd = ofDist(mouse.x, mouse.y, cur.x, cur.y);
		if(i == 0 || curd < mind) {
			mini = i;
			mind = curd;
		}
	}
	data[mini].set(x, y);
}
