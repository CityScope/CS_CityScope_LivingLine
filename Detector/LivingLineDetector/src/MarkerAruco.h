/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once

#include "ofMain.h"

#include "CommonTypes.h"
#include "RectDetector.h"
#include "QRBlock.h"

class MarkerAruco;
class ProbabiltyAruco;

typedef std::shared_ptr<MarkerAruco> MarkerArucoRef;

//------------------------------------------------------------------------------
class ProbabiltyAruco{
public:
    ProbabiltyAruco();

    void incProba();
    float getProba(int num);
    void resetProba();
private:
    float mProba;
    int   mInc;
};

//------------------------------------------------------------------------------
class MarkerAruco : public RectDetector, public ProbabiltyAruco{
public:
    MarkerAruco() : RectDetector(), ProbabiltyAruco() {
        mGridId = -1;
        mMakerId = -1;
        mBType = BlockType::grid;
        mIdType = std::make_pair(-1, 0);
        mDebugPos = false;
    }

    static MarkerArucoRef create(){
        return std::make_shared<MarkerAruco>();
    }

    void setMarkerId(int i){mMakerId = i;}
    int getMarkerId(){return mMakerId;}

    void setGridId(int i){mGridId = i;}
    int getGridId(){return mGridId;}

    void setPos(glm::vec2 mpos){mPos = mpos;}
    glm::vec2 getPos(){return mPos;}

    void enableOn(){mEnable = true;}
    void enableOff(){mEnable = false;}

    bool isEnable(){return mEnable;}

    float getProb(){return mProb;}
    void setProb(float p){mProb = p;}

    void setBlockType(BlockType bt){mBType = bt;}
    int getBlockType(){return mBType;}

    void makePairIdType(int id, int type);
    void updateTypePair(int type);
    void updateIdPair(int id);
    std::pair<int, int> getIdTypePair();

    //debug position
    void enableDebugPos(){mDebugPos = true;}
    void disableDebugPos(){mDebugPos = false;}

    bool isDebugPos(){return mDebugPos;}

private:
    //QR id
    int        mMakerId;

    //grid id
    int        mGridId;

    //orientation of the marker
    int        mOrientation;

    //position in the screen
    glm::vec2  mPos;

    //activate for detection
    bool       mEnable;

    //debg position based on the grid
    bool       mDebugPos;

    //debug mode
    bool       mDebug;

    //block type, RO, RL, RM, OS, etc..
    BlockType  mBType;

    //Marker type
    MarkerType mCurrentType;

    //probabily that the marker is detectes
    float      mProb;

    //id correponds the type
    std::pair<int, int> mIdType;

};
