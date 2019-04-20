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
  mWindowIterMax = 4; ///
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

//-----------------------------------------------------------------------------
void GridDetector::generateMarkers(std::vector<int> &ids,
                                   std::vector<QRBlockRef> &blocks, bool sort) {

  //copy identified ids
  mTagsIds = ids;

  //copy positions of the qr blokcs
  mCurrBlock = blocks;

  // clasification of ids and blocks
  if (sort) {
    std::sort(mCurrBlock.begin(), mCurrBlock.end(),
              [](const QRBlockRef &lhs, const QRBlockRef &rhs) -> bool {
                return lhs->getPos().x < rhs->getPos().x;
              });
    ofLog(OF_LOG_NOTICE) << "sorted";
  }

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
void GridDetector::drawRotation(){
  //draw rotation of the marker
  for (auto & detectedMk : mCurrBlock) {
    detectedMk->calculateRotation();
    glm::vec2 corner = detectedMk->getFirstCorner();
    glm::vec2 pos = detectedMk->getPos();

    ofSetColor(255);
    ofDrawLine(corner, pos);

    float angle = detectedMk->getRot();

    float px = cos(angle * TWO_PI) * 20 + pos.x;
    float py = sin(angle * TWO_PI) * 20 + pos.y;

    ofDrawLine(pos.x, pos.y, px, py);
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
void GridDetector::recordGrid() {
  if (mRecordOnce) {
    // ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " " << mMarkers.size();
    if (mCurrBlock.size() == mMaxMarkers) {
      ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " markes = " << mMaxMarkers;

      ofLog(OF_LOG_NOTICE) << "Detect";
      // set ids
      mFullIds.clear();
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk->getPos();
        int k = 0;
        for (auto &cblock : mCurrBlock) {
          glm::vec2 cenPos = cblock->getPos();
          float dis = glm::fastDistance(cenPos, pos);
          int arucoID = cblock->getMarkerId();

          //if the marker is inside the a distance or if the marker is a free unit then
          if ((dis >= 0.0 && dis <= mRadDetection) || (arucoID == 37 || arucoID == 38 || arucoID == 39) ) {
            mk->setMarkerId(mTagsIds.at(k));

            // got ids/
            mFullIds.push_back(mTagsIds.at(k));
            break;
          }
          k++;
        }
      }

      ofLog(OF_LOG_NOTICE) << "Done update fullids";
      ofLog(OF_LOG_NOTICE) << "Num Uniques: " << mFullIds.size();
      ofLog(OF_LOG_NOTICE) << "Done Recording";

      mRecordOnce = false;
    }
  }
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

    mCurrFree.clear();
    mCurrFree.resize(MAX_MARKERS);

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
        }
      }
    } //done calculating probabilty


    // ofLog(OF_LOG_NOTICE) << "Update";

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


        //set position, get rotation
        int id        = mIdsCounter[i];
      //  glm::vec2 pos = mCenterCounter[i];
      //  float rot     = mRotCounter[i];

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

        glm::vec2 newPos = glm::vec2(pos.x/(float)itr, pos.y/(float)itr);
        float newRot =  rot / (float)itr;

        MarkerArucoRef m = MarkerAruco::create();
        mFree->setMarkerId(id);
        mFree->setPos(newPos);
        mFree->setRot(newRot);

        mBlocksSend.push_back(m);
      }
    }

    // done activation and disactivation
    mTmpBlocks.clear();
    ofLog(OF_LOG_NOTICE) << "done";
  }

}
