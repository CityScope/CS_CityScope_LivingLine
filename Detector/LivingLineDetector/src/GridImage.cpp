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

  mCornerUp = glm::vec2(100, 100);
  mCornerDown = glm::vec2(300, 300);
  mDisp = glm::vec2(23, 23);

  mFboResolution.allocate(mDim.x, mDim.y, GL_RGBA);
  mPixs.allocate(mDim.x, mDim.y, OF_PIXELS_RGBA);

  // clean start with Fbos
  mFboResolution.begin();
  ofClear(0, 0, 0, 255);
  mFboResolution.end();
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
  mCam.setDeviceID(mCamId);
  mCam.setDesiredFrameRate(mFps);
  mCam.setUseTexture(true);
  mCam.setup(mDim.x, mDim.y);

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
void GridImage::updateCorners(){

  //update corners
  //mInputQuad[0]
  //mInputQuad[1]
  //mInputQuad[2]
  //mInputQuad[3]
}

//-----------------------------------------------------------------------------
void GridImage::calculatePerspective(){

  //mCropMat
  // Input Quadilateral or Image plane coordinates
   Point2f inputQuad[4];

   inputQuad[0] = Point2f(0, 0);
   inputQuad[1] = Point2f(0, 0);
   inputQuad[2] = Point2f(0, 0);
   inputQuad[3] = Point2f(0, 0);

   // Output Quadilateral or World plane coordinates
   Point2f outputQuad[4];
   outputQuad[0] = Point2f(0, 0);
   outputQuad[1] = Point2f(0, 0);
   outputQuad[2] = Point2f(0, 0);
   outputQuad[3] = Point2f(0, 0);

   // Lambda Matrix
   Mat lambda;
   lambda = getPerspectiveTransform( inputQuad, outputQuad );

   // Apply the Perspective Transform just found to the src image
   warpPerspective(mCropMat, mPerspectiveMat, lambda, mPerspectiveMat.size());

   //
}
