#pragma once

class ofxDisplayList {
private:
	GLuint _id;
	bool _ready;
	int* _references;
public:
	ofxDisplayList() {
		_id = glGenLists(1);
		_ready = false;
		_references = new int;
		(*_references) = 1;
	}
	ofxDisplayList(const ofxDisplayList& list) {
		_id = list._id;
		_ready = list._ready;
		_references = list._references;
		(*_references)++;
	}
	~ofxDisplayList() {
		(*_references)--;
		if(*_references == 0) {
			delete _references;
			glDeleteLists(_id, 1);
		}
	}
	bool ready() const {
		return _ready;
	}
	void update() {
		_ready = false;
	}
	void begin(bool execute = true) {
		glNewList(_id, execute ? GL_COMPILE_AND_EXECUTE : GL_COMPILE);
	}
	void end() {
		glEndList();
		_ready = true;
	}
	bool draw() {
		if(!_ready)
			return false;
		glCallList(_id);
		return true;
	}
};
