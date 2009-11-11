#pragma once

#include "Tree.h"
#include "Particle.h"

class ParticleSystem {
protected:
	bool exact;
	float gravitationalConstant;
	float timeStep;
	float friction;

	vector<Particle> particles;
	void approximateSolve() {
		Tree tree;
		tree.build(particles);
		//tree.draw();
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& cur = particles[i];
			cur.zeroForce();
			tree.sumForces(cur);
			cur.applyForce(gravitationalConstant, friction);
		}
	}
	void exactSolve() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& cur = particles[i];
			cur.zeroForce();
			for(int j = 0; j < n; j++) {
				if(i != j) {
					float xd = particles[j].x - cur.x;
					float yd = particles[j].y - cur.y;
					float rsq = (xd * xd + yd * yd);
					float mor3 = particles[j].mass / rsq;
					cur.xf += xd * mor3;
					cur.yf += yd * mor3;
				}
			}
			cur.applyForce(gravitationalConstant, friction);
		}
	}
	void updatePositions() {
		int n = particles.size();
		for(int i = 0; i < n; i++)
			particles[i].updatePosition(timeStep);
	}
public:
	ParticleSystem() {
		this->exact = true;
		this->gravitationalConstant = 1;
		this->timeStep = 1;
		this->friction = 1;
	}
	void setExact(bool exact) {
		this->exact = exact;
	}
	void setGravitationalConstant(float gravitationalConstant) {
		this->gravitationalConstant = gravitationalConstant;
	}
	void setTimeStep(float timeStep) {
		this->timeStep = timeStep;
	}
	void setFriction(float friction) {
		this->friction = friction;
	}
	void add(Particle& p) {
		particles.push_back(p);
	}
	void update() {
		if(exact)
			exactSolve();
		else
			approximateSolve();
		updatePositions();
	}
	void draw() {
		for(int i = 0; i < particles.size(); i++)
			particles[i].draw();
	}
};
