/*

Thomas Sanchez Lengeling
March, 2019

Living Line

*/

void ofApp::setupValues() {
  // num Inputs
  mNumInputs        = NUM_CAM_INPUTS;

  mDrawGUI          = true;
  mRefimentDetector = true;
  mCurrentInputIdx  = 0;
  mTotalMaxMarkers  = 0;

  mActivePerspectivePoints = false;
  mPerspInc = glm::vec2(0, 0);
  mPerspectiveIndex = 0;
  //load imputs for #cameras

  if(mNumInputs >= 1){
    ofLog(OF_LOG_NOTICE) << "Reading Cam Definition griddef.json";
    ofFile file("griddef.json");
    if (file.exists()) {
      ofJson gridjs;
      file >> gridjs;
      for (int j = 0; j < mNumInputs; j++) {
        std::string inputGrid("grid_" + to_string(j));
        ofLog(OF_LOG_NOTICE) << inputGrid<<" "<<j;
        mGridSizes.push_back( glm::vec2(gridjs[inputGrid]["x"], gridjs[inputGrid]["y"] ) );
        mMaxMarkers.push_back( gridjs[inputGrid]["max"]);
        mTotalMaxMarkers += gridjs[inputGrid]["max"].get<int>();
        ofLog(OF_LOG_NOTICE) <<j<<": "<<glm::vec2(gridjs[inputGrid]["x"], gridjs[inputGrid]["y"] ) <<" "<< gridjs[inputGrid]["max"]<<std::endl;
      }
      ofLog(OF_LOG_NOTICE) << "Done grid values JSON";
    } else {
      ofLog(OF_LOG_NOTICE) << "File does not exist griddef.json";
    }
  }
  ofLog(OF_LOG_NOTICE) << "Max Grid: " << mFullGridDim.x << " " << mFullGridDim.y;

  // load video first
  mTotalMarkers = 0;

  ofLog(OF_LOG_NOTICE) << "done setup init";
}

//-----------------------------------------------------------------------------
void ofApp::setupConnection() {
  std::string jsonNet = "networkUDP.json";
  ofLog(OF_LOG_NOTICE) << "Setup newtwork: " << jsonNet;
  ofFile file(jsonNet);
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &net : js) {
      if (i == 0) {
        mUDPIp = net["network_" + to_string(i)]["ip"];
        mUDPPort = int(net["network_" + to_string(i)]["port"]);
      } else if (i == 1) {
        mUDPRadarIp = net["network_" + to_string(i)]["ip"];
        mUDPRadarPort = int(net["network_" + to_string(i)]["port"]);
      }
      i++;
    }
    ofLog(OF_LOG_NOTICE) << "Loaded: UDP Table:";
    ofLog(OF_LOG_NOTICE) << "IP: " << mUDPIp << " Port: " << mUDPPort;

    ofLog(OF_LOG_NOTICE) << "Loaded: UDP Radar:";
    ofLog(OF_LOG_NOTICE) << "IP: " << mUDPRadarIp << " Port: " << mUDPRadarPort;
  } else {
    mUDPIp = "127.0.0.1";
    mUDPPort = 15800;
    ofLog(OF_LOG_NOTICE) << "fail loading newtwork: " << jsonNet
                         << " Default: " << mUDPIp << " " << mUDPPort;
  }

  //radar
  ofxUDPSettings settingsRadar;
  settingsRadar.sendTo(mUDPRadarIp, mUDPRadarPort);
  settingsRadar.blocking = false;
  mUDPConnectionRadar.Setup(settingsRadar);

  // table
  ofxUDPSettings settingsTable;
  settingsTable.sendTo(mUDPIp, mUDPPort);
  settingsTable.blocking = false;
  mUDPConnectionTable.Setup(settingsTable);

  if (mDebug) {
    std::string message = "connected to Aruco Detector";
    mUDPConnectionTable.Send(message.c_str(), message.length());
  }

  ofLog(OF_LOG_NOTICE) << "done setup UDP connection ";
}

