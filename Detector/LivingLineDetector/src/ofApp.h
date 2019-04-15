/*

Thomas Sanchez Lengeling
March, 2019

Living Line

*/

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// addons
#include "ofxCv.h"
#include "ofxDatGui.h"
#include "ofxOpenCv.h"

#include "CommonTypes.h"
#include "QRDetector.h"
#include "MarkerAruco.h"
#include "gui.h"
#include "GridImage.h"
#include "GridDetector.h"
#include "QRBlock.h"

#include "jsonclass.h"

#define NUM_CAM_INPUTS 1

#define CAM_WIDTH  1920  //1920
#define CAM_HEIGHT 1080 //1080

#define CAM_FRAMERATE 30 //10

class ofApp : public ofBaseApp {

public:
  void setup();
  void update();
  void draw();
  void exit();

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

  // system values
  bool mDebug;

  //GUI functions
  void setupGUI();
  void updateGUI();
  void drawGUI();

  void setupValues();

  // Calibrate
  void setupCalibration();
  void setupDetection();

  void updateGrid();
  void recordGrid();

  // clean Detection
  void cleanDetection();

  //draw grid info
  void offScreenRenderGrid();

  //create network communication
  void setupConnection();

  // save JSON file
  void saveJSONBlocks();

  //draw info on screen
  void drawInfoScreen();


  //setup cameras for input detectors
  void setupCam();

  //setup markers detector
  void   setupGridDetector();

  // GUI
  bool mDrawGUI;
  ofxDatButtonRef mBDebugVideo;
  ofxDatButtonRef mBDebugVideoGrid;
  ofxDatButtonRef mBDebugGrid;
  ofxDatButtonRef mBSingleGrid;
  ofxDatButtonRef mBFullGrid;
  ofxDatButtonRef mBFullCamView;

  //cam calibration process
  ofxDatButtonRef mCamCalibration;
  ofxDatButtonRef mCamPerspective;

  ofxDatButtonRef mBEnableCrop;
  ofxDatButtonRef mBCalibrateGrid;
  ofxDatButtonRef mBEnableVideo;
  ofxDatButtonRef mBDebugMarkers;

  ofxDatSliderRef mGammaValue;
  ofxDatSliderRef mAlphaValue;
  ofxDatSliderRef mBetaValue;

  ofxDatMatrixRef mBGridSelect;

  // 4 camera render
  ofFbo mFboSingle;
  std::vector<ofFbo> mFboGrid;
  ofFbo mFboFullGrid;

  bool mSingleCam;

  //grid image
  std::vector<GridImageRef> mGridImg;
  int mCurrentInputIdx;

  ofTexture mCurrentVideo;

  // aruco etector
  std::vector<QRDetectorRef> mArucoDetector;
  int mTotalMarkers;
  bool mRefimentDetector;

  ofImage mInputDetectImg;

  //inputs
  int mNumInputs;

  glm::vec2 mFullGridDim;
  int mTotalMaxMarkers;
  std::vector<glm::vec2> mGridSizes;
  std::vector<int> mMaxMarkers;

  //grid detector
  std::vector<GridDetectorRef> mGridDetector;
  bool mSortMarkers;

  // send commands
  ofxUDPManager mUDPConnectionTable;
  std::string   mUDPIp;
  int           mUDPPort;

  //network for Radar communication
  ofxUDPManager mUDPConnectionRadar;
  std::string   mUDPRadarIp;
  int           mUDPRadarPort;

};
