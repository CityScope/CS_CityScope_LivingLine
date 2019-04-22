#include "GridDetector.h"

GridDetector::GridDetector(glm::vec2 dim) {
  mGridDim = dim;
  mDebugGrid = false;
  mRecordOnce = true;
  mCalibrateGrid = false;
  mCleanDone = false;
  mCurrentGridId =0;

  mRadDetection = RAD_DETECTION;
  mMaxMarkers = mGridDim.x * mGridDim.y;

  //coordinate
  mCoordMapMinX = 0;
  mCoordMapMaxX = 0;
  mCoordMapMinY = 0;
  mCoordMapMaxY = 0;
}
//-----------------------------------------------------------------------------
void GridDetector::generateGridPos() {
  mMarkers.clear();
  float startGridX = 1920 * 0.1;
  float startGridY = 1080 * 0.1;

  float stepX = 50.0;
  float stepY = 50.0;

  float gapX = 13;
  float gapY = 13;

  int indeY = 0;
  int indeX = 0;

  ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers;
  for (int i = 0; i < mMaxMarkers; i++) {
    MarkerArucoRef m = MarkerAruco::create();

    float x = indeX * stepX + indeX * gapX + startGridX;
    float y = indeY * stepY + indeY * gapY + startGridY;

    m->setPos(glm::vec2(x, y));
    m->setGridId(i);
    m->setMarkerId(-1);
    mMarkers.push_back(m);
    indeX++;
    if (indeX >= mGridDim.x) {
      indeY++;
      indeX = 0;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setMaxMarkers(int max) {
  mMaxMarkers = max;
  ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers;
}

//-----------------------------------------------------------------------------
void GridDetector::setId(int id) { mId = id; }

//-----------------------------------------------------------------------------
void GridDetector::setupBlocks() {
  for (int i = 0; i < mMaxMarkers; i++) {
    QRBlockRef block = QRBlock::create();

    block->setMarkerId(i);
    mBlocks.push_back(block);
  }

  ofLog(OF_LOG_NOTICE) << "setup blocks ";
}

//-----------------------------------------------------------------------------
void GridDetector::setupCleaner() {

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 3; ///
  mCleanDone = false;

  for (int i = 0; i < mMaxMarkers; i++) {
    mIdsCounter.emplace(i, 0); // mFullIds.at(i), 0);
    mProCounter.emplace(i, 0);
  }

  ofLog(OF_LOG_NOTICE) << "setup clean";
}

//-----------------------------------------------------------------------------
//--- save json position
void GridDetector::setupGridJsonPos(std::string filePos) {
  ofLog(OF_LOG_NOTICE) << "Loading gridpos json: " << filePos;
  ofFile file(filePos); //"gridpos.json");
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &gridPos : js) {
      if (i < mMaxMarkers) {
        MarkerArucoRef m = MarkerAruco::create();
        m->setMarkerId(-1);

        int type = gridPos[to_string(i)]["type"];

        if (type == BlockType::grid) {

          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];

          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);

          //positions to the detect the markers
          mMarkers.push_back(m);
        }
      }
      i++;
    }
  } else {
    ofLog(OF_LOG_NOTICE) << "cannot find file";
  }
}

float GridDetector::setMapCoord(float minX, float maxX, float minY, float maxY){
  mCoordMapMinX = minX;
  mCoordMapMaxX = maxX;
  mCoordMapMinY = minY;
  mCoordMapMaxY = maxY;
}

//-----------------------------------------------------------------------------
void GridDetector::generateMarkers(std::vector<int> &ids,
                                   std::vector<QRBlockRef> &blocks) {

  //copy identified ids
  mTagsIds = ids;

  //copy positions of the qr blokcs
  mCurrBlock = blocks;

  //calculate rotation
  calculateRotations();

  mNumMarkers = mCurrBlock.size();

  //collection of blocks for error detection.
  mTmpBlocks.push_back(mCurrBlock);
}

