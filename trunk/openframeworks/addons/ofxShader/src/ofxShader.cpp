#include "ofxShader.h"

ofxShader::ofxShader() :
	bLoaded(false) {
}

ofxShader::~ofxShader() {
	unload();
}

void ofxShader::setup(string shaderName) {
	setup(shaderName, shaderName);
}

void ofxShader::setup(string vertexName, string fragmentName) {
	vertexName += ".vert";
	fragmentName += ".frag";
	setupInline(loadTextFile(vertexName), loadTextFile(fragmentName));
}

void ofxShader::setupInline(string vertexShaderSource, string fragmentShaderSource) {
	unload();
	if (GLEE_ARB_shader_objects) {
		vertexShader = (GLhandleARB) glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = (GLhandleARB) glCreateShader(GL_FRAGMENT_SHADER);

		const char* vsptr = vertexShaderSource.c_str();
		int vssize = vertexShaderSource.size();
		glShaderSourceARB(vertexShader, 1, &vsptr, &vssize);

		const char* fsptr = fragmentShaderSource.c_str();
		int fssize = fragmentShaderSource.size();
		glShaderSourceARB(fragmentShader, 1, &fsptr, &fssize);

		glCompileShader((GLuint) vertexShader);

		GLint compileStatus;
		glGetProgramiv((GLuint) vertexShader, GL_COMPILE_STATUS, &compileStatus);
		if(compileStatus == GL_TRUE)
			ofLog(OF_LOG_VERBOSE, "vertex shader compiled");
		else if (compileStatus == GL_FALSE)
			ofLog(OF_LOG_ERROR, "vertex shader failed to compile");

		char infobuffer[1000];
		GLsizei infobufferlen = 0;

		glGetInfoLogARB(vertexShader, 999, &infobufferlen, infobuffer);
		if (infobufferlen != 0) {
			infobuffer[infobufferlen] = '\0';
			string msg = "vertex shader reports:\n";
			ofLog(OF_LOG_ERROR, msg + infobuffer);
			return;
		}

		glCompileShader((GLuint) fragmentShader);

		glGetProgramiv((GLuint) fragmentShader, GL_COMPILE_STATUS, &compileStatus);
		if(compileStatus == GL_TRUE)
			ofLog(OF_LOG_VERBOSE, "fragment shader compiled");
		else if (compileStatus == GL_FALSE)
			ofLog(OF_LOG_ERROR, "fragment shader failed to compile");

		glGetInfoLogARB(fragmentShader, 999, &infobufferlen, infobuffer);
		if(infobufferlen != 0) {
			infobuffer[infobufferlen] = '\0';
			string msg = "fragment shader reports:\n";
			ofLog(OF_LOG_ERROR, msg + infobuffer);
			return;
		}

		shader = glCreateProgramObjectARB();
		glAttachObjectARB(shader, vertexShader);
		glAttachObjectARB(shader, fragmentShader);
		glLinkProgramARB(shader);

		bLoaded = true;
	} else {
		cout << "Sorry, it looks like you can't run 'ARB_shader_objects'." << endl;
		cout << "Please check the capabilites of your graphics card." << endl;
		cout << "http://www.ozone3d.net/gpu_caps_viewer/" << endl;
	}
}

void ofxShader::unload() {
	if(bLoaded) {
		if (vertexShader) {
			glDetachObjectARB(shader, vertexShader);
			glDeleteObjectARB(vertexShader);
			vertexShader = 0;
		}
		if (fragmentShader) {
			glDetachObjectARB(shader, fragmentShader);
			glDeleteObjectARB(fragmentShader);
			fragmentShader = 0;
		}
		if (shader) {
			glDeleteObjectARB(shader);
			shader = 0;
		}
	}
	bLoaded = false;
}

void ofxShader::begin() {
	if (bLoaded == true)
		glUseProgramObjectARB(shader);
}

void ofxShader::end() {
	if (bLoaded == true)
		glUseProgramObjectARB(0);
}

