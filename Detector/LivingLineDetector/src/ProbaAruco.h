#pragma once

//------------------------------------------------------------------------------
class ProbabiltyAruco{
public:
    ProbabiltyAruco();

    void incProba();
    float getProba(int num);
    void resetProba();

    int getInc(){return mInc;}
private:
    float mProba;
    int   mInc;
};

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