//-----------------------------------------------------------------------------
void GridDetector::drawBlock(float posx, float posy, float size, float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mBlocks) {

    ofSetColor(0, 255, 255);

    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(block->getMarkerId(), x + squareSize / 3.0, y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(block->getType(), x + squareSize / 3.0, y + squareSize * (2.0 / 3.0));
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::drawDetectedGridIn(float posx, float posy, float size,
                                      float space) {
  int i = 0;
  int j = 0; // mGridDim.y - 1;
  float squareSize = size;
  float squareSpace = space;
  for (auto &mk : mMarkers) {
    if (mk->isEnable()) {
      ofSetColor(0, 200, 255);
    } else {
      ofSetColor(255, 255, 0);
    }
    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(mk->getGridId(), x + squareSize / 3.0, y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(mk->getMarkerId(), x + squareSize / 3.0, y + squareSize * (2.0 / 3.0));
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::drawDetectedGrid(float posx, float posy, float size,
                                    float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mCurrBlock) {
    if (block->getType() == -1) {
      ofSetColor(255, 255, 0);
    } else {
      ofSetColor(0, 200, 255);
    }
    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    float strx = x + squareSize / 3.0;
    float stry1 = y + squareSize * (1.0 / 3.0);
    float stry2 = y + squareSize * (2.0 / 3.0);
    ofDrawBitmapString(block->getType(), strx, stry1);
    ofDrawBitmapString(block->getMarkerId(), strx, stry2);
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::updateBlockTypes() {
  // update blocks and types
  for (auto &block : mBlocks) {
    int id = block->getMarkerId();
    for (auto &mk : mMarkers) {
      if (mk->getIdTypePair().first == id) {
        mk->updateTypePair(block->getType());
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setGridPos(glm::vec2 mousePos) {
  if (mDebugGrid) {
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk->getPos();
      float dist = glm::fastDistance(pos, mousePos);
      if (dist >= 0.0 && dist <= MOUSE_RADIUS) {
        mk->setPos(mousePos);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::drawMarkers() {
  for (auto &mk : mMarkers) {
    glm::vec2 pos = mk->getPos();

    if (mk->isEnable()) {
      ofSetColor(255);
      ofDrawCircle(pos.x, pos.y, 7);
      ofSetColor(200, 80);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0);
    } else {
      ofSetColor(255, 255, 0);
      ofDrawCircle(pos.x, pos.y, 4);
      ofSetColor(255, 355, 100);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0);
    }

    //update grid position
    if (mDebugGrid) {
      ofSetColor(0, 100, 200, 100);
      ofDrawCircle(pos.x, pos.y, mRadDetection);
    }
    if(mUpdateGrid){
      if(mk->isDebugPos()){
        ofSetColor(80, 80, 150, 150);
        ofDrawCircle(pos.x, pos.y, mRadDetection);
      }
    }

    ofSetColor(255);
    ofDrawBitmapString(mk->getMarkerId(), pos.x - 20, pos.y - 7);
    ofDrawBitmapString(mk->getGridId(), pos.x - 25, pos.y - 17);
  }
}

//-----------------------------------------------------------------------------
void GridDetector::calculateRotations(){
  //draw rotation of the marker
  for (auto & detectedMk : mCurrBlock) {
    detectedMk->calculateRotation();
  }
}

//-----------------------------------------------------------------------------
void GridDetector::gridPosIdInc(){
  //disable current maker
  mMarkers.at(mCurrentGridId)->disableDebugPos();

  mCurrentGridId++;
  if(mCurrentGridId >= mMaxMarkers){
    mCurrentGridId = 0;
  }

  //update current marker
  mMarkers.at(mCurrentGridId)->enableDebugPos();
}

//-----------------------------------------------------------------------------
void GridDetector::gridPosIdDec(){
  //disable current maker
  mMarkers.at(mCurrentGridId)->disableDebugPos();

  mCurrentGridId--;
  if(mCurrentGridId <= 0){
    mCurrentGridId = mMaxMarkers - 1;
  }

    //update current marker
    mMarkers.at(mCurrentGridId)->enableDebugPos();
}

//-----------------------------------------------------------------------------
// save json
void GridDetector::saveGridJson() {
  saveGridJson("gridpos_0" + to_string(mId + 1) + ".json");
}

//-----------------------------------------------------------------------------
void GridDetector::saveGridJson(std::string fileName){
  ofJson writer;
  int i = 0;
  for (auto &mk : mMarkers) {
    ofJson pt;
    pt[to_string(i)]["posx"] = mk->getPos().x;
    pt[to_string(i)]["posy"] = mk->getPos().y;
    pt[to_string(i)]["type"] = mk->getBlockType();
    writer.push_back(pt);
    i++;
  }
  ofLog(OF_LOG_NOTICE) << "json write: 0" + to_string(mId + 1) + " - " + to_string(mId);
  ofSaveJson(fileName, writer);
}

//-----------------------------------------------------------------------------
void GridDetector::calibrateGrid() {
  // draw grid
}

//-----------------------------------------------------------------------------
void GridDetector::updateCleaner() {
  // update clenaer variables
  mWindowCounter++;
  if (mWindowCounter >= mWindowIterMax) {
    mWindowCounter = 0;
    mCleanDone = true;
  }
}
//-----------------------------------------------------------------------------
void GridDetector::resetCleaner() {
  // reset
  if (mCleanDone) {
    mWindowCounter = 0;
    mCleanDone = false;
  }
}

//-----------------------------------------------------------------------------
void GridDetector::cleanGrid() {
  if (mCleanDone) {
    // clasical probabilty of ocurance

    // ofLog(OF_LOG_NOTICE) << "reset proba";
    for (auto &mk : mMarkers) {
      mk->resetProba();
    }

    //clean free memory
    mCurrFree.clear();
    for(int i = 0; i < MAX_MARKERS; i++){
      QRBlockRef free =  QRBlock::create();
      mCurrFree.push_back(free);
    }

    // ofLog(OF_LOG_NOTICE) << "calculate freq";
    // calculate the frequency of ocurance
    for (auto &blocks : mTmpBlocks) {
      for (auto &block : blocks) {
        glm::vec2 blockPos = block->getPos();

        int k = 0;
        for (auto &mk : mMarkers) {
          glm::vec2 boardPos = mk->getPos();
          float dis = glm::fastDistance(blockPos, boardPos);
          int qrId  = block->getMarkerId();
          //check if the detection was free unit or a knob
          if ( (dis >= 0 && dis <= mRadDetection)) { //free units
            mIdsCounter[k] = block->getMarkerId(); // block.mId
            mk->incProba();
            // not sure i need it break;
            break;
          }
          k++;
        }
      }
    } //done calculating probabilty


    //free units
    for (auto &blocks : mTmpBlocks) {
      for (auto &block : blocks) {
        int qrId  = block->getMarkerId();

      //  ofLog(OF_LOG_NOTICE) <<" "<< block->getRot() ;

          //check if the detection was free unit or a knob
        if ( qrId == 37){
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }else if( qrId == 38){
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }else if( qrId == 45 ){
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }else if ( qrId == 40){ //free units
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }else if( qrId == 41){
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }else if( qrId == 49 ){
          mCurrFree.at(qrId)->addPos( block->getPos() );
          mCurrFree.at(qrId)->addRot( block->getRot() );
          mCurrFree.at(qrId)->setMarkerId(qrId);
          mCurrFree.at(qrId)->incProba();
        }
      }
    } //done calculating probabilty

    //check the markers that have been detected
    int i = 0;
    int indeX = 0;
    mBlocksSend.clear();
    for (auto &mk : mMarkers) {
      float proba = mk->getProba(mWindowIterMax);
      if (proba >= 0.5){//1.0 / (float)mWindowIterMax) {
        mk->enableOn();

        mk->setMarkerId(mIdsCounter[i]); //qr id
        mk->updateIdPair(mIdsCounter[i]);

        //get position
        glm::vec2 pos = mk->getPos();
        MarkerArucoRef m = MarkerAruco::create();
        m->setMarkerId(mIdsCounter[i]);
        m->setPos(pos);

        mBlocksSend.push_back(m);

      } else {
        mk->enableOff();
        mk->setMarkerId(-1);
      }
      i++;
      indeX++;

      int newId = mk->getMarkerId();
    }

    for (auto & mFree : mCurrFree) {
      int itr = mFree->getInc();
      if(itr >= 1){
        glm::vec2 pos =  mFree->getPos();
        float rot = mFree->getRot();
        int id =  mFree->getMarkerId();

        //calcute error
        glm::vec2 newPos = glm::vec2(pos.x/(float)itr, pos.y/(float)itr);

        float newRot =  rot / (float)itr;

        //Coordinate mapping
        float newMapPosX = ofMap(newPos.x, 0, 1280.0, mCoordMapMinX, mCoordMapMaxX);
        float newMapPosY = ofMap(newPos.y, 0, 720.0, mCoordMapMinY, mCoordMapMaxY);

        //ofLog(OF_LOG_NOTICE) <<pos.x<<" "<<pos.y<<" "<<itr<<" "<<newPos.x<<" "<<newPos.y;
        //ofLog(OF_LOG_NOTICE) <<mCoordMapMinX<<" "<<mCoordMapMaxX<<" "<<mGridDim.x<<" "<<newMapPosX;

        //save the files
        MarkerArucoRef m = MarkerAruco::create();
        m->setMarkerId(id);
        m->setPos(glm::vec2(newMapPosX, newMapPosY));
        m->setRot(newRot);
        mBlocksSend.push_back(m);
      }
    }

    // done activation and disactivation
    mTmpBlocks.clear();
    ofLog(OF_LOG_NOTICE) << "done: "<<mId;
  }

}
