/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once

#include "ofMain.h"

#include "CommonTypes.h"
#include "QRBlock.h"
#include "ProbaAruco.h"

class MarkerAruco;
typedef std::shared_ptr<MarkerAruco> MarkerArucoRef;

//--------------------------------------------------------------------
class MarkerAruco : public ProbabiltyAruco{
public:
    MarkerAruco() : ProbabiltyAruco() {
        mGridId = -1;
        mMakerId = -1;
        mRot = 0;
        mBType = BlockType::grid;
        mIdType = std::make_pair(-1, 0);
        mDebugPos = false;
    }

    static MarkerArucoRef create(){
        return std::make_shared<MarkerAruco>();
    }

    //qr code id
    void setMarkerId(int i){mMakerId = i;}
    int getMarkerId(){return mMakerId;}

    //grid id
    void setGridId(int i){mGridId = i;}
    int getGridId(){return mGridId;}

    //grid pos
    void setPos(glm::vec2 mpos){mPos = mpos;}
    glm::vec2 getPos(){return mPos;}

    //rot
    void setRot(float rot){mRot = rot;}
    float getRot(){return mRot;}

    //enable
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
    int        mMakerId; //qr id

    //grid id
    int        mGridId;  //grid id, 0,1,2,4..etc

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

    //rotation
    float mRot;

    //Marker type
    MarkerType mCurrentType;

    //probabily that the marker is detectes
    float      mProb;

    //id correponds the type
    std::pair<int, int> mIdType;

};
