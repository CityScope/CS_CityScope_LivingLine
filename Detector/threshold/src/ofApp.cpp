#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

void ofApp::setup() {
	cam.setVerbose(true);
	cam.setup(1280, 720);
	cam.setDeviceID(0);
	thresh.allocate(1280, 720, OF_IMAGE_GRAYSCALE);

	int beta =0;
	alpha =1.0;
}

//------------------------------------------------------------
void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		convertColor(cam, thresh, CV_RGB2GRAY);

		cv::Mat mMatSat;
		cv::Mat input = ofxCv::toCv(cam.getPixels());
		//make the brigthness adjusments

/*
		for( int y = 0; y < input.rows; y++ ) {
			for( int x = 0; x < input.cols; x++ ) {
				for( int c = 0; c < input.channels(); c++ ) {
						input.at<Vec3b>(y,x)[c] =
							saturate_cast<uchar>( alpha*input.at<Vec3b>(y,x)[c] + beta );
				}
			}
		}
*/
		input.convertTo(mMatSat, -1, alpha, beta);

		//input.copyTo(mMatSat);
		ofxCv::toOf(mMatSat, imgCut.getPixels());
		imgCut.update();


		if(ofGetMousePressed()) {
			autothreshold(thresh);
		} else {
			float thresholdValue = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
			std::cout<< thresholdValue <<std::endl;
			threshold(thresh, thresholdValue);
		}
		thresh.update();
	}
}

//------------------------------------------------------------
void ofApp::draw() {
	cam.draw(0, 0, 640, 480);
	thresh.draw(640, 0, 640, 480);




	imgCut.draw(640*2, 0, 640, 480);
}

//------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'a'){
		if(alpha < 5.0){
			alpha+=0.01;
		}
		cout<<"alpha: "<<alpha<<std::endl;
	}

	if(key == 's'){
		if(alpha > -1.0){
			alpha -= 0.01;
		}
		cout<<"alpha: "<<alpha<<std::endl;
	}

	if(key == 'z'){
		if(beta < 255){
			beta += 1;
		}
		cout<<"beta: "<<beta<<std::endl;
	}

	if(key == 'x'){
		if(beta > -50){
			beta -= 1;
		}
		cout<<"beta: "<<beta<<std::endl;
	}

}
//------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}


//------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}


//------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}