void ofxShader::setTexture(const char* name, ofBaseHasTexture& img, int textureLocation) {
	setTexture(name, img.getTextureReference(), textureLocation);
}

void ofxShader::setTexture(const char* name, ofTexture& tex, int textureLocation) {
	if(bLoaded) {
		tex.bind();
		setUniform(name, textureLocation);
	}
}

void ofxShader::setUniform(const char* name, int v1) {
	if(bLoaded)
		glUniform1iARB(getLoc(name), v1);
}

void ofxShader::setUniform(const char* name, int v1, int v2) {
	if(bLoaded)
		glUniform2iARB(getLoc(name), v1, v2);
}

void ofxShader::setUniform(const char* name, int v1, int v2, int v3) {
	if(bLoaded)
		glUniform3iARB(getLoc(name), v1, v2, v3);
}

void ofxShader::setUniform(const char* name, int v1, int v2, int v3, int v4) {
	if(bLoaded)
		glUniform4iARB(getLoc(name), v1, v2, v3, v4);
}

void ofxShader::setUniform(const char* name, float v1) {
	if(bLoaded)
		glUniform1fARB(getLoc(name), v1);
}

void ofxShader::setUniform(const char* name, float v1, float v2) {
	if(bLoaded)
		glUniform2fARB(getLoc(name), v1, v2);
}

void ofxShader::setUniform(const char* name, float v1, float v2, float v3) {
	if(bLoaded)
		glUniform3fARB(getLoc(name), v1, v2, v3);
}

void ofxShader::setUniform(const char* name, float v1, float v2, float v3, float v4) {
	if(bLoaded)
		glUniform4fARB(getLoc(name), v1, v2, v3, v4);
}

void ofxShader::setUniform1v(const char* name, int* v, int count) {
	if(bLoaded)
		glUniform1ivARB(getLoc(name), count, v);
}

void ofxShader::setUniform2v(const char* name, int* v, int count) {
	if(bLoaded)
		glUniform2ivARB(getLoc(name), count, v);
}

void ofxShader::setUniform3v(const char* name, int* v, int count) {
	if(bLoaded)
		glUniform3ivARB(getLoc(name), count, v);
}

void ofxShader::setUniform4v(const char* name, int* v, int count) {
	if(bLoaded)
		glUniform4ivARB(getLoc(name), count, v);
}

void ofxShader::setUniform1v(const char* name, float* v, int count) {
	if(bLoaded)
		glUniform1fvARB(getLoc(name), count, v);
}

void ofxShader::setUniform2v(const char* name, float* v, int count) {
	if(bLoaded)
		glUniform2fvARB(getLoc(name), count, v);
}

void ofxShader::setUniform3v(const char* name, float* v, int count) {
	if(bLoaded)
		glUniform3fvARB(getLoc(name), count, v);
}

void ofxShader::setUniform4v(const char* name, float* v, int count) {
	if(bLoaded)
		glUniform4fvARB(getLoc(name), count, v);
}

void ofxShader::setAttribute(const char* name, short v1) {
	if(bLoaded)
		glVertexAttrib1s(getLoc(name), v1);
}

void ofxShader::setAttribute(const char* name, short v1, short v2) {
	if(bLoaded)
		glVertexAttrib2s(getLoc(name), v1, v2);
}

void ofxShader::setAttribute(const char* name, short v1, short v2, short v3) {
	if(bLoaded)
		glVertexAttrib3s(getLoc(name), v1, v2, v3);
}

void ofxShader::setAttribute(const char* name, short v1, short v2, short v3, short v4) {
	if(bLoaded)
		glVertexAttrib4s(getLoc(name), v1, v2, v3, v4);
}

void ofxShader::setAttribute(const char* name, float v1) {
	if(bLoaded)
		glVertexAttrib1f(getLoc(name), v1);
}

void ofxShader::setAttribute(const char* name, float v1, float v2) {
	if(bLoaded)
		glVertexAttrib2f(getLoc(name), v1, v2);
}

