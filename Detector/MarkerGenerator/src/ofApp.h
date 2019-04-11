#pragma once

#include "ofMain.h"

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

#define CUSTOM_DIC 50

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofImage createMarker(int dicId, int id, int bBits);
		ofImage createCustomMarker(int id);

private:
	int mMakerId;
	int mBorderBits;
	int mDictionaryId;

	bool mGenerateTags;

	std::vector<std::string> mDictionaryTags;
	std::vector<int> mDictionaryMax;

	ofImage mVidImg;
};
