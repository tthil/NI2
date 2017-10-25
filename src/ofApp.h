#pragma once

#include <OpenNI.h>
#include "ofMain.h"
//#include "ofxOpenCv.h"

using namespace openni;
using namespace std;
//using namespace cv;

class ofApp : public ofBaseApp
{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// ------------- OpenCV --------
	/*	
	ofVideoGrabber vidGrabber;
	ofVideoPlayer vidPlayer;
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage grayDiff;
	ofxCvContourFinder contourFinder;

	int threshold;
	bool bLearnBakground;
*/
	// -------------- OpenNi -----------

	int window_w = 640;
	int window_h = 480;
	OniRGB888Pixel *gl_texture;
	VideoStream depthSensor;
	Device device;
	bool histogram_enable = false;
	bool color_enable = false;
	bool blackfill_enable = false;
};

/* 
SENSOR_COLOR
a. 320x240 at 30fps with 200 format
b. 320x240 at 30fps with 201 format
c. 320x240 at 30fps with 205 format
d. 320x240 at 15fps with 200 format
e. 320x240 at 15fps with 201 format
f. 320x240 at 15fps with 205 format
g. 320x240 at 60fps with 200 format
h. 320x240 at 60fps with 201 format
i. 320x240 at 60fps with 205 format
j. 640x480 at 30fps with 200 format
k. 640x480 at 30fps with 201 format
l. 640x480 at 30fps with 205 format
m. 640x480 at 15fps with 200 format
n. 640x480 at 15fps with 201 format
o. 640x480 at 15fps with 205 format
p. 1280x1024 at 30fps with 200 format
q. 1280x1024 at 30fps with 202 format
r. 1280x1024 at 30fps with 201 format
s. 1280x1024 at 30fps with 205 format
t. 1280x720 at 30fps with 200 format
u. 1280x720 at 30fps with 202 format
v. 1280x960 at 30fps with 200 format
w. 1280x960 at 30fps with 202 format

SENSOR_IR
a. 320x240 at 30fps with 203 format
b. 320x240 at 30fps with 200 format
c. 320x240 at 60fps with 203 format
d. 320x240 at 60fps with 200 format
e. 640x480 at 30fps with 203 format
f. 640x480 at 30fps with 200 format
g. 1280x1024 at 30fps with 203 format
h. 1280x1024 at 30fps with 200 format

SENSOR_DEPTH
a. 320x240 at 30fps with 100 format
b. 320x240 at 30fps with 101 format
c. 320x240 at 60fps with 100 format
d. 320x240 at 60fps with 101 format
e. 640x480 at 30fps with 100 format
f. 640x480 at 30fps with 101 format
g. 1280x1024 at 30fps with 100 format
h. 1280x1024 at 30fps with 101 format
i. 160x120 at 30fps with 100 format
j. 160x120 at 30fps with 101 format
*/