void ofxShader::setAttribute(const char* name, float v1, float v2, float v3) {
	if(bLoaded)
		glVertexAttrib3f(getLoc(name), v1, v2, v3);
}

void ofxShader::setAttribute(const char* name, float v1, float v2, float v3, float v4) {
	if(bLoaded)
		glVertexAttrib4f(getLoc(name), v1, v2, v3, v4);
}

void ofxShader::setAttribute(const char* name, double v1) {
	if(bLoaded)
		glVertexAttrib1d(getLoc(name), v1);
}

void ofxShader::setAttribute(const char* name, double v1, double v2) {
	if(bLoaded)
		glVertexAttrib2d(getLoc(name), v1, v2);
}

void ofxShader::setAttribute(const char* name, double v1, double v2, double v3) {
	if(bLoaded)
		glVertexAttrib3d(getLoc(name), v1, v2, v3);
}

void ofxShader::setAttribute(const char* name, double v1, double v2, double v3, double v4) {
	if(bLoaded)
		glVertexAttrib4d(getLoc(name), v1, v2, v3, v4);
}

void ofxShader::setAttributeLocation(const char* name, int index) {
	glBindAttribLocation((GLuint) shader, index, name);
}

int ofxShader::getAttributeLocation(const char* name) {
	return glGetAttribLocationARB(shader, name);
}

inline GLint ofxShader::getLoc(const char* name) {
	return glGetUniformLocationARB(shader, name);
}

void ofxShader::printActiveUniforms() {
	GLint numUniforms = 0;
	glGetProgramiv((GLuint) shader, GL_ACTIVE_UNIFORMS, &numUniforms);
	cout << numUniforms << " uniforms:" << endl;

	GLint uniformMaxLength = 0;
	glGetProgramiv((GLuint) shader, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

	GLint count = -1;
	GLenum type = 0;
	GLchar* uniformName = new GLchar[uniformMaxLength];
	for(GLint i = 0; i < numUniforms; i++) {
		GLsizei length;
		glGetActiveUniform((GLuint) shader, i, uniformMaxLength, &length, &count, &type, uniformName);
		cout << " [" << i << "] ";
		for(int j = 0; j < length; j++)
			cout << uniformName[j];
		cout << " @ index " << glGetUniformLocation((GLuint) shader, uniformName) << endl;
	}
	delete [] uniformName;
}

void ofxShader::printActiveAttributes() {
	GLint numAttributes = 0;
	glGetProgramiv((GLuint) shader, GL_ACTIVE_ATTRIBUTES, &numAttributes);
	cout << numAttributes << " attributes:" << endl;

	GLint attributeMaxLength = 0;
	glGetProgramiv((GLuint) shader, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength);

	GLint count = -1;
	GLenum type = 0;
	GLchar* attributeName = new GLchar[attributeMaxLength];
	for(GLint i = 0; i < numAttributes; i++) {
		GLsizei length;
		glGetActiveAttrib((GLuint) shader, i, attributeMaxLength, &length, &count, &type, attributeName);
		cout << " [" << i << "] ";
		for(int j = 0; j < length; j++)
			cout <<attributeName[j];
		cout << " @ index " << glGetAttribLocation((GLuint) shader, attributeName) << endl;
	}
	delete [] attributeName;
}

string ofxShader::loadTextFile(string filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str());
	if(file.is_open()) {
		string text;
		while(!file.eof()) {
			string line;
			getline(file, line);
			text += line + '\n';
		}
		file.close();
		stringstream msg;
		msg << "Loaded " << filename << ", " << text.size() << " characters.";
		ofLog(OF_LOG_VERBOSE, msg.str());
		return text;
	} else {
		ofLog(OF_LOG_ERROR, "Could not open " + filename);
	}
}

void ofxShader::logError() {
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		const GLubyte* errString = gluErrorString(err);
		ofLog(OF_LOG_ERROR, (const char*) errString);
	}
}
