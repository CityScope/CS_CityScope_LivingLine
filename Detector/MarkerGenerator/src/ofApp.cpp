#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("Highly Reliable Marker Creator");
	ofEnableAlphaBlending();

	mMakerId = 0;
	mBorderBits = 1;


	mDictionaryTags.push_back("DICT_4X4_50");
	mDictionaryTags.push_back("DICT_4X4_100");
	mDictionaryTags.push_back("DICT_4X4_250");
	mDictionaryTags.push_back("DICT_4X4_1000");
	mDictionaryTags.push_back("DICT_5X5_50");
	mDictionaryTags.push_back("DICT_5X5_100");
	mDictionaryTags.push_back("DICT_5X5_250");
	mDictionaryTags.push_back("DICT_5X5_1000");
	mDictionaryTags.push_back("DICT_6X6_50");
	mDictionaryTags.push_back("DICT_6X6_100");
	mDictionaryTags.push_back("DICT_6X6_250");
	mDictionaryTags.push_back("DICT_6X6_1000");
	mDictionaryTags.push_back("DICT_7X7_50");
	mDictionaryTags.push_back("DICT_7X7_100");
	mDictionaryTags.push_back("DICT_7X7_250");
	mDictionaryTags.push_back("DICT_7X7_1000");

	mDictionaryMax.push_back(50);
	mDictionaryMax.push_back(100);
	mDictionaryMax.push_back(250);
	mDictionaryMax.push_back(1000);
	mDictionaryMax.push_back(50);
	mDictionaryMax.push_back(100);
	mDictionaryMax.push_back(250);
	mDictionaryMax.push_back(1000);
	mDictionaryMax.push_back(50);
	mDictionaryMax.push_back(100);
	mDictionaryMax.push_back(250);
	mDictionaryMax.push_back(1000);
	mDictionaryMax.push_back(50);
	mDictionaryMax.push_back(100);
	mDictionaryMax.push_back(250);
	mDictionaryMax.push_back(1000);

	mVidImg = createMarker(mDictionaryId, mMakerId, mBorderBits);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(200);
	{
		if(mGenerateTags){
			for(int i = 0; i < mDictionaryMax.at(mDictionaryId); i++){
				ofImage img = createMarker(mDictionaryId, i, mBorderBits);
				mVidImg.save(mDictionaryTags.at(mDictionaryId)+"/"+to_string(mDictionaryId)+"_"+to_string(i)+".png");
			}
			mGenerateTags = false;
			std::cout<<"Done Generating "<<mDictionaryTags.at(mDictionaryId)<<std::endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(30);
	ofDrawBitmapString("Id: "+to_string(mMakerId), 15, 15);
	ofDrawBitmapString("Dictionary Id: "+to_string(mDictionaryId) +" "+mDictionaryTags.at(mDictionaryId), 15, 35);
	ofDrawBitmapString("Custom Id: "+to_string(CUSTOM_DIC), 300, 35);

	ofSetColor(255);
	mVidImg.draw(50, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == '1'){
		mMakerId++;
		if(mMakerId >= mDictionaryMax.at(mDictionaryId)){
			mMakerId =49;
		}
		mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}
	if(key == '2'){
		mMakerId--;
		if(mMakerId == -1){
			mMakerId = 0;
		}
		mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}

	if(key == 'q'){
		mBorderBits++;
		if(mBorderBits >=  5){
			mBorderBits =5;
		}
		mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}

	if(key == 'w'){
		mBorderBits--;
		if(mBorderBits <= 1){
			mBorderBits =1;
		}
		mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}

	if(key == 'a'){
		mDictionaryId++;
		if(mDictionaryId >=  15){
			mDictionaryId = 15;
		}
		mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}

	if(key == 's'){
		mDictionaryId--;
		if(mDictionaryId <= 0){
			mDictionaryId =0;
		}
		 mVidImg =	createMarker(mDictionaryId, mMakerId, mBorderBits);
	}


	if(key == 'p'){
		mVidImg.save(mDictionaryTags.at(mDictionaryId)+"_"+to_string(mMakerId)+".png");
	}

	if(key == 'g'){
		mGenerateTags = true;
	}

	if(key == 'p'){
		mVidImg.save(mDictionaryTags.at(mDictionaryId)+"_"+to_string(mMakerId)+".png");
	}

	if(key == 'd'){
		mVidImg = createCustomMarker(mMakerId);
		std::cout<<"generated custom markers"<<std::endl;
	}

	if(key == 'f'){
		mVidImg = createCustomMarker(mMakerId);
		std::cout<<"generated custom markers"<<std::endl;
	}

	if(key == 'c'){
			for(int i = 0; i < CUSTOM_DIC; i++){
				ofImage img = createCustomMarker(i);
				img.save("cus_"+to_string(CUSTOM_DIC)+"/"+to_string(mDictionaryId)+"_"+to_string(i)+".png");
			}
			mGenerateTags = false;
			std::cout<<"Done Custom Generating "<<CUSTOM_DIC<<std::endl;

	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
ofImage ofApp::createCustomMarker(int id)
{
	int markerId =  id;
	int borderBits = 1;
	int markerSize = 400;
	bool showImage = false;

	cv::Ptr<cv::aruco::Dictionary> dic = cv::aruco::Dictionary::create(CUSTOM_DIC, 4);

	cv::Mat markerImg;
	cv::Mat outputMat;
	ofImage img;

	cv::aruco::drawMarker(dic, markerId, markerSize, markerImg, borderBits);

	// create video output
  markerImg.copyTo(outputMat);
  ofxCv::toOf(outputMat, img.getPixels());
  img.update();
	return img;
}

//--------------------------------------------------------------
ofImage ofApp::createMarker(int dicId, int id, int bBits)
{


	/*
	DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL =
		*/

	int dictionaryId = dicId;
	int markerId =  id;
	int borderBits = bBits;
	int markerSize = 400;
	bool showImage = false;

	cv::Ptr<cv::aruco::Dictionary> dic =
			cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

	cv::Mat markerImg;
	cv::Mat outputMat;
	ofImage img;

	cv::aruco::drawMarker(dic, markerId, markerSize, markerImg, borderBits);

	// create video output
  markerImg.copyTo(outputMat);
  ofxCv::toOf(outputMat, img.getPixels());
  img.update();
	return img;
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
