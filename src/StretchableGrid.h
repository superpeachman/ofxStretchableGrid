//
//  StretchableGrid.h
//
//
//  Created by Toshiya Momota on 9/12/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxKinect.h"
#include "GridMovers.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"

class StretchableGrid {
public:
    StretchableGrid();
    void update();
    void draw();
    void stop();
    void setRotY(bool flag);
    void setRotX(bool flag);

    void setMode(int _mode);
    void setAlphaEffect(bool _alpha);
    void setLightEnable(bool _light);
    void setLightDepth(float _depth);
    void setObjectDepth(float &_depth);
    void setDamp(float &_damp);
    void setInitStrength(float &_initStrength);
    void setAlphaEffectRise(float &_alphaEffectRise);
    void setMinDis(int &_min);
    void setMaxDis(int &_max);
    void toggleDebug();
    
    void setPyrScale(float &_pyrScale);
    void setLevels(int &_levels);
    void setWinSize(int &_winSize);
    void setIterations(int &_iterations);
    void setPolyN(int &_polyN);
    void setPolySigma(float &_polySigma);
    void setFlowScale(float &_flowScale);
    void setDrawFlow(bool &_drawFlow);

    void setRemoveGrid();

private:
    /*--------------------------------------------------------------
     
     
     GUI
     
     
     --------------------------------------------------------------*/
    ofxPanel gui;
    ofxSlider<int> drawMode;
    ofxToggle alphaEffectToggle;
    ofxToggle lightEnable;
    ofxSlider<float> lightDepth;
    ofxSlider<float> objectDepth;
    ofxSlider<float> DAMPING;
    ofxSlider<float> initStrength;
    ofxSlider<float> alphaEffectRise;
    ofxSlider<int> minThreathold;
    ofxSlider<int> maxThreathold;
    
    ofxSlider<float> pyrScale;
    ofxSlider<float> polySigma;
    ofxSlider<float> flowScale;
    ofxSlider<int> levels;
    ofxSlider<int> winsize;
    ofxSlider<int> iterations;
    ofxSlider<int> polyN;
    ofxToggle drawFlow;
    
    bool debugFlag = true;

    /*--------------------------------------------------------------
     
     
     KINECT
     
     
     --------------------------------------------------------------*/
    ofxKinect kinect;
    
    int wscreen=640;
    int hscreen=480;
    
    float zoomF =0.8f;
    float rotX = ofDegToRad(0);
    float rotY = ofDegToRad(0);
    
    float xDis = 8;// Cell size of X grid
    //float xDis = 20;// Cell size of X grid
    float yDis = 8;// Cell size of Y grid
    //float yDis = 20;// Cell size of Y grid
    int xNum = wscreen / xDis;// Number of X grid
    int yNum = hscreen / yDis;// Number of Y grid
    
    vector<ofVec3f> initPos;
    vector<GridMovers> gridMovers;

    ofMesh gridMesh;
    ofImage meshImage;
    ofPixels depthImages;
    ofxCvGrayscaleImage monoImage;
    ofLight light;
    float colorHue;
    ofColor lightColor;
    
    ofVec3f depthV = *new ofVec3f(0,0,0);

    ofxCv::FlowFarneback flow;
    ofVec2f flowForce;
    
    /*
     
     KINECT DISTANCE
     
     */    
    bool removeGridFlag = false;
    vector<ofVec3f> explosionVec;
    
    
    /*
     
     EASING
     
     */
    float initTime = 0;
    float now = 0;
    float endTime = 0;
    float speed = 6;
    float currentVal = 0;
    std::vector<std::string> easingNames;
 
    float explosionAngle = 0;

    //for the triangle mesh
    void setNormals( ofMesh &mesh );
    

};
