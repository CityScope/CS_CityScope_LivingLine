#include "GridImage.h"

using namespace std;
using namespace cv;


//----------------------------------------------------------------------------
GridImage::~GridImage(){
  mCam.close();
  cout<<"cam closed "<<mId<<std::endl;
}

//----------------------------------------------------------------------------
GridImage::GridImage(glm::vec2 dims) {
  mDim = dims;
  mLength = glm::vec2(0, 0);
  mGamma = 0.65;
  mActivateCrop = false;
  mActivateCam = true;
  mFps = 30;

  mCamLoaded = false;

  mCamCounter  = 0;
  mCamThread   = 0;

  mCornerUp = glm::vec2(100, 100);
  mCornerDown = glm::vec2(300, 300);
  mDisp = glm::vec2(23, 23);

  mFboResolution.allocate(mDim.x, mDim.y, GL_RGBA);
  mPixs.allocate(mDim.x, mDim.y, OF_PIXELS_RGBA);

  // clean start with Fbos
  mFboResolution.begin();
  ofClear(0, 0, 0, 255);
  mFboResolution.end();

  //Perspective
  cornerIndex= 0;
  mCalculatedPersp = false;
  mInputQuad[0] = cv::Point2f(20, 20);
  mInputQuad[1] = cv::Point2f(700, 20);
  mInputQuad[2] = cv::Point2f(700, 800);
  mInputQuad[3] = cv::Point2f(20, 800);
}

void GridImage::threadTimer(int counter) {

  while(counter <= 1000){
    counter++;
  }
}

//-----------------------------------------------------------------------------
void GridImage::setupCam(int id, int fps) {
  mCamId = id;
  mFps = fps;

  ofLog(OF_LOG_NOTICE) << "Loading Cam: " << mCamId << " " << mId << " "
                       << mDim.x << ", " << mDim.y << "  can: " << mCam.getWidth()
                       << ", " << mCam.getHeight()<<" fps: "<<mFps<<std::endl;

  //mCam.setVerbose(true);
  //mCam.listDevices();
 if(!mCamLoaded){
   ofLog(OF_LOG_NOTICE) << "Starting to open and close Cam: " << mCamId<<std::endl;
    mCam.close();

    mCam.setDeviceID(mCamId);
    mCam.setDesiredFrameRate(mFps);
    mCam.setUseTexture(true);

    ofLog(OF_LOG_NOTICE) << "Cam:"<<std::endl;

    ofSleepMillis(1000);
    bool camSetup = mCam.initGrabber(mDim.x, mDim.y);
    if(camSetup == true){
      ofLog(OF_LOG_NOTICE) << "Cam opened: " << mCamId<<" "<<mCamCounter<<std::endl;
      mCamLoaded = true;
    }
  }

  ofLog(OF_LOG_NOTICE) << "loaded Cam: " << mCamId <<
  " Cam: "<< mCam.getWidth()<< ", " << mCam.getHeight()<<std::endl;
}

//-----------------------------------------------------------------------------
bool GridImage::updateImage() {
  bool newFrame = false;

  mCam.update();
  newFrame = mCam.isFrameNew();
  if (newFrame) {
      // mFboResolution.begin();
      // mCam.draw(0, 0, mDim.x, mDim.y);
      // mFboResolution.end();
      // ofLog(OF_LOG_NOTICE) << " " << newFrame << " ";
  }
  return newFrame;
}

//-----------------------------------------------------------------------------
ofPixels &GridImage::getImgPixels() {
  return mCam.getPixels();
}

//-----------------------------------------------------------------------------
void GridImage::setCropUp(glm::vec2 up) {
  mCornerUp = up;
}

//-----------------------------------------------------------------------------
void GridImage::setCropDown(glm::vec2 down) {
  mCornerDown = down;
}

//-----------------------------------------------------------------------------
void GridImage::drawImage(int x, int y, int w, int h) {
  mCam.draw(x, y, w, h);
}

//-----------------------------------------------------------------------------
void GridImage::drawImage(int x, int y) {
  mCam.draw(x, y, mDim.x, mDim.y);
}

//-----------------------------------------------------------------------------
void GridImage::adjustGamma(cv::Mat &img) {
  if (!img.empty()) {

    cv::Mat imgGamma;
    img.convertTo(imgGamma, -1, mAlpha, mBeta);

    cv::Mat lookUpTable(1, 256, CV_8U);
    unsigned char *p = lookUpTable.ptr();
    for (int i = 0; i < 256; i++) {
      p[i] = saturate_cast<unsigned char>(pow(i / 255.0, mGamma) * 255.0);
    }
    cv::LUT(imgGamma, lookUpTable, imgGamma);

    imgGamma.copyTo(img);
  }


}

