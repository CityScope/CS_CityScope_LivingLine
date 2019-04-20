#pragma once

//------------------------------------------------------------------------------
class ProbabiltyAruco{
public:
    ProbabiltyAruco(){
      mProba = 0.0;
      mInc = 0;
    }

    void incProba(){
      mInc++;
    }

    float getProba(int num){
        return ((float)mInc / (float)num);
    }
    void resetProba(){
      mProba = 0.0;
      mInc = 0;
    }

    int getInc(){
      return mInc;
    }

private:
    float mProba;
    int   mInc;
};
