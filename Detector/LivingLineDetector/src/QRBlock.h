#pragma once

#include <memory>

#include "ofMain.h"
#include "CommonTypes.h"
#include "ProbaAruco.h"

/*
Class for each  Physical Block
*/

class QRBlock;
typedef std::shared_ptr<QRBlock> QRBlockRef;

//----------------------------------------------------------
class QRBlock : public ProbabiltyAruco{

public:
  QRBlock() : ProbabiltyAruco()  {
    mPos = glm::vec2(0, 0);
    mFirstCorner = glm::vec2(0, 0);
    mRot = 0;
    mEnable = false;
    mIdType = std::make_pair(-1, 0);
  }

  static QRBlockRef create() { return std::make_shared<QRBlock>(); }

  glm::vec2 getPos() { return mPos; }
  void setPos(glm::vec2 p) { mPos = p; }
  void addPos(glm::vec2 p){mPos+=p;}

  glm::vec2 getFirstCorner(){return mFirstCorner;}
  void setFirstCorner(glm::vec2 pos){mFirstCorner = pos;}

  int getMarkerId() { return mIdType.first; }
  void setMarkerId(int id) { mIdType.first = id; }

  int getType() { return mIdType.second; }
  void setType(int type) { mIdType.second = type; }

  void setRot(float rot){mRot = rot;}
  float getRot(){return mRot;}
  void addRot(float rot){mRot += rot;}

  void calculateRotation(){

    glm::vec2 lineBetween  = mPos - mFirstCorner;
    float mRot2 = atan2(lineBetween.x, lineBetween.y) - M_PI/2.0;
    float mapRot = ofMap(mRot2, -M_PI/2, M_PI/2, 0, 360);
    float mapAngle = ofMap(mapRot, -360, 360, 0, 360);

    mRot = mapAngle;

    ofLog(OF_LOG_NOTICE) <<" "<< mRot;
     //float maP = mapAngle + 135;

  }

private:
  glm::vec2 mPos;
  glm::vec2 mFirstCorner;

  //rotation
  float mRot;

  bool mEnable;
  MarkerType mMType;

  //first -> QR Marker type
  //Type -> building, Office, house, etc
  std::pair<int, int> mIdType;
};
///----------------------------------------------------
