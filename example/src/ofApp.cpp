#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    /*--------------------------------------------------------------
     
     
     BASE
     
     
     --------------------------------------------------------------*/
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    ofEnableAlphaBlending();
    
    stretchableGrid = new StretchableGrid();
}
//--------------------------------------------------------------
void ofApp::update(){
    stretchableGrid->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    stretchableGrid->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //FULL SCREEN --------------------------------------------------------------
    if(key == OF_KEY_ALT){
        ofToggleFullscreen();
    }
    
    switch(key){
        case OF_KEY_LEFT:
            stretchableGrid->setRotY(true);
            break;
        case OF_KEY_RIGHT:
            stretchableGrid->setRotY(false);
            break;
        case OF_KEY_UP:
            stretchableGrid->setRotX(true);
            break;
        case OF_KEY_DOWN:
            stretchableGrid->setRotX(false);
            break;
    }
    
    if(key == 'q'){
        stretchableGrid->setRemoveGrid();
    }else if(key == 'w'){
    }else if(key == 'e'){
        stretchableGrid->toggleDebug();
    }else if(key == 'r'){
    }else if(key == 't'){
    }else if(key == '6'){
        stretchableGrid->setMode(1);
    }else if(key == '7'){
        stretchableGrid->setMode(2);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    stretchableGrid->stop();
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
