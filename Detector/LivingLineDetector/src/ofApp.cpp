#include "ofApp.h"
#include "setup.h"

using namespace std;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {

  // DEBUG
  mDebug = false;
  ofLog(OF_LOG_NOTICE) << "Debuging: " << mDebug << std::endl;

  if(mDebug){
    ofSetLogLevel(OF_LOG_VERBOSE);
  }

  ofSetBackgroundAuto(false);
  ofBackground(0);

  //setup values
  setupValues();
  setupGridDetector();
  setupDetection();
  setupCalibration();
  setupCam(); //dont load the cameras at the begining
  setupConnection();
  setupGUI();

  ofLog(OF_LOG_NOTICE) << "finished setup";
}

//---------------------------------------------------------------------------
void ofApp::cleanDetection() {

  // calculate probabilyt and clean nois
  if (mBSingleGrid->isActive()) {
    mGridDetector.at(mCurrentInputIdx)->cleanGrid();

    mGridDetector.at(mCurrentInputIdx)->resetCleaner();
  } else if (mBFullGrid->isActive()) {

    for (auto &gridDetector : mGridDetector) {
      gridDetector->cleanGrid();
    }

    for (auto &gridDetector : mGridDetector) {
        gridDetector->resetCleaner();
    }
  }
}

//--------------------------------------------------------------
void ofApp::update() {

  bool newFrame = false;
  int currentId = mCurrentInputIdx;

  std::vector<cv::Mat> vidMatImgs;
  std::vector<ofPixels> pixelsImg;
  std::vector<cv::Mat> imageCopys;
  std::vector<bool> newFrames;
  std::vector<std::string> UDPStrings;
  cv::Mat imageCopy;

  int j = 0;
  for (auto &gridImage : mGridImg) {
    newFrame = gridImage->updateImage();
    newFrames.push_back(newFrame);
    pixelsImg.push_back(gridImage->getImgPixels());
    j++;
  }

  //
  if (mBSingleGrid->isActive()) {
    cv::Mat copyCrop;
    ofPixels pixs = pixelsImg.at(currentId);
    if (pixs.getHeight() > 0) {
      cv::Mat input = ofxCv::toCv(pixs);;//.clone();

      //crop
      if( !mCamPerspective->isActive() ){
        mGridImg.at(currentId)->cropImg(input);
        cv::Mat copMat = mGridImg.at(currentId)->getCropMat();
        copMat.copyTo(copyCrop);
      }else{
        //perspective transformation
        if(mGridImg.at(currentId)->isCalculatedPersp()){
          mGridImg.at(currentId)->calculatePerspective(input);
        }
        cv::Mat copMat = mGridImg.at(currentId)->getPersMat();
        copMat.copyTo(copyCrop);
      }

      //update mapping values
      if(mUpdateCoordinate->isActive()){
        float minX = mMapMinX->getValue();
        float maxX = mMapMaxX->getValue();
        float minY = mMapMinY->getValue();
        float maxY = mMapMaxY->getValue();
        //save coordinate system
        mGridDetector.at(mCurrentInputIdx)->setMapCoord(minX, maxX, minY, maxY);
      }

      //color correction
      mGridImg.at(currentId)->setGamma(mGammaValue->getValue());
      mGridImg.at(currentId)->setAlpha(mAlphaValue->getValue());
      mGridImg.at(currentId)->setBeta(mBetaValue->getValue());

      // calculate Gamma
      mGridImg.at(currentId)->adjustGamma(copyCrop);
      copyCrop.copyTo(imageCopy);
    } else {
      if(mDebugMode->isActive()) ofLog(OF_LOG_NOTICE) << "error size ";
    }

  } else if (mBFullGrid->isActive()) {
    int i = 0;
    for (auto &pixs : pixelsImg) {
      cv::Mat copyCrop;
      if (pixs.getHeight() > 0) {
        cv::Mat input = ofxCv::toCv(pixs);

        //crop img
        if( !mCamPerspective->isActive() ){
          mGridImg.at(currentId)->cropImg(input);
          cv::Mat copMat = mGridImg.at(i)->getCropMat();
          copMat.copyTo(copyCrop);
        }else{
          //perspective transformation
          if(mGridImg.at(i)->isCalculatedPersp()){
            mGridImg.at(i)->calculatePerspective(input);
          }
          cv::Mat copMat = mGridImg.at(i)->getPersMat();
          copMat.copyTo(copyCrop);
        }

        mGridImg.at(i)->adjustGamma(copyCrop);
        imageCopys.push_back(copyCrop);
      } else {
        if(mDebugMode->isActive()) ofLog(OF_LOG_NOTICE) << "error size: " << i;
      }
      i++;
    }
  }

  mTotalMarkers = 0;
  if (mBSingleGrid->isActive()) {
    // && newFrames[mCurrentInputIdx]
    if (!imageCopy.empty()) {

      // detect the markers
      mArucoDetector.at(currentId)->detectMarkers(imageCopy, mRefimentDetector);

      // calculate the number of total markers
      mTotalMarkers += mArucoDetector.at(currentId)->getNumMarkers();

      if(mViewCams->isActive()){
        // get the marker image output
        mInputDetectImg = mArucoDetector.at(currentId)->getOfImg();
        //vidMat = mArucoDetector.at(currentId)->getMatImg();
        mGridImg.at(currentId)->updateDetectImg(mInputDetectImg);
      }

      // save the positions and id from the detected markers.
      mGridDetector.at(currentId)->generateMarkers(mArucoDetector.at(currentId)->getTagIds(), mArucoDetector.at(currentId)->getBoard());

      //update error check
      mGridDetector.at(currentId)->updateCleaner();
    } else {
      if(mDebugMode->isActive()) ofLog(OF_LOG_NOTICE) << "empty mat img copy: " << currentId;
    }
  } else if (mBFullGrid->isActive()) {
    if (!imageCopys.empty()) {

      int i = 0;
      for (auto &matcopy : imageCopys) {

        // detect the markers
        mArucoDetector.at(i)->detectMarkers(matcopy, mRefimentDetector);

        // calculate the number of total markers
        mTotalMarkers += mArucoDetector.at(i)->getNumMarkers();

        if(mViewCams->isActive()){
          // get the marker image output
          mInputDetectImg = mArucoDetector.at(i)->getOfImg();
          //vidMat = mArucoDetector.at(i)->getMatImg();
          mGridImg.at(i)->updateDetectImg(mInputDetectImg);
        }

        // save the positions and id from the detected markers.
        mGridDetector.at(i)->generateMarkers(mArucoDetector.at(i)->getTagIds(), mArucoDetector.at(i)->getBoard());

        //update errors
        mGridDetector.at(i)->updateCleaner();
        i++;
      }
    } else {
      if(mDebugMode->isActive()) ofLog(OF_LOG_NOTICE) << "empty mat img copy ";
    }
  }

  cleanDetection();

  offScreenRenderGrid();

  // udate
  if (mDrawGUI) {
    updateGUI();
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetColor(0, 0, 0, 255);
  ofRect(0, 0, ofGetWidth(), ofGetHeight());

  //draw video streaming information
  if (mBDebugVideo->isActive()) {
    ofSetColor(255);
    mInputDetectImg.draw(0, 0, 1280, 720);

    if (mBEnableCrop->isActive()) {
      mGridImg.at(mCurrentInputIdx)->drawImage(0, 0);
    } else {

      int i = 0;
      for (auto &gridImage : mGridImg) {
        if (mCurrentInputIdx == i) {
          ofSetColor(0, 200, 150, 200);
        } else {
          ofSetColor(255, 150);
        }
        gridImage->drawImage(ofGetWidth() - 426, 240 * i, 426, 240);
        i++;
      }
    }
  }

  //draw video steam as a grid
  if (mBDebugVideoGrid->isActive()) {

    if (mBSingleGrid->isActive()) {
      int id = mCurrentInputIdx;
      float sqsize = 36;
      float sqspace = 5;

      glm::vec2 dim = mGridDetector.at(id)->getDim();
      int spaceX = dim.x * (sqsize + sqspace);
      int spaceY = dim.y * (sqsize + sqspace);

      mGridDetector.at(id)->drawDetectedGridIn(10, 280, sqsize, sqspace);
      mGridDetector.at(id)->drawDetectedGrid(spaceX + 50, 50, sqsize, sqspace);

      mGridImg.at(id)->getImg().draw(spaceX + 50, spaceY + 100, 640, 360);
    } else if (mBFullGrid->isActive()) {
      int i = 0;
      int j = 0;
      int k = 0;
      glm::vec2 jump(0, 0);
      float sqsize = 35;
      float sqspace = 2;
      for (auto gridDetector : mGridDetector) {
        if (k > 0) {
          glm::vec2 dim = mGridDetector.at(k - 1)->getDim();
          jump = glm::vec2(dim.x * (sqsize + sqspace) * i, dim.y * (sqsize + sqspace) * j);
        }

        float posx = i * (jump.x) + i * 50 + 20;
        float posy = j * (jump.y) + j * 50 + 20;

        gridDetector->drawDetectedGridIn(posx, posy, sqsize, sqspace);

        i++;
        k++;
        if (i >= 2) {
          j++;
          i = 0;
        }

      }
    }
  }

//-------------------------------------------------------------
  if (mBDebugGrid->isActive()) {

    //draw full grid or a single cma view
    ofSetColor(255);
    if (mBSingleGrid->isActive()) {
      mFboSingle.draw(0, 0);
    } else if (mBFullGrid->isActive()) {
      int i = 0;
      int j = 0;
      float w = ofGetWidth() / 2.0;
      float h = ofGetHeight() / 2.0;
      for (auto &fbo : mFboGrid) {
        fbo.draw(i * w, j * h, w, h);
        i++;
        if (i >= 2) {
          j++;
          i = 0;
        }
      }
    }

    //draw the crop img single or full
    if (mBEnableCrop->isActive()) {
      mGridImg.at(mCurrentInputIdx)->drawImage(0, 0, ofGetWidth(), ofGetHeight());
    } else {
      int i = 0;
      for (auto &gridImage : mGridImg) {
        if (mCurrentInputIdx == i) {
          ofSetColor(0, 200, 150, 200);
        } else {
          ofSetColor(255, 150);
        }
        gridImage->drawImage(ofGetWidth() - 426, 240 * i, 426, 240);
        i++;
      }
    }
  }

  // draw full input view
  // 3 cam 3 views
  if (mBFullCamView->isActive()) {
    int i = 0;
    ofSetColor(255, 255);
    for (auto &gridImage : mGridImg) {
      gridImage->drawImage(640 * i + 100, 360 + 200, 640, 360);
      i++;
    }
  }

  // draw GUI
  if (mDrawGUI) {
    ofSetColor(255);
    drawGUI();
  }

  //draw input cams
  if (mBEnableCrop->isActive()) {
    mGridImg.at(mCurrentInputIdx)->drawCropImg();
    mGridImg.at(mCurrentInputIdx)->drawCropRoi();
  }

  //send json
  if(mSendUDP->isActive()){
    sendUDPJson();
  }

  if(mActivePerspectivePoints){
    ofSetColor(0, 200, 255, 150);
    ofDrawCircle(mMousePos.x, mMousePos.y, 20);

    ofSetColor(0, 220, 225, 150);
    ofDrawCircle(mCurretPerspInc.x, mCurretPerspInc.y, 15);
  }

  // draw results
  drawInfoScreen();
}

//----------------------------------------------------------------------------
void ofApp::drawInfoScreen() {
  int maxM = mGridDetector.at(mCurrentInputIdx)->getMaxMarkers();
  int numM = mGridDetector.at(mCurrentInputIdx)->getNumMarkers();
  glm::vec2 cdim = mGridDetector.at(mCurrentInputIdx)->getDim();
  ofSetColor(255);
  int posx = ofGetWidth() - 230;

  ofDrawBitmapString("Fps: " + to_string(ofGetFrameRate()), posx, 20);
  ofDrawBitmapString("Total Dec: " + to_string(mTotalMarkers), posx, 40);
  ofDrawBitmapString("Max Dec: " + to_string(mTotalMaxMarkers), posx, 60);
  ofDrawBitmapString("Inputs: " + to_string(mGridImg.size()), posx, 90);
  ofDrawBitmapString("Current input: " + to_string(mCurrentInputIdx), posx, 110);
  ofDrawBitmapString("Max Markers Real: " + to_string(maxM), posx, 130);
  ofDrawBitmapString("Markers: " + to_string(numM), posx, 150);
  ofDrawBitmapString("Dim: " + to_string(int(cdim.x)) + " " + to_string(int(cdim.y)), posx, 170);
  ofDrawBitmapString("Max Markers: " + to_string(int(cdim.x * cdim.y)), posx, 190);
  ofDrawBitmapString("Full Dim: " + to_string(int(mFullGridDim.x)) + " " + to_string(int(mFullGridDim.y)), posx, 210);
  ofDrawBitmapString("UDP IP: " + mUDPIp, posx, 250);
  ofDrawBitmapString("UDP Port: " + to_string(mUDPPort), posx, 270);
  ofDrawBitmapString("Persp: "+to_string(mPerspectiveIndex)+" - "+to_string(mCurretPerspInc.x)+" "+to_string(mCurretPerspInc.y), posx, 290);
}

//---------------------------------------------------------------------------
void ofApp::offScreenRenderGrid() {
  if (mBSingleGrid->isActive()) {
    mFboSingle.begin();
    mInputDetectImg.draw(0, 0);
    mGridDetector.at(mCurrentInputIdx)->drawMarkers();
    mFboSingle.end();
  }

  else if (mBFullGrid->isActive()) {
    int i = 0;
    for (auto &fbo : mFboGrid) {
      mFboFullGrid.begin();
      mGridImg.at(i)->getImg().draw(0, 0);
      mGridDetector.at(i)->drawMarkers();
      mFboFullGrid.end();
      i++;
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateGUI() {
  mGammaValue->update();
  mAlphaValue->update();
  mBetaValue->update();

  mBEnableCrop->update();
  mBEnableCams->update();
  mBDebugVideoGrid->update();
  mBFullCamView->update();

  mCamCalibration->update();
  mCamPerspective->update();
  mSendUDP->update();
  mDebugMode->update();

  mBDebugVideo->update();
  mBDebugGrid->update();
  mBSingleGrid->update();
  mBFullGrid->update();
  mBDebugMarkers->update();
  mBDebugVideoGrid->update();

  mBGridSelect->update();
  mViewCams->update();

  mMapMinX->update();

  mMapMinX->update();
  mMapMaxX->update();
  mMapMinY->update();
  mMapMaxY->update();

  mUpdateCoordinate->update();
}

//--------------------------------------------------------------
void ofApp::drawGUI() {
  mGammaValue->draw();
  mAlphaValue->draw();
  mBetaValue->draw();

  mBEnableCrop->draw();
  mBEnableCams->draw();
  mBFullCamView->draw();

  mCamCalibration->draw();
  mCamPerspective->draw();
  mSendUDP->draw();
  mDebugMode->draw();

  mBDebugVideo->draw();
  mBDebugGrid->draw();
  mBSingleGrid->draw();
  mBFullGrid->draw();
  mBDebugMarkers->draw();
  mBDebugVideoGrid->draw();

  mBGridSelect->draw();
  mViewCams->draw();

  mMapMinX->draw();
  mMapMaxX->draw();
  mMapMinY->draw();
  mMapMaxY->draw();

  mUpdateCoordinate->draw();
}

//--------------------------------------------------------------
void ofApp::saveJSONBlocks() {

  int i = 0;
  /*
  if (mEnableKnob) {
      ofJson pt;
      pt[to_string(i)]["posx"] = mKnobAmenitie->getStaticPos().x;
      pt[to_string(i)]["posy"] = mKnobAmenitie->getStaticPos().y;
      pt[to_string(i)]["type"] = BlockType::knobStatic;
      // writer.push_back(pt);
      i++;

  }
  */
}

//--------------------------------------------------------------
void ofApp::sendUDPJson(){
  ofJson writer;

  {

    ofJson jsonFixed;
    std::string inputStr("fixed_units");

    //fill the fixed units with the real values
    std::vector<ofJson> fixed;
    for (auto &gridDetector : mGridDetector) {
      auto markers = gridDetector->getCompiledMarkers();
      for(auto & mb : markers){
        ofJson json;
        int id =mb->getMarkerId();
        //not fixed or knobs
        if(id == 3){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 0;
          fixed.push_back(json);
        }else if(id == 10){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 1;
          fixed.push_back(json);
        }else if(id  == 13){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 6;
          fixed.push_back(json);
        }else if(id  == 17){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 3;
          fixed.push_back(json);
        }else if(id  == 22){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 2;
          fixed.push_back(json);
        }else if(id  == 30){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 5;
        fixed.push_back(json);
        }else if(id  == 34){
          json["x"] = round(mb->getPos().x);
          json["y"] = round(mb->getPos().y);
          json["type"] = 4;
          fixed.push_back(json);
        }
      }
    }
    writer[inputStr] = fixed;
    //writer.push_back(jsonFixed);
  }

  {
    ofJson jsonFree;
    std::string inputStr("free_units");
    std::vector<ofJson> free;
    for (auto &gridDetector : mGridDetector) {
      auto markers = gridDetector->getCompiledMarkers();
      for(auto & mb : markers){
        ofJson json;
        int id =mb->getMarkerId();
        if( id == 40){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 7;
          free.push_back(json);
        }else if(id == 41){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 8;
          free.push_back(json);
        }else if(id == 49){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 9;
          free.push_back(json);
        }
      }
    }
    writer[inputStr] = free;
    //writer.push_back(jsonFree);
  }

  {
    ofJson jsonKnob;
    std::string inputStr("knobs");
    std::vector<ofJson> knob;
    for (auto &gridDetector : mGridDetector) {
      auto markers = gridDetector->getCompiledMarkers();
      for(auto & mb : markers){
        ofJson json;
        int id =mb->getMarkerId();
        if( id == 37){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 10;
          knob.push_back(json);
        }else if(id == 38){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 11;
          knob.push_back(json);
        }else if(id == 45){
          json["x"]    = round(mb->getPos().x);
          json["y"]    = round(mb->getPos().y);
          json["rot"]  = mb->getRot();
          json["type"] = 12;
          knob.push_back(json);
        }
      }
    }
    writer[inputStr] = knob;
    //writer.push_back(jsonKnob);
  }
  std::string udpjson = writer.dump();

  if(mDebug) ofLog(OF_LOG_NOTICE) << udpjson<<std::endl;

  //ofLog(OF_LOG_NOTICE) << "Set UDP Test";
  mUDPConnectionTable.Send(udpjson.c_str(), udpjson.length());
  //ofLog(OF_LOG_NOTICE) << udpjson<<std::endl;
}

float ofApp::round(float var){
    float value = (int)(var * 100 + 0.5);
    return (float)value / 100;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == 'g') {
    mDrawGUI = !mDrawGUI;
    ofLog(OF_LOG_NOTICE) << "Draw GUI " << mDrawGUI;
  }

  if (key == '1') {
    if( mCamPerspective->isActive() && mGridImg.at(mCurrentInputIdx)->isCalculatedPersp() ){
      mPerspectiveIndex++;
      if(mPerspectiveIndex>=4){
        mPerspectiveIndex =0;
      }
      ofLog(OF_LOG_NOTICE) << "Perspective Index " << mPerspectiveIndex;
    }
  }

  if(key == OF_KEY_UP){
    if( mCamPerspective->isActive() && mGridImg.at(mCurrentInputIdx)->isCalculatedPersp() ){
      mPerspInc = glm::vec2(0.0, 0.2);
      mGridImg.at(mCurrentInputIdx)->addInputPersp(mPerspInc, mPerspectiveIndex);
      mCurretPerspInc = mGridImg.at(mCurrentInputIdx)->getInputPersp(mPerspectiveIndex);
    }
  }
  if(key == OF_KEY_DOWN){
    if( mCamPerspective->isActive() && mGridImg.at(mCurrentInputIdx)->isCalculatedPersp() ){
      mPerspInc = glm::vec2(0.0, -0.2);
      mGridImg.at(mCurrentInputIdx)->addInputPersp(mPerspInc, mPerspectiveIndex);
      mCurretPerspInc = mGridImg.at(mCurrentInputIdx)->getInputPersp(mPerspectiveIndex);
    }
  }
  if(key == OF_KEY_RIGHT){
    if( mCamPerspective->isActive() && mGridImg.at(mCurrentInputIdx)->isCalculatedPersp() ){
      mPerspInc = glm::vec2(0.2, 0.0);
      mGridImg.at(mCurrentInputIdx)->addInputPersp(mPerspInc, mPerspectiveIndex);
      mCurretPerspInc = mGridImg.at(mCurrentInputIdx)->getInputPersp(mPerspectiveIndex);
    }
  }
  if(key == OF_KEY_LEFT){
    if( mCamPerspective->isActive() && mGridImg.at(mCurrentInputIdx)->isCalculatedPersp() ){
      mPerspInc = glm::vec2(-0.2, 0.0);
      mGridImg.at(mCurrentInputIdx)->addInputPersp(mPerspInc, mPerspectiveIndex);
      mCurretPerspInc = mGridImg.at(mCurrentInputIdx)->getInputPersp(mPerspectiveIndex);
    }
  }

  //up, down

  if (key == '5') {
    mGridImg.at(mCurrentInputIdx)->setCropDisp(mGridImg.at(mCurrentInputIdx)->getCropDisp() + glm::vec2(1, 1));
  }

  if (key == '6') {
    mGridImg.at(mCurrentInputIdx)->setCropDisp(mGridImg.at(mCurrentInputIdx)->getCropDisp() - glm::vec2(1, 1));
  }

  if (key == '2') {
      mGridDetector.at(mCurrentInputIdx)->saveGridJson();
      ofLog(OF_LOG_NOTICE) << "saved json grid positions: " << mCurrentInputIdx;
  }

  if (key == '3') {
      ofJson writer;
      int i = 0;
      for (auto &gridImage : mGridImg) {
        ofJson pt;
        std::string inputImg("cam_" + to_string(i));

        //cam id
        pt[inputImg]["camId"] = gridImage->getCamId();

        //cut
        pt[inputImg]["x1"]    = gridImage->getCropUp().x;
        pt[inputImg]["y1"]    = gridImage->getCropUp().y;
        pt[inputImg]["x2"]    = gridImage->getCropDown().x;
        pt[inputImg]["y2"]    = gridImage->getCropDown().y;
        pt[inputImg]["disX"]  = gridImage->getCropDisp().x;
        pt[inputImg]["disY"]  = gridImage->getCropDisp().y;

        //img processing values
        pt[inputImg]["gamma"] = gridImage->getGamma();
        pt[inputImg]["alpha"] = gridImage->getAlpha();
        pt[inputImg]["beta"]  = gridImage->getBeta();

        //save perspective
        pt[inputImg]["px0"] = gridImage->getInputPersp(0).x;
        pt[inputImg]["py0"] = gridImage->getInputPersp(0).y;
        pt[inputImg]["px1"] = gridImage->getInputPersp(1).x;
        pt[inputImg]["py1"] = gridImage->getInputPersp(1).y;
        pt[inputImg]["px2"] = gridImage->getInputPersp(2).x;
        pt[inputImg]["py2"] = gridImage->getInputPersp(2).y;
        pt[inputImg]["px3"] = gridImage->getInputPersp(3).x;
        pt[inputImg]["py3"] = gridImage->getInputPersp(3).y;

        //coordinate
        pt[inputImg]["mapMinX"] = mGridDetector.at(i)->getCoordMapMinX();
        pt[inputImg]["mapMaxX"] = mGridDetector.at(i)->getCoordMapMaxX();
        pt[inputImg]["mapMinY"] = mGridDetector.at(i)->getCoordMapMinY();
        pt[inputImg]["mapMaxY"] = mGridDetector.at(i)->getCoordMapMaxY();

        writer.push_back(pt);
        i++;
      }

      ofLog(OF_LOG_NOTICE) << "Image json write grid";
      ofSaveJson("img.json", writer);
  }

  if(key == '4'){
    ofLog(OF_LOG_NOTICE)<<"Loading Cam files";
    ofFile file("img.json");
    if (file.exists()) {
      ofJson camjs;
      file >> camjs;
      int j = 0;
      for (auto & cam : camjs) {
        if(j < mNumInputs){
          std::string inputImg("cam_" + to_string(j));
          int camId =  cam[inputImg]["camId"];
          ofLog(OF_LOG_NOTICE)<<"Loading: " << j << ": CamId: " << camId<<" "<<std::endl;

          mGridImg.at(j)->resetPerspetive();
          mGridImg.at(j)->calculatedPersp();

          //perspective
          glm::vec2 inputQuad0 = glm::vec2(cam[inputImg]["px0"], cam[inputImg]["py0"]);
          glm::vec2 inputQuad1 = glm::vec2(cam[inputImg]["px1"], cam[inputImg]["py1"]);
          glm::vec2 inputQuad2 = glm::vec2(cam[inputImg]["px2"], cam[inputImg]["py2"]);
          glm::vec2 inputQuad3 = glm::vec2(cam[inputImg]["px3"], cam[inputImg]["py3"]);

          mGridImg.at(j)->setInputPersp(inputQuad0, 0);
          mGridImg.at(j)->setInputPersp(inputQuad1, 1);
          mGridImg.at(j)->setInputPersp(inputQuad2, 2);
          mGridImg.at(j)->setInputPersp(inputQuad3, 3);

          //coordinate
          float minX = cam[inputImg]["mapMinX"];
          float maxX = cam[inputImg]["mapMaxX"];
          float minY = cam[inputImg]["mapMinY"];
          float maxY = cam[inputImg]["mapMaxY"];
          mGridDetector.at(j)->setMapCoord(minX, maxX, minY, maxY);

          //update GUI
          mMapMinX->ofParam = minX;
          mMapMaxX->ofParam = maxX;
          mMapMinY->ofParam = minY;
          mMapMaxY->ofParam = maxY;

          //read beta alpha gama
          float gamma = cam[inputImg]["gamma"];
          float alpha = cam[inputImg]["alpha"];
          float beta  = cam[inputImg]["beta"];

          //beta alpha
          mGridImg.at(j)->setGamma(gamma);
          mGridImg.at(j)->setAlpha(alpha);
          mGridImg.at(j)->setBeta(beta);

          mGammaValue->ofParam = gamma;
          mAlphaValue->ofParam = alpha;
          mBetaValue->ofParam  = beta;

          j++;
          if(j == 1)
            break;
        }
      }
    }
  }

  if(key == '5'){
    ofLog(OF_LOG_NOTICE)<<"Loading Cam files";
    ofFile file("img.json");
    if (file.exists()) {
      ofJson camjs;
      file >> camjs;
      int j = 1;
      for (auto & cam : camjs) {
        if(j < mNumInputs){
          std::string inputImg("cam_" + to_string(j));
          int camId =  cam[inputImg]["camId"];
          ofLog(OF_LOG_NOTICE)<<"Loading: " << j << ": CamId: " << camId<<" "<<std::endl;

          mGridImg.at(j)->resetPerspetive();
          mGridImg.at(j)->calculatedPersp();

          //perspective
          glm::vec2 inputQuad0 = glm::vec2(cam[inputImg]["px0"], cam[inputImg]["py0"]);
          glm::vec2 inputQuad1 = glm::vec2(cam[inputImg]["px1"], cam[inputImg]["py1"]);
          glm::vec2 inputQuad2 = glm::vec2(cam[inputImg]["px2"], cam[inputImg]["py2"]);
          glm::vec2 inputQuad3 = glm::vec2(cam[inputImg]["px3"], cam[inputImg]["py3"]);

          mGridImg.at(j)->setInputPersp(inputQuad0, 0);
          mGridImg.at(j)->setInputPersp(inputQuad1, 1);
          mGridImg.at(j)->setInputPersp(inputQuad2, 2);
          mGridImg.at(j)->setInputPersp(inputQuad3, 3);

          //coordinate
          float minX = cam[inputImg]["mapMinX"];
          float maxX = cam[inputImg]["mapMaxX"];
          float minY = cam[inputImg]["mapMinY"];
          float maxY = cam[inputImg]["mapMaxY"];
          mGridDetector.at(j)->setMapCoord(minX, maxX, minY, maxY);

          //update GUI
          mMapMinX->ofParam = minX;
          mMapMaxX->ofParam = maxX;
          mMapMinY->ofParam = minY;
          mMapMaxY->ofParam = maxY;

          //read beta alpha gama
          float gamma = cam[inputImg]["gamma"];
          float alpha = cam[inputImg]["alpha"];
          float beta  = cam[inputImg]["beta"];

          //beta alpha
          mGridImg.at(j)->setGamma(gamma);
          mGridImg.at(j)->setAlpha(alpha);
          mGridImg.at(j)->setBeta(beta);

          mGammaValue->ofParam = gamma;
          mAlphaValue->ofParam = alpha;
          mBetaValue->ofParam  = beta;

          j++;
          if(j == 2)
            break;
        }
      }
    }
  }

  //send test json file
  if(key == 'j'){
    sendUDPJson();
  }

  //perspective
  if (key == 'p') {
    //add corners to the perspective
    mActivePerspectivePoints  = !mActivePerspectivePoints;
    ofLog(OF_LOG_NOTICE) << "Perspective Points " << mCurrentInputIdx<<" "<<mActivePerspectivePoints;
  }
  //reset perspective
  if(key == 'o'){
    mGridImg.at(mCurrentInputIdx)->resetPerspetive();
    mActivePerspectivePoints = false;
    ofLog(OF_LOG_NOTICE) << "Reset Perspective" << mCurrentInputIdx;
  }

  if (key == 'd') {
    mDebug = !mDebug;
    ofLog(OF_LOG_NOTICE) << "Debug " << mDebug;
  }

  if (key == 'r') {
    mRefimentDetector = !mRefimentDetector;
    ofLog(OF_LOG_NOTICE) << "Refiment " << mRefimentDetector;
  }

  if (key == 'c') {
  }

  if (key == 'n') {
    mGridDetector.at(mCurrentInputIdx)->toogleDebugGrid();
  }

  if (key == '8') {
    mGridImg.at(mCurrentInputIdx)->resetCrop();
    ofLog(OF_LOG_NOTICE) << "Reset Crop " << mCurrentInputIdx;
  }

  if (key == 'u') {
    std::string msgInfo(mUDPRadarIp+" "+to_string(mUDPRadarPort));
    mUDPConnectionTable.Send(msgInfo.c_str(), msgInfo.length());
    ofLog(OF_LOG_NOTICE) << "Set UDP Table Test";
  }

  if (key == '0') {
    mGridDetector.at(mCurrentInputIdx)->generateGridPos();
    ofLog(OF_LOG_NOTICE) << "Reset gris pos " << mCurrentInputIdx;
  }

  if(key == 'v'){
    mGridDetector.at(mCurrentInputIdx)->toogleUpdateGrid();
    ofLog(OF_LOG_NOTICE) << "Update grid Position";
  }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

  if(mActivePerspectivePoints){
    mMousePos.x = x;
    mMousePos.y = y;
  }

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
  if (mDebug) {
    mGridDetector.at(mCurrentInputIdx)->setGridPos(glm::vec2(x, y));
  }

  //crop input camera information
  if (mBEnableCrop->isActive()) {
    {
      float distUp = ofDist(mGridImg.at(mCurrentInputIdx)->getCropUp().x,  mGridImg.at(mCurrentInputIdx)->getCropUp().y, x, y);
      if (distUp >= 0.0 && distUp <= 35) {
        mGridImg.at(mCurrentInputIdx)->setCropUp(glm::vec2(x, y));
      }

      float distDown =
          ofDist(mGridImg.at(mCurrentInputIdx)->getCropDown().x, mGridImg.at(mCurrentInputIdx)->getCropDown().y, x, y);
      if (distDown >= 0.0 && distDown <= 35) {
        mGridImg.at(mCurrentInputIdx)->setCropDown(glm::vec2(x, y));
      }
    }
  }


}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
  //move center points for the GUI
  if(mActivePerspectivePoints){
    mGridImg.at(mCurrentInputIdx)->updateCorners(glm::vec2(x, y));
  }

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//--------------------------------------------------------------
void ofApp::exit(){
  mGridImg.clear();
	cout<<"exit app"<<std::endl;
}