//-----------------------------------------------------------------------------
void GridImage::resetCrop() {

  mCornerUp = glm::vec2(100, 100);
  mCornerDown = glm::vec2(300, 300);
  mDisp = glm::vec2(5, 5);

}

//-----------------------------------------------------------------------------
void GridImage::cropImg(cv::Mat &inputVideo) {

  //update sizes
  mLength.x = mCornerDown.x - mCornerUp.x;
  mLength.y = mCornerDown.y - mCornerUp.y;
  mRoi.x = mCornerUp.x;
  mRoi.y = mCornerUp.y;
  mRoi.width = mLength.x + mDisp.x;
  mRoi.height = mLength.y + mDisp.y;

  // Copy the data into new matrix
  if (mRoi.width < mDim.x && mRoi.height < mDim.y) {
    if (mRoi.x > 0 && mRoi.y > 0 && mRoi.width < mDim.x &&
        mRoi.height < mDim.y) {
      cv::Mat cutMat(inputVideo, mRoi);
      cutMat.copyTo(mCropMat);
    } else {
      ofLog(OF_LOG_NOTICE) << "error crop width";
    }
  } else {
    inputVideo.copyTo(mCropMat);
    ofLog(OF_LOG_NOTICE) << "error roi dimentions crop";
  }

}

//-----------------------------------------------------------------------------
void GridImage::drawCropRoi() {
  if (mActivateCrop) {
    ofSetColor(0, 200, 125, 50);
  } else {
    ofSetColor(0, 220, 195, 155);
  }

  ofBeginShape();
  ofVertex(mCornerUp);
  ofVertex(glm::vec2(mCornerDown.x, mCornerUp.y));
  ofVertex(mCornerDown);
  ofVertex(glm::vec2(mCornerUp.x, mCornerDown.y));
  ofEndShape();

  ofSetColor(255, 50, 50, 155);
  ofDrawCircle(mCornerDown.x, mCornerDown.y, 5, 5);
  ofDrawCircle(mCornerUp.x, mCornerUp.y, 5, 5);
}

//-----------------------------------------------------------------------------
void GridImage::drawCropImg() {
  ofImage imgCut;
  ofxCv::toOf(mCropMat, imgCut.getPixels());
  imgCut.update();

  ofSetColor(200, 200);
  imgCut.draw(mCornerUp.x, mCornerUp.y, mLength.x + mDisp.x, mLength.y + mDisp.y);

  ofxCv::drawMat(mCropMat, 0, 200, 200, 200);
  imgCut.draw(0, 400, 200, 200);
}

//-----------------------------------------------------------------------------
void GridImage::updateCorners(glm::vec2 corner){

  //update corners Input

  //Input Quadilateral or Image plane coordinates
  mInputQuad[cornerIndex] = cv::Point2f(corner.x, corner.y);
  ofLog(OF_LOG_NOTICE)<<"added point "<<cornerIndex<<" "<<corner.x<<", "<<corner.y<<std::endl;

  cornerIndex++;
  if(cornerIndex>=4){
    cornerIndex =0;
    ofLog(OF_LOG_NOTICE) << "new calculated perspective done";
    mCalculatedPersp = true;
  }

}
//-----------------------------------------------------------------------------
void GridImage::calculatePerspective(cv::Mat &inputVideo){
    //mCropMat

   // Output Quadilateral or World plane coordinates
   ofPoint tl(mInputQuad[0].x, mInputQuad[0].y);
   ofPoint tr(mInputQuad[1].x, mInputQuad[1].y);
   ofPoint br(mInputQuad[2].x, mInputQuad[2].y);
   ofPoint bl(mInputQuad[3].x, mInputQuad[3].y);

   float widthA = sqrt( pow(br.x - bl.x, 2) +  pow(br.y - bl.y, 2) );
   float widthB = sqrt( pow(tr.x - tl.x, 2) +  pow(tr.y - tl.y, 2) );
   float maxWidth = glm::max(widthA, widthB);

   float heightA = sqrt( pow(tr.x - br.x, 2) +  pow(tr.y - br.y, 2) );
   float heightB = sqrt( pow(tl.x - bl.x, 2) +  pow(tl.y - bl.y, 2) );
   float maxHeight = glm::max(heightA, heightB);

   cv::Point2f outputQuad[4];
   outputQuad[0] =  cv::Point2f(0, 0);
   outputQuad[1] =  cv::Point2f(maxWidth - 1, 0);
   outputQuad[2] =  cv::Point2f(maxWidth - 1, maxHeight -1);
   outputQuad[3] =  cv::Point2f(0, maxHeight -1);

   // Lambda Matrix
   Mat lambda = getPerspectiveTransform( mInputQuad, outputQuad );

   // Apply the Perspective Transform just found to the src image
   warpPerspective(inputVideo, mPerspectiveMat, lambda, mPerspectiveMat.size());
}
