#include "MarkerAruco.h"


//------------------------------------------------------------------------------
void MarkerAruco::makePairIdType(int id, int type) {
  mIdType = std::make_pair(id, type);
}
//------------------------------------------------------------------------------
void MarkerAruco::updateTypePair(int type) { mIdType.second = type; }
void MarkerAruco::updateIdPair(int id) { mIdType.first = id; }
std::pair<int, int> MarkerAruco::getIdTypePair() { return mIdType; }