//-----------------------------------------------------------------------------
void ofApp::setupGUI() {

  mBDebugVideo = ofxDatButton::create();
  mBDebugVideo->button = new ofxDatGuiToggle("Debug View", true);
  mBDebugVideo->button->setPosition(10, 10);
  mBDebugVideo->button->setWidth(110, .4);
  mBDebugVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugVideo->mActive = !mBDebugVideo->mActive;
  });

  mBDebugGrid = ofxDatButton::create();
  mBDebugGrid->button = new ofxDatGuiToggle("Aruco View", false);
  mBDebugGrid->button->setPosition(10, 60);
  mBDebugGrid->button->setWidth(110, .4);
  mBDebugGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugGrid->mActive = !mBDebugGrid->mActive;
  });

  mBDebugVideoGrid = ofxDatButton::create();
  mBDebugVideoGrid->button = new ofxDatGuiToggle("Grid View", false);
  mBDebugVideoGrid->button->setPosition(10, 110);
  mBDebugVideoGrid->button->setWidth(110, .4);
  mBDebugVideoGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugVideoGrid->mActive = !mBDebugVideoGrid->mActive;
  });

  mBSingleGrid = ofxDatButton::create();
  mBSingleGrid->button = new ofxDatGuiToggle("Single Input", false);
  mBSingleGrid->button->setPosition(10, 160);
  mBSingleGrid->button->setWidth(110, .4);
  mBSingleGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBSingleGrid->mActive = !mBSingleGrid->mActive;
  });

  mBFullGrid = ofxDatButton::create();
  mBFullGrid->setActivation(false);
  mBFullGrid->button = new ofxDatGuiToggle("Full Input");
  mBFullGrid->button->setPosition(10, 210);
  mBFullGrid->button->setWidth(110, .4);
  mBFullGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBFullGrid->mActive = !mBFullGrid->mActive;

    if (mBFullGrid->isActive()) {
      std::cout << "done allocated full img";
    } else {
      std::cout << "done allocated single img";
    }
  });

  mBFullCamView = ofxDatButton::create();
  mBFullCamView->setActivation(false);
  mBFullCamView->button = new ofxDatGuiToggle("Full Cam View");
  mBFullCamView->button->setPosition(10, 260);
  mBFullCamView->button->setWidth(110, .4);
  mBFullCamView->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBFullCamView->mActive = !mBFullCamView->mActive;
  });

  mCamCalibration = ofxDatButton::create();
  mCamCalibration->setActivation(false);
  mCamCalibration->button = new ofxDatGuiToggle("Calibration");
  mCamCalibration->button->setPosition(10, 310);
  mCamCalibration->button->setWidth(110, .4);
  mCamCalibration->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mCamCalibration->mActive = !mBFullCamView->mActive;
  });

  mCamPerspective = ofxDatButton::create();
  mCamPerspective->setActivation(false);
  mCamPerspective->button = new ofxDatGuiToggle("Perspective");
  mCamPerspective->button->setPosition(10, 360);
  mCamPerspective->button->setWidth(110, .4);
  mCamPerspective->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mCamPerspective->mActive = !mCamPerspective->mActive;
  });

  mSendUDP = ofxDatButton::create();
  mSendUDP->setActivation(false);
  mSendUDP->button = new ofxDatGuiToggle("Send Json");
  mSendUDP->button->setPosition(10, 410);
  mSendUDP->button->setWidth(110, .4);
  mSendUDP->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mSendUDP->mActive = !mSendUDP->mActive;
  });

  mDebugMode = ofxDatButton::create();
  mDebugMode->setActivation(false);
  mDebugMode->button = new ofxDatGuiToggle("Debug");
  mDebugMode->button->setPosition(10, 460);
  mDebugMode->button->setWidth(110, .4);
  mDebugMode->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mDebugMode->mActive = !mDebugMode->mActive;
  });

  int sliderStartX = 150;

  mBEnableCrop = ofxDatButton::create();
  mBEnableCrop->setActivation(false);
  mBEnableCrop->button = new ofxDatGuiToggle("Toogle Crop");
  mBEnableCrop->button->setPosition(sliderStartX, 10);
  mBEnableCrop->button->setWidth(390, .4);
  mBEnableCrop->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBEnableCrop->mActive = !mBEnableCrop->mActive;
    mGridImg.at(mCurrentInputIdx)->toogleCrop();
  });

  mBEnableCams = ofxDatButton::create();
  mBEnableCams->setActivation(false);
  mBEnableCams->button = new ofxDatGuiToggle("Toogle Cam");
  mBEnableCams->button->setPosition(sliderStartX, 60);
  mBEnableCams->button->setWidth(390, .4);
  mBEnableCams->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    setupCam();
  });

  //gamma
  mGammaValue = ofxDatSlider::create();
  mGammaValue->slider =
      new ofxDatGuiSlider(mGammaValue->ofParam.set("gamma", 1.29, 0, 2));
  mGammaValue->slider->setWidth(390, .4);
  mGammaValue->slider->setPosition(sliderStartX, 110);
  mGammaValue->slider->onSliderEvent(
      [&](ofxDatGuiSliderEvent v) { mGammaValue->ofParam = v.value; });

  //alpha
  mAlphaValue = ofxDatSlider::create();
  mAlphaValue->slider =  new ofxDatGuiSlider(mAlphaValue->ofParam.set("alpha", 1.03, 0, 3));
  mAlphaValue->slider->setWidth(390, .4);
  mAlphaValue->slider->setPosition(sliderStartX, 160);
  mAlphaValue->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) { mAlphaValue->ofParam = v.value; });

  //beta
  mBetaValue = ofxDatSlider::create();
  mBetaValue->slider = new ofxDatGuiSlider(mBetaValue->ofParam.set("beta", 17.21, 0, 255));
  mBetaValue->slider->setWidth(390, .4);
  mBetaValue->slider->setPosition(sliderStartX, 210);
  mBetaValue->slider->onSliderEvent( [&](ofxDatGuiSliderEvent v) { mBetaValue->ofParam = v.value; });



  mBDebugMarkers = ofxDatButton::create();
  mBDebugMarkers->setActivation(false);
  mBDebugMarkers->button = new ofxDatGuiToggle("Toogle Markers Draw");
  mBDebugMarkers->button->setPosition(sliderStartX, 260);
  mBDebugMarkers->button->setWidth(390, .4);
  mBDebugMarkers->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    for (auto &det : mArucoDetector) {
      det->toggleMarkerInfo();
    }
  });

  mBGridSelect = ofxDatMatrix::create();
  mBGridSelect->matrix = new ofxDatGuiMatrix("Grid Matrix", 2 * 2, true);
  mBGridSelect->matrix->setRadioMode(true);
  mBGridSelect->matrix->setOpacity(0.8);
  mBGridSelect->matrix->setWidth(390, .4);
  mBGridSelect->matrix->setPosition(sliderStartX, 310);
  mBGridSelect->matrix->onMatrixEvent([&](ofxDatGuiMatrixEvent v) {
    mCurrentInputIdx = v.child;
    ofLog(OF_LOG_NOTICE) << "Index: " << mCurrentInputIdx << std::endl;
    mFboSingle.begin();
    ofClear(0, 0, 0, 255);
    mFboSingle.end();
  });

  mViewCams = ofxDatButton::create();
  mViewCams->setActivation(false);
  mViewCams->button = new ofxDatGuiToggle("View Cams");
  mViewCams->button->setPosition(sliderStartX, 360);
  mViewCams->button->setWidth(390, .4);
  mViewCams->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mViewCams->mActive = !mViewCams->mActive;
  });

  //coodinate mapping
  mMapMinX = ofxDatSlider::create();
  mMapMinX->slider =  new ofxDatGuiSlider(mMapMinX->ofParam.set("Map Min X", 0, -325, 325));
  mMapMinX->slider->setWidth(390, .4);
  mMapMinX->slider->setPosition(sliderStartX, 410);
  mMapMinX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) { mMapMinX->ofParam = v.value; });

  mMapMaxX = ofxDatSlider::create();
  mMapMaxX->slider =  new ofxDatGuiSlider(mMapMaxX->ofParam.set("Map Max X", 1.03, -325, 325));
  mMapMaxX->slider->setWidth(390, .4);
  mMapMaxX->slider->setPosition(sliderStartX, 460);
  mMapMaxX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) { mMapMaxX->ofParam = v.value; });

  mMapMinY = ofxDatSlider::create();
  mMapMinY->slider =  new ofxDatGuiSlider(mMapMinY->ofParam.set("Map Min Y", 1.03, -145, 145));
  mMapMinY->slider->setWidth(390, .4);
  mMapMinY->slider->setPosition(sliderStartX, 510);
  mMapMinY->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) { mMapMinY->ofParam = v.value; });

  mMapMaxY = ofxDatSlider::create();
  mMapMaxY->slider =  new ofxDatGuiSlider(mMapMaxY->ofParam.set("Map Max Y", 1.03, -145, 145));
  mMapMaxY->slider->setWidth(390, .4);
  mMapMaxY->slider->setPosition(sliderStartX, 560);
  mMapMaxY->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) { mMapMaxY->ofParam = v.value; });

  mUpdateCoordinate = ofxDatButton::create();
  mUpdateCoordinate->setActivation(false);
  mUpdateCoordinate->button = new ofxDatGuiToggle("Update Coordinate");
  mUpdateCoordinate->button->setPosition(sliderStartX, 610);
  mUpdateCoordinate->button->setWidth(390, .4);
  mUpdateCoordinate->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mUpdateCoordinate->mActive = !mUpdateCoordinate->mActive;
  });



  // initial comands for setup
  mBFullGrid->setActivation(false);
  mBDebugVideoGrid->setActivation(false);

  ofLog(OF_LOG_NOTICE) << "done setup gui";
}
//-----------------------------------------------------------------------------
void ofApp::setupDetection() {
  for (int i = 0; i < mNumInputs; i++) {
    QRDetectorRef detector = QRDetector::create();
    detector->setupCalibration(mGridSizes.at(i).x, mGridSizes.at(i).y);
    mArucoDetector.push_back(detector);
  }

  ofLog(OF_LOG_NOTICE) << "done setup detector: " << mArucoDetector.size();
}

