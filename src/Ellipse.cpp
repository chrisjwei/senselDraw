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

void Ellipse::drawCursor(){
  if (active){
    ofPushMatrix();
    ofTranslate(ofGetWidth()-x, y);
    ofRotateZ(-rotation);
    ofSetColor(ofColor::blue.getLerped(ofColor::red, f*1.2));
    ofDrawEllipse(0,0,width,height);
    ofPopMatrix();
  }
}


