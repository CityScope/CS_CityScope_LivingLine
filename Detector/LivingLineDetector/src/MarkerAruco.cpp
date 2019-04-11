#include "MarkerAruco.h"

//--------------------------------------------------------------
ProbabiltyAruco::ProbabiltyAruco() {
  mProba = 0.0;
  mInc = 0;
}
//--------------------------------------------------------------
void ProbabiltyAruco::incProba() { mInc++; }
float ProbabiltyAruco::getProba(int maxNum) {
  return ((float)mInc / (float)maxNum);
}
//--------------------------------------------------------------
void ProbabiltyAruco::resetProba() {
  mProba = 0.0;
  mInc = 0;
}

//------------------------------------------------------------------------------
void MarkerAruco::makePairIdType(int id, int type) {
  mIdType = std::make_pair(id, type);
}
//------------------------------------------------------------------------------
void MarkerAruco::updateTypePair(int type) { mIdType.second = type; }
void MarkerAruco::updateIdPair(int id) { mIdType.first = id; }
std::pair<int, int> MarkerAruco::getIdTypePair() { return mIdType; }