//-----------------------------------------------------------------------------
void ofApp::setupCalibration() {
  ofLog(OF_LOG_NOTICE) << "done setup calibration";
}

//-----------------------------------------------------------------------------
void ofApp::setupCam() {

  // setup inputs
  ofLog(OF_LOG_NOTICE) << "setting inputs: " << mNumInputs;

  for (int i = 0; i < mNumInputs; i++) {
    GridImageRef gridImage =
        GridImage::create(glm::vec2(CAM_WIDTH, CAM_HEIGHT));
    gridImage->setId(i);
    mGridImg.push_back(gridImage);
  }

  // Print available devices
  ofLog(OF_LOG_NOTICE) << "Cam devices:";

  if(mNumInputs >= 1){
    ofLog(OF_LOG_NOTICE) << "Loading cam and crop positions "<<mGridImg.size();
    ofFile file("img.json");
    bool foundFile = false;
    if (file.exists()) {
      ofJson camjs;
      file >> camjs;
      int j = 0;
      for (auto & cam : camjs) {
        if(j < mNumInputs){
          std::string inputImg("cam_" + to_string(j));
          int camId =  cam[inputImg]["camId"];
          ofLog(OF_LOG_NOTICE)<<"Loading: " << j << ": CamId: " << camId<<" "<<std::endl;


          mGridImg.at(j)->setCropUp(glm::vec2(cam[inputImg]["x1"], cam[inputImg]["y1"]));
          mGridImg.at(j)->setCropDown(glm::vec2(cam[inputImg]["x2"], cam[inputImg]["y2"]));
          mGridImg.at(j)->setCropDisp(glm::vec2(cam[inputImg]["disX"], cam[inputImg]["disY"]));

          float gm = float(cam[inputImg]["gamma"]);
          mGridImg.at(j)->setGamma(gm);

          ofLog(OF_LOG_NOTICE) << "Loading cam devices:";
          mGridImg.at(j)->setupCam(camId, CAM_FRAMERATE);

          ofLog(OF_LOG_NOTICE) << "Gamma: " << gm;
          j++;
        }
      }
      ofLog(OF_LOG_NOTICE) << "Done crop values JSON";
      foundFile = true;
    } else {
      ofLog(OF_LOG_NOTICE) << "File does not exist img.json";
    }
  }

  // fill the fbos with the appropiate dimentions
  mFboSingle.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);

  // clean start with Fbos
  mFboSingle.begin();
  ofClear(0, 0, 0, 255);
  mFboSingle.end();

  ofLog(OF_LOG_NOTICE) << "done setup video";
}


//-----------------------------------------------------------------------------
void ofApp::setupGridDetector() {

  //generate detector information
  for (int i = 0; i < mNumInputs; i++) {
    ofLog(OF_LOG_NOTICE) << "setup grid: " << i << " " << mGridSizes.at(i);
    GridDetectorRef griD = GridDetector::create(mGridSizes.at(i));
    griD->setId(i);
    griD->setupGridJsonPos("gridpos_0" + to_string(i + 1) + ".json");

    griD->setMaxMarkers(mMaxMarkers.at(i));
    griD->setupCleaner();
    griD->setupBlocks();

    mGridDetector.push_back(griD);

    // allocate fbo for drawing
    ofFbo fboTemp;
    fboTemp.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGB);
    fboTemp.begin();
    ofClear(0, 0, 0);
    fboTemp.end();
    mFboGrid.push_back(fboTemp);
  }

  ofLog(OF_LOG_NOTICE) << "done setup grid :" << mNumInputs;
}

//-----------------------------------------------------------------------------
