#include "ofApp.h"

/** rescaleFbo used to rescale our max height that is represented by the color
 *  white. A 50% opacity black rectangle is draw into our fbo, which halfs
 *  the brightness of all pixels in the fbo. We then double the fboScale to
 *  account for the halfing in height and decrease the opacity of future
 *  ellipses draw into the buffer */
void ofApp::rescaleFbo(){
  fbo.begin();
    ofSetColor(0,0,0,128);
    ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
  fbo.end();
  fboScale *= 2;
}

/** goes through each vertex in the mesh and samples the fbo based on each
 *  vertex's position. Based on the brightness of the pixel sampled, the
 *  new z value is set accordingly. If a pixel brightness is beyond some
 *  threshold, then the fbo is rescaled
 */
void ofApp::updateMesh(ofMesh *mesh){
  ofPixels pixels;
  fbo.readToPixels(pixels);
  bool rescale = false;
  for (int row=0; row<meshH; row++) {
    for (int col=0; col<meshW; col++) {
      int i = col + row * meshW;
      ofVec3f currPos = mesh->getVertex(i);

      int x = ofMap(col, 0, meshW, 0, ofGetWidth());
      x = ofGetWidth() - x;
      int y = ofMap(row, 0, meshH, 0, ofGetHeight());
      ofColor currColor = pixels.getColor(x,y);
      if (currColor.getBrightness() > 200) rescale = true;
      currPos.z = fboScale * 4 * currColor.getBrightness();
      mesh->setVertex(i, currPos);
    }
  }
  if (rescale){
    rescaleFbo();
  }
}

void ofApp::drawUi(){
  ofDrawBitmapStringHighlight("SenselDraw",32, 32);
  ofDrawBitmapStringHighlight(">> (c)lear buffer | (t)oggle views | rotate (m)odes", 32, 64);
  ofDrawBitmapStringHighlight(">> current mode:", 32, 96);
  if (mode == 0){
    ofDrawBitmapStringHighlight("[+]", 180, 96);  
  } else if (mode == 1){
    ofDrawBitmapStringHighlight("[-]", 180, 96);  
  } else {
    ofDrawBitmapStringHighlight("[?]", 180, 96);  
  }
  
}

//--------------------------------------------------------------
void ofApp::setup(){
  ofEnableAlphaBlending();

  sensel.setup();

  /* setup some variables */
  fboScale = 1;          /* our fbo's scale */
  toggleFboDraw = false; /* used to draw fbo instead of mesh */
  drawingActive = true;  /* used to deactivate drawing */
  mode = 0;              /* 0 = adding, 1 = subtracting */
  maxModes = 2;

  /* setup camera */
  cam.setPosition(ofVec3f(0, 0, 0));
  cam.setTarget(ofVec3f(0,0,0));
  cam.setDistance(2000);
  cam.roll(180.f);

  /* setup our mesh */
  meshSize = 30;
  meshH = (ofGetHeight()/meshSize)+1;
  meshW = (ofGetWidth()/meshSize)+1;
  
  /* place vertexes into our mesh */  
  for (int row=0; row<meshH; row++) {
    for (int col=0; col<meshW; col++) { 
      mesh.addVertex(ofVec3f((col) * meshSize, (row) * meshSize, 0));
      //mesh.addTexCoord(ofPoint(x * (ofGetWidth() / meshW), y * (ofGetHeight() / meshH)));
      mesh.addColor(ofColor::grey);
    }
  }
  /* link vertexes into triangle mesh */
  for (int y=0; y<meshH-1; y++) {
    for (int x=0; x<meshW-1; x++) {
      int i1 = x + meshW * y;
      int i2 = x+1 + meshW * y;
      int i3 = x + meshW * (y+1);
      int i4 = x+1 + meshW * (y+1);
      mesh.addTriangle( i1, i2, i3 );
      mesh.addTriangle( i2, i4, i3 );
    }
  }
    
  /* setup frame buffer */
  fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
  fbo.begin();
    ofClear(0,0,0,1);
  fbo.end();
}

//--------------------------------------------------------------
void ofApp::update(){
  sensel.update();
  /* clear contact points */
  for (unsigned int i=0; i<MAX_CONTACTS; i++){
    ellipse[i].deactivate();
  }
  /* get new contact points */
  for (unsigned int i=0; i<sensel.getContacts().size(); i++) {
    int contactId = sensel.getContacts()[i].contactID;
    int x = ofMap(sensel.getContacts()[i].position.x,0,1,0,ofGetWidth());
    int y = ofMap(sensel.getContacts()[i].position.y,0,1,0,ofGetHeight());
    int h = sensel.getContacts()[i].majorAxis;
    int w = sensel.getContacts()[i].minorAxis;
    float f = sensel.getContacts()[i].force;
    int r = sensel.getContacts()[i].orientation;

    ofColor color;
    if (mode == 0){
      color = ofColor(255, 255, 255, 64*f/fboScale);  
    } else if (mode == 1){
      color = ofColor(0, 0, 0, 64*f/fboScale);  
    } else {
      return;
    }
    //string contactType = sensel.getContacts()[i].contactType;
    ellipse[contactId].set(x,y,w*20,h*20,r,f,color);
  }
  /* update our fbo */
  if (drawingActive){
    fbo.begin();
      for (int i =0; i < MAX_CONTACTS; i++ ){ellipse[i].draw();}
    fbo.end();
  }
  /* update mesh */
  updateMesh(&mesh);
}

//--------------------------------------------------------------
void ofApp::draw(){
  if (toggleFboDraw) fbo.draw(0,0);
  else {
    cam.begin();
    ofPushMatrix();
      ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
      for (int i =0; i < MAX_CONTACTS; i++ ){ellipse[i].drawCursor();} 
      mesh.drawWireframe();
    ofPopMatrix();
    cam.end();
  }
  drawUi();
}

void ofApp::exit(){
  sensel.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'c'){
    fbo.begin();
    ofClear(0,0,0,1);
    fbo.end();
    updateMesh(&mesh);
  }
  if (key == 't'){
    toggleFboDraw = !toggleFboDraw;
  } 
  if (key == 'm'){
    mode = (mode + 1) % maxModes;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}
