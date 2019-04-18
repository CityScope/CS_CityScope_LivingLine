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

// addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

#include "QRBlock.h"
#include "CommonTypes.h"
#include "QRDetector.h"
#include "MarkerAruco.h"
#include "GridImage.h"

#define RAD_DETECTION 38
#define MOUSE_RADIUS  17

//based on the numbers of Markers
#define MAX_MARKERS   50

class GridDetector;
typedef std::shared_ptr<GridDetector> GridDetectorRef;

class GridDetector{
public:

    GridDetector(glm::vec2 dim);

    static GridDetectorRef create(glm::vec2 dim) {
        return std::make_shared<GridDetector>(dim);
    }

    glm::vec2 getDim(){return mGridDim;}

    void setId(int id);
    int getId(){return mId;}

    int getNumMarkers(){return mNumMarkers;}

    void setMaxMarkers(int max);
    int getMaxMarkers(){return mMaxMarkers;}

    void setupGridJsonPos(std::string filePos);
    void setupBlocks();

    void generateMarkers(std::vector<int> & ids, std::vector<QRBlockRef> & blocks, bool sort = false );
    void clasification();
    void updateBlockTypes();

    void drawMarkers( );

    //update grid positions
    void toogleDebugGrid(){mDebugGrid =!mDebugGrid;}
    void toogleUpdateGrid(){mUpdateGrid = !mUpdateGrid;}

    void gridPosIdInc();
    void gridPosIdDec();

    void calibrateGrid();
    void recordGrid();

    void setupCleaner();
    void updateCleaner();
    void resetCleaner();
    void cleanGrid();
    bool isDoneCleaner(){return mCleanDone;}

    void setGridPos(glm::vec2 mousePos);

    //save json files
    void saveGridJson();
    void saveGridJson(std::string fileName);

    //draw detected grid
    void drawDetectedGrid(float posx, float posy, float size = 20, float space = 5);
    void drawDetectedGridIn(float posx, float posy, float size = 20, float space = 5);
    void drawDetectedBlock(float posx, float posy, float size = 20, float space = 5);

    void drawBlock(float posx, float posy, float size = 20, float space = 5);

    std::string getUDPMsg(){return mUDPMsgIds;}
    std::vector< std::string > getUDPMsgVector(){return mUDPStrIds;}
    std::vector< std::vector<int> > getUDPVector(){return mUDPVecIds;}

    std::string getUPDNumTypes(){return mUDPNumTags;}

    void drawRotation();

private:

    //tags
    std::string mUDPMsgIds;

    //tags
    std::vector< std::string > mUDPStrIds;

    //UDP Vec
    std::vector< std::vector<int> > mUDPVecIds;

    //tags num tags
    std::string mUDPNumTags;

    //dimentions
    glm::vec2  mGridDim;

    //id;
    int mId;

    //max markers possible
    int mMaxMarkers;

    //number of markers detected
    int mNumMarkers;

    //debug grid
    bool mDebugGrid;

    //update grid
    bool mUpdateGrid;
    int mCurrentGridId;

    bool mRecordOnce;

    //clearner
    int mWindowIterMax;
    int mWindowCounter;

    bool mCleanDone;

    ///radar
    int mNumRL;
    int mNumRM;
    int mNumRS;

    int mNumOL;
    int mNumOM;
    int mNumOS;
    int mNumPark;

    //movable part of fixed
    // Free piece does not take into account
    bool mFreePiece;

    //radius of detection
    float mRadDetection;

    //calibrate grid
    bool mCalibrateGrid;

    std::map<int, int> mIdsCounter;
    std::map<int, int> mProCounter;
    std::map<int, int> mCenterCounter;
    std::map<int, int> mRotCounter;

    // check if found marker
    // point that we analyze if there was a detection or not
    std::vector<MarkerArucoRef> mMarkers;

    //obtain the QRCode from the block
    std::vector<QRBlockRef> mCurrBlock;

    //temporal vector that we clean the current blocks
    std::vector<std::vector<QRBlockRef>> mTmpBlocks;

    std::vector<QRBlockRef> mBlocks;
    std::vector<int> mTagsIds;
    std::vector<int> mFullIds;

};
