#pragma once

#include "ofMain.h"
#include "ofxSenselMorph.h"
#include "ofxBlur.h"
#include "Ellipse.h"
#include <stdlib.h>

#define MAX_KERN_SIZE 100

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
    
        void exit();
        void updateMesh(ofMesh *);
        void rescaleFbo();
        void drawUi();


    double kernel[MAX_KERN_SIZE][MAX_KERN_SIZE];

    bool drawingActive;
    int clearing;
    
    ofEasyCam cam;
    ofxSenselMorph sensel;


    Ellipse ellipse[MAX_CONTACTS];
    vector<Ellipse> ellipses;
    ofVec2f cursor[MAX_CONTACTS];
    int mode;
    int maxModes;

    int meshW, meshH, meshSize;

    ofMesh mesh;
    int selected;

    ofFbo fbo;
    int fboScale;
    ofxBlur blur;

    bool toggleFboDraw;
};
