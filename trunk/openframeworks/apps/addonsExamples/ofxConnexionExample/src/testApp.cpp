#include "testApp.h"

void testApp::setup(){
	// pass in a string that is the name of your application
	// so the driver knows where to send events
	ofxConnexion::start("3dConnexion"); // change this so it doesn't have to be a pascal string
	
	// if you want to get data events, you can add a listener toofxConnexion::connexionEvent
	ofAddListener(ofxConnexion::connexionEvent, this, &testApp::connexionMoved);
	
	ofBackground(0, 0, 0);
}

testApp::~testApp() {
	// make sure to stop the space navigator before you're done
	ofxConnexion::stop();
}

void testApp::update(){
}

void testApp::connexionMoved(ConnexionData& data) {
	// just got the latest data
	// this would be a good place to get data from multiple devices
	
	// use the button state to set the LED
	ofxConnexion::setLed(data.getButton(0));
}

void testApp::draw(){
	// set up some basic lighting
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	float noAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float whiteDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float position[] = {1.0f, -1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2 , 0);
	
	// get the most up to date data
	ConnexionData& data = ofxConnexion::connexionData;
	
	// some of these things are negative, but might be different
	// depending on how you've configured your space navigator
	ofTranslate(
		data.translation[0],
		data.translation[1],
		-data.translation[2]);
	ofRotateX(-data.rotation[0]);
	ofRotateY(-data.rotation[1]);
	ofRotateZ(data.rotation[2]);
	
	ofSetColor(255, 255, 255);
	ofRotateX(180);
	glutSolidTeapot(200);
}
