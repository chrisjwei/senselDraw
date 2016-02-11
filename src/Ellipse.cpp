#include "Ellipse.h"


Ellipse::Ellipse(){
  active = false;
}

void Ellipse::deactivate(){
  x = -1;
  y = -1;
  active = false;
}

void Ellipse::set(int xi, int yi, int w, int h, int rot, float force, ofColor c)
{
  x = xi;
  y = yi;
  width = w;
  height = h;
  rotation = rot;
  color = c;
  active = true;
  f = force;
}

void Ellipse::draw(){
  if (active){
    ofPushMatrix();
    ofTranslate(x, y);
    ofRotateZ(rotation);
    ofSetColor(color);
    ofDrawEllipse(0,0,width,height);
    ofPopMatrix();
  }
}

void Ellipse::drawCursor(int mode){
  if (active){
    ofColor topColor;

    ofPushMatrix();
    ofTranslate(ofGetWidth()-x, y);
    ofRotateZ(-rotation);
    if (mode == 0){
      topColor = ofColor::blue;
    } else if (mode == 1) {
      topColor = ofColor::red;
    } else {
      topColor = ofColor::black; 
    }
    ofSetColor(ofColor::grey.getLerped(topColor, ofMap(f, 0, 1.0, 0.3, 1.0,  true)));
    ofDrawEllipse(0,0,width,height);
    ofPopMatrix();
  }
}


