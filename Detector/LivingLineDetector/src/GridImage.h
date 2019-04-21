#pragma once
/*

Thomas Sanchez Lengeling
March, 2019

Living Line

*/

#include "ofMain.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

#include "gui.h"

class GridImage;
typedef std::shared_ptr<GridImage> GridImageRef;

class GridImage{
public:
    GridImage(glm::vec2 dims);
    ~GridImage();

    static GridImageRef create(glm::vec2 dims) {
        return std::make_shared<GridImage>(dims);
    }

    //setup video
    void setupCam(int id, int ftps = 30);

    //cam
    ofVideoGrabber    & getCam(){return mCam;}

    int getCamId(){return mCamId;}

    //draw update images
    bool updateImage();
    void drawImage(int x, int y, int w, int h);
    void drawImage(int x, int y);

    ofPixels  & getImgPixels();

    //gamma, alpha an beta correction
    void setGamma(float gm){mGamma = gm;}
    void setAlpha(float alpha){mAlpha = alpha;}
    void setBeta(int beta){mBeta = beta;}

    float getGamma(){return mGamma;}
    float getAlpha(){return mAlpha;}
    float getBeta(){return mBeta;}

    // clean Image
    void adjustGamma(cv::Mat &img);

    void toogleCrop(){mActivateCrop =! mActivateCrop;}
    void toogleCam(){mActivateCam =! mActivateCam;}

    void cropImg(cv::Mat & inputVideo);

    void drawCropImg();
    void drawCropRoi();

    void resetCrop();

    cv::Mat & getCropMat(){return mCropMat;}
    cv::Mat & getPersMat(){return mPerspectiveMat;}

    void setCropUp(glm::vec2 up);
    void setCropDown(glm::vec2 down);

    void setCropDisp(glm::vec2 dis) { mDisp = dis; }

    glm::vec2 getCropUp(){return mCornerUp;}
    glm::vec2 getCropDown(){return mCornerDown;}
    glm::vec2 getCropDisp(){return mDisp;}

    void setId(int id){mId = id;}

    void updateDetectImg(ofImage & img){mVidImg = img;}
    ofImage & getImg(){return mVidImg;}

    ofTexture & getFboTexture(){return mFboResolution.getTexture();}

    //Perspective Transform
    void calculatePerspective(cv::Mat &inputVideo);
    //get cornes for transformation
    void updateCorners(glm::vec2 corner);

    bool isCalculatedPersp(){return mCalculatedPersp;}
    void calculatedPersp(){mCalculatedPersp = true;}

    void threadTimer(int counter);

    glm::vec2 getPerspDim(){return mPerspeDim;}

    void resetPerspetive();

    void setInputPersp(glm::vec2 pos, int index);
    void addInputPersp(glm::vec2 pos, int index);

    glm::vec2 getInputPersp(int index);
private:
    //input stream
    ofVideoGrabber      mCam;
    int                 mFps;
    int                 mCamId;
    ofPixels            mPixs;

    glm::vec2           mDim;
    glm::vec2           mLength;

    int               mCamCounter;
    int               mCamThread;

    //if loaded cam
    bool                mCamLoaded;

    //Resolution of the I
    ofFbo               mFboResolution;

    //input image
    ofImage             mVidImg;

    //acative streaming
    bool mActivateCam;

    //crop of image
    bool      mActivateCrop;
    cv::Rect  mRoi;
    glm::vec2 mCornerUp;
    glm::vec2 mCornerDown;
    glm::vec2 mDisp;

    // Perspective Transformation
    bool        mCalculatedPersp;  //can calculate perspective
    cv::Mat     mPerspectiveMat;

    //perspective points
    cv::Point2f mInputQuad[4];
    int         cornerIndex;
    glm::vec2   mPerspeDim;
    bool        mResetPersp;

    //crop Mat
    cv::Mat   mCropMat;

    //id of the Image
    int mId;

    //img correction
    float mGamma;

    float mAlpha;
    int  mBeta;



};
