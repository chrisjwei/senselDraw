#include "ofApp.h"

/** goes through each vertex in the mesh and samples the fbo based on each
 *  vertex's position. Based on the brightness of the pixel sampled, the
 *  new z value is set accordingly. If a pixel brightness is beyond some
 *  threshold, then the fbo is rescaled
 */
void ofApp::updateMesh(ofMesh *mesh){
  ofPixels pixels;
  fbo[fboIndex].readToPixels(pixels);
  for (int row=0; row<meshH; row++) {
    for (int col=0; col<meshW; col++) {
      int i = col + row * meshW;
      ofVec3f currPos = mesh->getVertex(i);

      int x = ofMap(col, 0, meshW, 0, ofGetWidth(), true);
      x = ofGetWidth() - x;
      int y = ofMap(row, 0, meshH, 0, ofGetHeight(), true);
      ofColor currColor = pixels.getColor(x,y);
      //if (currColor.getBrightness() > 200) rescale = true;
      float z = currPos.z;
      float newz = 8 * currColor.getLightness();
      currPos.z = newz;
      mesh->setVertex(i, currPos);

      int dz = newz-z;
      float amt;
      if (dz > 0){
        amt = ofMap(dz,0, fboScale * 32, 0, 1, true);
        mesh->setColor(i, ofColor::grey.getLerped(ofColor::blue, amt));
      } else if (dz < 0) {
        amt = ofMap(-dz,0, fboScale * 32, 0, 1, true);
        mesh->setColor(i, ofColor::grey.getLerped(ofColor::red, amt));
      } else {
        mesh->setColor(i, ofColor::grey);
      }
    }
  }
}

void ofApp::drawUi(){
  ofDrawBitmapStringHighlight("SenselDraw",32, 30);
  ofDrawBitmapStringHighlight(">> (c)lear | (v)iews | (m)odes | (h)ide", 32, 55);
  ofDrawBitmapStringHighlight(">> current mode:", 32, 80);
  int offset = 145;
  if (mode == 0){
    ofDrawBitmapStringHighlight("[+]", 32 + offset, 80);  
  } else if (mode == 1){
    ofDrawBitmapStringHighlight("[-]", 32 + offset, 80);  
  } else {
    ofDrawBitmapStringHighlight("[?]", 32 + offset, 80);  
  }
  
}

//--------------------------------------------------------------
void ofApp::setup(){
  ofEnableAlphaBlending();
  ofSetSmoothLighting(true);
  sensel.setup();

  /* setup some variables */
  fboScale = 1;          /* our fbo's scale */
  drawingActive = true;  /* used to deactivate drawing */
  hideMenu = false;      /* used to show or hide menu */
  mode = 0;              /* 0 = adding, 1 = subtracting */
  maxModes = 2;
  viewMode = 0;
  maxViewModes = 3;

  /* setup camera */
  cam.setPosition(ofVec3f(0, 0, 0));
  cam.setTarget(ofVec3f(0,0,0));
  cam.setDistance(2000);
  cam.roll(180.f);

  //light.setPointLight();
  //light.setPosition(ofVec3f(0,-1000,0));
  light.setPointLight();
  light.setPosition(ofVec3f(0,0,0));
  //light.setOrientation(ofVec3f(0, 90, 0));
  //light.setAmbientColor(ofColor::white);
  //light.setDiffuseColor(ofColor::red);
  //light.setSpecularColor(ofColor::blue);

  /* setup our mesh */
  meshSize = 15;
  meshH = (ofGetHeight()/meshSize)+1;
  meshW = (ofGetWidth()/meshSize)+1;
  
  /* place vertexes into our mesh */  
  for (int row=0; row<meshH; row++) {
    for (int col=0; col<meshW; col++) { 
      mesh.addVertex(ofVec3f(col * meshSize, row * meshSize, 0));
      //mesh.addTexCoord(ofPoint(col * meshSize, row * meshSize));
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
    
  /* setup frame buffers */
  fboIndex = 0;
  for (int i = 0; i < NUM_FRAMES; i++){
    fbo[i].allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    fbo[i].begin();
      ofClear(0,0,0,1);
    fbo[i].end();
  }
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

    float opacity = ofMap(0.5 + atan(10*f-9)/(3.1415926535),0.0,.9,0.0,256.0,true);

    if (mode == 0){
      color = ofColor(255, 255, 255, opacity);  
    } else if (mode == 1){
      color = ofColor(0, 0, 0, opacity);  
    } else {
      return;
    }
    //string contactType = sensel.getContacts()[i].contactType;
    ellipse[contactId].set(x,y,w*20,h*20,r,f,color);
  }
  /* update our fbo */
  if (drawingActive){
    fbo[fboIndex].begin();
      for (int i =0; i < MAX_CONTACTS; i++ ){ellipse[i].draw();}
    fbo[fboIndex].end();
  }
  /* update mesh */
  updateMesh(&mesh);
  fboIndex = (fboIndex + 1) % NUM_FRAMES;
}

//--------------------------------------------------------------
void ofApp::draw(){
  if (viewMode == 0){
    cam.begin();
    ofPushMatrix();
      ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
      for (int i =0; i < MAX_CONTACTS; i++ ){ellipse[i].drawCursor(mode);} 
      mesh.drawWireframe();
    ofPopMatrix();
    cam.end();
  } else if (viewMode == 1){
    ofEnableLighting();
    cam.begin();
    light.enable();
    ofPushMatrix();
      ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
      mesh.draw();  
    ofPopMatrix();
    light.disable();
    cam.end();

    ofDisableLighting();
  }
  if (viewMode == 2){
    ofSetColor(ofColor::white);
    fbo[fboIndex].draw(0,0);
  }
  if (!hideMenu) drawUi();
}

void ofApp::exit(){
  sensel.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'c'){
    for (int i = 0; i < NUM_FRAMES; i++){
      fbo[i].begin();
      ofClear(0,0,0,1);
      fbo[i].end();
    }
    updateMesh(&mesh);
  }
  if (key == 'v'){
    viewMode = (viewMode + 1) % maxViewModes;
  } 
  if (key == 'm'){
    mode = (mode + 1) % maxModes;
  }
  if (key == 'h'){
    hideMenu = !hideMenu;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}
