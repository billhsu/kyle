#ifndef _TEST_APP
#define _TEST_APP

#include "ofxConnexion.h"

#include "ofMain.h"

class testApp : public ofBaseApp{

public:
	void setup();
	~testApp();
	void update();
	void draw();
	
	void connexionMoved(ConnexionData& data);
};

#endif
