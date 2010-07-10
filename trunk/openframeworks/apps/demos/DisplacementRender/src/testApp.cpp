#include "testApp.h"

void testApp::setup() {
	ofSetVerticalSync(false);

	displacement.loadImage("displacement.png");
	int w = (int) displacement.getWidth();
	int h = (int) displacement.getHeight();

	shader.setup("shader");
	shader.begin();

	displacement.getTextureReference().bind();
	// necessary to use GL_NEAREST for sampler2D in vertex shader
	// ofSetMinMagFilters(GL_NEAREST, GL_NEAREST); // alternatively, if using r62
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	shader.setTexture("displacementMap", displacement, 0);

	shader.end();

	shader.printActiveUniforms();
	shader.printActiveAttributes();

	rotX = 0;
	rotY = 0;

	glEnable(GL_DEPTH_TEST);
}

void testApp::update(){
}

void testApp::draw(){
	ofBackground(128, 128, 128);

	ofSetColor(0, 0, 0);

	glPushMatrix();

	int w = (int) displacement.getWidth();
	int h = (int) displacement.getHeight();

	glTranslatef(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	rotX = ofLerp(mouseX, rotX, rotSmooth);
	rotY = ofLerp(mouseY, rotY, rotSmooth);
	glRotatef(rotX, 0, 1, 0);
	glRotatef(-rotY, 1, 0, 0);
	glTranslatef(-w / 2, -h / 2, -320);

	shader.begin();

	if(!mesh.draw()) {
		mesh.begin();
		int step = 1;
		glBegin(GL_TRIANGLE_STRIP);
		for(int y = 0; y < h - step; y += step) {
			for(int x = 0; x < w; x += step) {
				glTexCoord2f(x, y);
				glVertex2f(x, y);

				glTexCoord2f(x, y + 1);
				glVertex2f(x, y + step);
			}
		}
		glEnd();
		mesh.end();
	}

	shader.end();

	glPopMatrix();

	ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
}
