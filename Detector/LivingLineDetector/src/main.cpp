#include "ofApp.h"
#include "ofMain.h"
#include "config.h"

/*
Thomas Sanchez Lengeling Mapping
March 2019
*/

void prepareSettings();

//========================================================================
int main() {
  ofSetupOpenGL(1920, 1080, OF_WINDOW); // <-------- setup the GL context
  ofSetWindowPosition(0, 0);
  ofRunApp(new ofApp());
}

//========================================================================
void prepareSettings() {
  ofJson js;
  std::string configFile = "config.json";
  ofFile file(configFile);
  if (file.exists()) {
    ofLog(OF_LOG_NOTICE) << " Reading Config File " << configFile;
    file >> js;

    SystemVars::getInstance().width = js["window"]["width"];
    SystemVars::getInstance().height = js["window"]["height"];
    SystemVars::getInstance().projectName = js["deploy"]["location"];

  }
}
