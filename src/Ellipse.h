#include "ofMain.h"

class Ellipse{
public:
  Ellipse();
  void deactivate();
  void set(int, int, int, int, int, float, ofColor);
  void draw();
  void drawCursor(int);

  bool active;
  float x, y, width, height, rotation, f;
  ofColor color;
};