#pragma once

#include "ofMain.h"
#include "ofxSenselMorph.h"
#include "ofxBlur.h"
#include "Ellipse.h"
#include <stdlib.h>
/* best not to exceed 30ish frames */
#define NUM_FRAMES 30

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
    
        void exit();
        void updateMesh(ofMesh *);
        void drawUi();

    bool drawingActive, hideMenu;
    int clearing;
    
    ofEasyCam cam;
    ofxSenselMorph sensel;
    ofLight light;


    Ellipse ellipse[MAX_CONTACTS];
    vector<Ellipse> ellipses;
    ofVec2f cursor[MAX_CONTACTS];
    int mode;
    int maxModes;
    int viewMode;
    int maxViewModes;

    int meshW, meshH, meshSize;

    ofMesh mesh;
    int selected;

    ofFbo fbo[NUM_FRAMES];
    int fboIndex;
    int fboScale;
};
