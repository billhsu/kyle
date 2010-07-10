#pragma once

#include "ofMain.h"
#include <fstream>

class ofxShader {
public:
	ofxShader();
	~ofxShader();

	void setup(string shaderName);
	void setup(string fragmentShaderName, string vertexShaderName);
	void logError();

	void begin();
	void end();


	void setSampler2d(char* name, ofImage& img, int textureLocation);

	// set a single uniform vector
	void setUniform(char* name, int v1);
	void setUniform(char* name, int v1, int v2);
	void setUniform(char* name, int v1, int v2, int v3);
	void setUniform(char* name, int v1, int v2, int v3, int v4);

	void setUniform(char* name, float v1);
	void setUniform(char* name, float v1, float v2);
	void setUniform(char* name, float v1, float v2, float v3);
	void setUniform(char* name, float v1, float v2, float v3, float v4);

	// set an array of uniform vectors
	void setUniform1v(char* name, int* v, int count = 1);
	void setUniform2v(char* name, int* v, int count = 1);
	void setUniform3v(char* name, int* v, int count = 1);
	void setUniform4v(char* name, int* v, int count = 1);

	void setUniform1v(char* name, float* v, int count = 1);
	void setUniform2v(char* name, float* v, int count = 1);
	void setUniform3v(char* name, float* v, int count = 1);
	void setUniform4v(char* name, float* v, int count = 1);

	// set attributes that vary per vertex
	void setAttribute(char* name, short v1);
	void setAttribute(char* name, short v1, short v2);
	void setAttribute(char* name, short v1, short v2, short v3);
	void setAttribute(char* name, short v1, short v2, short v3, short v4);

	void setAttribute(char* name, float v1);
	void setAttribute(char* name, float v1, float v2);
	void setAttribute(char* name, float v1, float v2, float v3);
	void setAttribute(char* name, float v1, float v2, float v3, float v4);

	void setAttribute(char* name, double v1);
	void setAttribute(char* name, double v1, double v2);
	void setAttribute(char* name, double v1, double v2, double v3);
	void setAttribute(char* name, double v1, double v2, double v3, double v4);


	GLhandleARB vertexShader;
	GLhandleARB fragmentShader;
	GLhandleARB shader;

protected:
	string loadShaderText(string filename);
	void unload();
	GLint getLoc(char* name);

	bool bLoaded;
};
