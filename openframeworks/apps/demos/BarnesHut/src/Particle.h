#pragma once

#include "Body.h"
#include "ofGraphics.h"

class Particle : public Body {
public:
	float xv, yv;
	float xf, yf;
	Particle(float _x, float _y, float _mass,
		float _xv = 0, float _yv = 0) :
		Body(_x, _y, _mass),
		xv(_xv), yv(_yv) {
	}
	void updatePosition(float timeStep) {
		x += xv * timeStep;
		y += yv * timeStep;
	}
	void zeroForce() {
		xf = 0;
		yf = 0;
	}
	void applyForce(float gravitationalConstant, float friction) {
		xf *= gravitationalConstant * mass;
		yf *= gravitationalConstant * mass;
		xv += xf;
		yv += yf;
		xv *= friction;
		yv *= friction;
	}
	void draw() {
		//ofRect(x, y, mass, mass);
		ofCircle(x, y, mass);
	}
};
