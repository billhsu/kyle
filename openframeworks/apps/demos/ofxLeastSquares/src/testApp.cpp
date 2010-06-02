#include "testApp.h"

void testApp::setup() {
	xdiv = 4;
	ydiv = 3;
	n = xdiv * ydiv;

	gdiv = 8;

	inputCount = 6;
	outputCount = 2;

	for(int y = 0; y < ydiv; y++) {
		for(int x = 0; x < xdiv; x++) {
			ofPoint cur(
				ofMap(x, -1, xdiv, 0, ofGetWidth()),
				ofMap(y, -1, ydiv, 0, ofGetHeight()));
			reference.push_back(cur);
			measured.push_back(cur);
		}
	}

	ls.setup(inputCount, outputCount);
}

void testApp::update() {
	ls.clear();

	for(int i = 0; i < n; i++) {
		ofPoint& ipt = measured[i];
		ofPoint& opt = reference[i];
		ls.add(makeInput(ipt.x, ipt.y), makeOutput(opt.x, opt.y));
	}

	ls.update();
}

void testApp::draw() {
	ofBackground(0, 0, 0);

	for(int i = 0; i < n; i++) {
		ofSetColor(255, 0, 0);
		ofPoint& from = reference[i];
		ofPoint& to = measured[i];
		ofCircle(to.x, to.y, 2);
		ofLine(from.x, from.y, to.x, to.y);

		ofSetColor(255, 255, 255);
		drawMap(to.x, to.y);
	}

	ofSetColor(0, 255, 0);
	for(int y = 0; y < ydiv * gdiv; y++) {
		for(int x = 0; x < xdiv * gdiv; x++) {
			float inx = ofMap(x, -1, xdiv * gdiv, 0, ofGetWidth());
			float iny = ofMap(y, -1, ydiv * gdiv, 0, ofGetHeight());
			drawMap(inx, iny);
		}
	}
}

// Ax + Bx^2 + Cy + Dy^2 + Exy + F
vector<float> testApp::makeInput(float x, float y) {
	vector<float> in;
	in.resize(inputCount);
	in[0] = x;
	in[1] = x * x;
	in[2] = y;
	in[3] = y * y;
	in[4] = x * y;
	in[5] = 1;
	return in;
}

vector<float> testApp::makeOutput(float x, float y) {
	vector<float> out;
	out.resize(outputCount);
	out[0] = x;
	out[1] = y;
	return out;
}

void testApp::drawMap(float x, float y) {
	vector<float> out = ls.map(makeInput(x, y));
	ofLine(x, y, out[0], out[1]);
}

void testApp::mousePressed(int x, int y, int button) {
	mouseDragged(x, y, button);
}

void testApp::mouseDragged(int x, int y, int button) {
	float mind = 0;
	int mini = 0;
	ofPoint mouse(x, y);
	for(int i = 0; i < n; i++) {
		ofPoint& cur = measured[i];
		float curd = ofDist(mouse.x, mouse.y, cur.x, cur.y);
		if(i == 0 || curd < mind) {
			mini = i;
			mind = curd;
		}
	}
	measured[mini].set(x, y);
}
