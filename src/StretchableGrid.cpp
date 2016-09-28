//
//  StretchableGrid.cpp
//
//
//  Created by Toshiya Momota on 9/12/16.
//
//

#include "StretchableGrid.h"
#include "ofxEasing.h"

using namespace ofxCv;
using namespace cv;

StretchableGrid::StretchableGrid(){
    /*--------------------------------------------------------------
     
     
     GUI
     
     
     --------------------------------------------------------------*/
    gui.setup();
    gui.add(drawMode.setup("MODE", 0, 0, 2));
    gui.add(alphaEffectToggle.setup("ALPHA EFFECT", true));
    gui.add(lightEnable.setup("LIGHT ENABLE", false));
    gui.add(lightDepth.setup("LIGHT DEPTH", 300, 0, 1000));
    gui.add(DAMPING.setup("DAMPING", 0.90f, 0.01f, 0.99f));
    gui.add(initStrength.setup("INIT STRENGTH", 0.95f, 0.01f, 0.99f));
    gui.add(alphaEffectRise.setup("ALPHA EFFECT RISE", 1.8f, 1.f, 3.f));
    gui.add(objectDepth.setup("OBJECT DEPTH", 0, -2000, 2000));
    gui.add(minThreathold.setup("MIN DISTANCE", 1000, 1000, 5000));
    gui.add(maxThreathold.setup("MAX DISTANCE", 200, 0, 1000));
    gui.add(pyrScale.setup("pyrScale", 0.3, 0, 1));
    gui.add(levels.setup("levels", 2, 1, 8));
    gui.add(winsize.setup("winsize", 10, 4, 64));
    gui.add(iterations.setup("iterations", 2, 1, 8));
    gui.add(polyN.setup("polyN", 5, 5, 10));
    gui.add(polySigma.setup("polySigma", 1.1, 1.1, 2));
    gui.add(flowScale.setup("flowScale", 0.2, 0.0, 0.2));
    gui.add(drawFlow.setup("draw opticalflow", false));
    
    DAMPING.addListener(this, &StretchableGrid::setDamp);
    initStrength.addListener(this, &StretchableGrid::setInitStrength);
    minThreathold.addListener(this, &StretchableGrid::setMinDis);
    maxThreathold.addListener(this, &StretchableGrid::setMaxDis);
    pyrScale.addListener(this, &StretchableGrid::setPyrScale);
    levels.addListener(this, &StretchableGrid::setLevels);
    winsize.addListener(this, &StretchableGrid::setWinSize);
    iterations.addListener(this, &StretchableGrid::setIterations);
    polyN.addListener(this, &StretchableGrid::setPolyN);
    polySigma.addListener(this, &StretchableGrid::setPolySigma);
    flowScale.addListener(this, &StretchableGrid::setFlowScale);
    drawFlow.addListener(this, &StretchableGrid::setDrawFlow);
    
    /* ---------------------
     
     KINECT
     
     --------------------- */
    
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    /* ---------------------
     
     GRID POSITION
     
     --------------------- */
    initPos.resize(xNum * yNum);
    gridMovers.resize(xNum * yNum);
    explosionVec.resize(xNum * yNum);

    for (int i = 0; i < initPos.size(); i++) {
        auto x = (i % xNum) * xDis - (float)wscreen/(float)2;
        auto y = floor((float)i / (float)xNum) * yDis - (float)hscreen/(float)2;
        initPos[i] = *new ofVec3f(x, y, 0);
        //cout << "x:" << x << " y:" << y << "\n";

    }
    
    // Create objects at starting location
    // Note third argument in Spring constructor is "rest length"
    for (int i = 0; i < initPos.size(); i++) {
            gridMovers[i] = *new GridMovers();
            gridMovers[i].setInitPos(initPos[i]);
    }
    
    for (int i = 0; i < initPos.size(); i++) {
        if(i < (initPos.size() - xNum)){
            if((i+1) % xNum != 0){
                gridMesh.addVertex(initPos[i]);
                gridMesh.addVertex(initPos[i+1]);
                gridMesh.addVertex(initPos[i+xNum]);
                
                gridMesh.addVertex(initPos[i+1]);
                gridMesh.addVertex(initPos[i+1+xNum]);
                gridMesh.addVertex(initPos[i+xNum]);
                
                //cout << i << "-> x:" << x+(float)wscreen/(float)2 << " y:" << y+(float)hscreen/(float)2 << "\n";
            }else{
                gridMesh.addVertex(initPos[i]);
                gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y,initPos[i].z));
                gridMesh.addVertex(ofVec3f(initPos[i].x,initPos[i].y+yDis,initPos[i].z));
                
                gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y,initPos[i].z));
                gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y+yDis,initPos[i].z));
                gridMesh.addVertex(ofVec3f(initPos[i].x,initPos[i].y+yDis,initPos[i].z));
            }
        }else{
            gridMesh.addVertex(initPos[i]);
            gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y,initPos[i].z));
            gridMesh.addVertex(ofVec3f(initPos[i].x,initPos[i].y+yDis,initPos[i].z));
            
            gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y,initPos[i].z));
            gridMesh.addVertex(ofVec3f(initPos[i].x+xDis,initPos[i].y+yDis,initPos[i].z));
            gridMesh.addVertex(ofVec3f(initPos[i].x,initPos[i].y+yDis,initPos[i].z));
        }
    }
    gridMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    gridMesh.setupIndicesAuto();//Set up indices
    setNormals(gridMesh);
    
    for (int i = 0; i < gridMesh.getNumVertices(); i++) {
        auto x = i % xNum * xDis;
        auto y = floor((float)i / (float)xNum) * yDis;

        gridMesh.addTexCoord(ofPoint(x, y));
        gridMesh.addTexCoord(ofPoint(x+xDis, y));
        gridMesh.addTexCoord(ofPoint(x, y+yDis));
        
        gridMesh.addTexCoord(ofPoint(x+xDis, y));
        gridMesh.addTexCoord(ofPoint(x+xDis, y+yDis));
        gridMesh.addTexCoord(ofPoint(x, y+yDis));
        
        gridMesh.addColor(ofFloatColor(1.0));
        gridMesh.addColor(ofFloatColor(1.0));
        gridMesh.addColor(ofFloatColor(1.0));
        
        gridMesh.addColor(ofFloatColor(1.0));
        gridMesh.addColor(ofFloatColor(1.0));
        gridMesh.addColor(ofFloatColor(1.0));
    }

    meshImage.allocate(wscreen, hscreen, OF_IMAGE_COLOR);
    monoImage.allocate(wscreen, hscreen);
    
    ofSetSmoothLighting(true);
    light.setPointLight();
    //light.setSpotlight();
    
    light.setAmbientColor(ofColor(0.f, 0.f, 0.f));    //Ambient
    light.setDiffuseColor(ofColor(255.f, 0.f, 0.f));    //Around high light
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f));    //Highlight
    
    colorHue = ofRandom(0, 250);
    lightColor.setBrightness(180.f);
    lightColor.setSaturation(150.f);
    
    flow.setPyramidScale(pyrScale);
    flow.setNumLevels(levels);
    flow.setWindowSize(winsize);
    flow.setNumIterations(iterations);
    flow.setPolyN(polyN);
    flow.setPolySigma(polySigma);
    
    easingNames = {
        "linear",
        "quad",
        "cubic",
        "bounce",
        "back",
        "quart",
        "quint",
        "sine",
        "elastic",
        "circ",
        "easeOutExpo",
    };
    
}

void StretchableGrid::update(){
    kinect.update();
    
    if(kinect.isFrameNewDepth()){
        meshImage.setFromPixels(kinect.getPixels());
        if(drawFlow) monoImage.setFromPixels(meshImage);
        if(drawFlow) flow.calcOpticalFlow(monoImage);
        depthImages.clear();
        depthImages = kinect.getDepthPixels();
        
        for (int i = 0; i < gridMovers.size(); i++) {
            /* -------------
            
            
             depth
             
             
             -------------- */
            auto xPos = i%xNum;
            auto yPos = floor((float)i/(float)xNum);
            //xPos = ofMap(xPos, 0, xNum, 0, wscreen);
            //yPos = ofMap(yPos, 0, yNum, 0, hscreen);
            xPos *= xDis;
            yPos *= yDis;
            
            depthV *= 0;
            
            /* -------------
             
             
             optical flow
             
             
             -------------- */
            flowForce *= 0;
            if(drawFlow) flowForce = flow.getFlowOffset(xPos, yPos) * flowScale;

            /* -------------
             
             
             Integrate
             
             
             -------------- */
            auto mz = (int)depthImages[floor(xPos + yPos * wscreen)];

            //int minThreathold = 3000;
            //int maxThreathold = 200;
            if (minThreathold > mz && mz > maxThreathold) {
                if(removeGridFlag == false) {
                    if(drawFlow) {
                        depthV.set(-flowForce.x, -flowForce.y, mz);
                    }else{
                        depthV.set(0, 0, mz);
                    }
                    gridMovers[i].applyForce(depthV);
                }else{
                    endTime = initTime + speed;
                    now = ofGetElapsedTimef();
                    currentVal = ofxeasing::map_clamp(now, initTime, endTime, 0.f, 1.f, &ofxeasing::exp::easeOut);
                    if(now < endTime){
                        gridMesh.setVertex(i*6, gridMesh.getVertex(i*6)+(gridMesh.getVertex(i*6) - explosionVec[i])*0.1f);
                        gridMesh.setVertex(i*6+1, gridMesh.getVertex(i*6+1)+(gridMesh.getVertex(i*6+1) - explosionVec[i])*0.1f);
                        gridMesh.setVertex(i*6+2, gridMesh.getVertex(i*6+2)+(gridMesh.getVertex(i*6+2) - explosionVec[i])*0.1f);
                        gridMesh.setVertex(i*6+3, gridMesh.getVertex(i*6+3)+(gridMesh.getVertex(i*6+3) - explosionVec[i])*0.1f);
                        gridMesh.setVertex(i*6+4, gridMesh.getVertex(i*6+4)+(gridMesh.getVertex(i*6+4) - explosionVec[i])*0.1f);
                        gridMesh.setVertex(i*6+5, gridMesh.getVertex(i*6+5)+(gridMesh.getVertex(i*6+5) - explosionVec[i])*0.1f);
                    }else{
                        removeGridFlag = false;
                    }
                }
            }
            
            gridMovers[i].applyForce((gridMovers[i].getLocation() - initPos[i]) * -initStrength);
            gridMovers[i].update();
        }
        
        for (int i = 0; i < initPos.size(); i++) {
            if(i < (initPos.size() - xNum)){//
                if((i+1) % xNum != 0){
                    if(removeGridFlag == false) {
                        gridMesh.setVertex(i*6, gridMovers[i].getLocation());
                        gridMesh.setVertex(i*6+1, gridMovers[i+1].getLocation());
                        gridMesh.setVertex(i*6+2, gridMovers[i+xNum].getLocation());
                        
                        gridMesh.setVertex(i*6+3, gridMovers[i+1].getLocation());
                        gridMesh.setVertex(i*6+4, gridMovers[i+1+xNum].getLocation());
                        gridMesh.setVertex(i*6+5, gridMovers[i+xNum].getLocation());
                    }
                    
                    if(alphaEffectToggle){
                        auto depth = gridMesh.getVertex(i*6).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6, ofFloatColor(depth*alphaEffectRise));
                        depth = gridMesh.getVertex(i*6+1).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6+1, ofFloatColor(depth*alphaEffectRise));
                        depth = gridMesh.getVertex(i*6+2).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6+2, ofFloatColor(depth*alphaEffectRise));

                        depth = gridMesh.getVertex(i*6+3).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6+3, ofFloatColor(depth*alphaEffectRise));
                        depth = gridMesh.getVertex(i*6+4).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6+4, ofFloatColor(depth*alphaEffectRise));
                        depth = gridMesh.getVertex(i*6+5).z/(minThreathold-maxThreathold);
                        gridMesh.setColor(i*6+5, ofFloatColor(depth*alphaEffectRise));
                    }else{
                        gridMesh.setColor(i*6, ofFloatColor(1.0));
                        gridMesh.setColor(i*6+1, ofFloatColor(1.0));
                        gridMesh.setColor(i*6+2, ofFloatColor(1.0));
                        
                        gridMesh.setColor(i*6+3, ofFloatColor(1.0));
                        gridMesh.setColor(i*6+4, ofFloatColor(1.0));
                        gridMesh.setColor(i*6+5, ofFloatColor(1.0));
                    }

                }else{// RIGHT LINE
                    gridMesh.setColor(i*6, ofFloatColor(0.0));
                    gridMesh.setColor(i*6+1, ofFloatColor(0.0));
                    gridMesh.setColor(i*6+2, ofFloatColor(0.0));
                    gridMesh.setColor(i*6+3, ofFloatColor(0.0));
                    gridMesh.setColor(i*6+4, ofFloatColor(0.0));
                    gridMesh.setColor(i*6+5, ofFloatColor(0.0));
                }
            }else{
                //LAST LINE
                gridMesh.setColor(i*6, ofFloatColor(0.0));
                gridMesh.setColor(i*6+1, ofFloatColor(0.0));
                gridMesh.setColor(i*6+2, ofFloatColor(0.0));
                gridMesh.setColor(i*6+3, ofFloatColor(0.0));
                gridMesh.setColor(i*6+4, ofFloatColor(0.0));
                gridMesh.setColor(i*6+5, ofFloatColor(0.0));
            }
        }
    }
    
    if(lightEnable){
        light.setPosition(ofGetWidth()*.5f, ofGetHeight()*.5f, lightDepth);
        
        colorHue += .1f;
        if(colorHue >= 255) colorHue = 0.f;
        lightColor.setHue(colorHue);
        light.setDiffuseColor(lightColor);
        light.enable();
    }else{
        ofDisableLighting();
        light.disable();
    }
}

void StretchableGrid::draw(){
    ofPushMatrix();
    ofEnableDepthTest();
    
    //ofSetColor(light.getDiffuseColor());
    if(lightEnable){
        ofEnableLighting();
        if(debugFlag) light.draw();
    }
    
    ofTranslate(ofGetWidth()*.5f, ofGetHeight()*.5f, objectDepth);
    ofRotateX(rotX);
    ofRotateY(rotY);
    //ofScale(zoomF);

    //kinect.draw(wscreen/2 + 10, -hscreen/2, wscreen, hscreen);
    
    ofPushStyle();
    
    switch(drawMode){
        case 0:
            meshImage.bind();
            gridMesh.draw();
            meshImage.unbind();
            break;
        case 1:
            gridMesh.drawWireframe();
            break;
        case 2:
            gridMesh.draw();
            break;
    }
    
    for (int i = 0; i < 50; i++) {
        //gridMovers[i].draw();
    }

    
    
    if(lightEnable) ofDisableLighting();
    
    ofPopStyle();
    ofDisableDepthTest();
    ofPopMatrix();
    
    if(debugFlag) gui.draw();
    if(debugFlag) ofDrawBitmapString(ofToString(ofGetFrameRate())+ "fps" , 0, 15);
}

void StretchableGrid::setRotY(bool flag){
    if(flag){
        rotY += 1;
    }else{
        rotY -= 1;
    }
}

void StretchableGrid::setRotX(bool flag){
    if(flag){
        rotX += 1;
    }else{
        rotX -= 1;
    }
}

void StretchableGrid::setMode(int _mode){
    drawMode=_mode;
}
void StretchableGrid::setAlphaEffect(bool _alpha){
    alphaEffectToggle = _alpha;
}
void StretchableGrid::setLightEnable(bool _light){
    lightEnable = _light;
}
void StretchableGrid::setLightDepth(float _depth){
    lightDepth = _depth;
}
void StretchableGrid::setObjectDepth(float &_depth){
    objectDepth = _depth;
}
void StretchableGrid::setDamp(float &_damp){
    for (int i = 0; i < gridMovers.size(); i++) {
        gridMovers[i].setDamp(_damp);
    }
}
void StretchableGrid::setInitStrength(float &_initStrength){
    initStrength = _initStrength;
}
void StretchableGrid::setAlphaEffectRise(float &_alphaEffectRise){
    alphaEffectRise = _alphaEffectRise;
}
void StretchableGrid::setMinDis(int &_min){
    minThreathold = _min;
    //cout << "minThreathold:" << minThreathold << "\n";
}
void StretchableGrid::setMaxDis(int &_max){
    maxThreathold = _max;
    //cout << "maxThreathold:" << maxThreathold << "\n";
}
void StretchableGrid::toggleDebug(){
    debugFlag = !debugFlag;
}
void StretchableGrid::setPyrScale(float &_pyrScale){
    flow.setPyramidScale(_pyrScale);
}
void StretchableGrid::setLevels(int &_levels){
    flow.setNumLevels(_levels);
}
void StretchableGrid::setWinSize(int &_winSize){
    flow.setWindowSize(_winSize);
}
void StretchableGrid::setIterations(int &_iterations){
    flow.setNumIterations(_iterations);
}
void StretchableGrid::setPolyN(int &_polyN){
    flow.setPolyN(_polyN);
}
void StretchableGrid::setPolySigma(float &_polySigma){
    flow.setPolySigma(_polySigma);
}
void StretchableGrid::setFlowScale(float &_flowScale){
    flowScale = _flowScale;
}
void StretchableGrid::setDrawFlow(bool &_drawFlow){
    drawFlow = _drawFlow;
}
void StretchableGrid::setRemoveGrid(){
    removeGridFlag = !removeGridFlag;
    initTime = ofGetElapsedTimef();
    
    for(int i=0; i<explosionVec.size();i++){
        explosionVec[i] = ofVec3f(ofRandom(-1000, 1000),ofRandom(-1000, 1000),ofRandom(-1000, 1000));
        //explosionVec[i] = ofVec3f(ofRandom(-300, 300),ofRandom(-300, 300),ofRandom(-300, 300));
    }
}
//--------------------------------------------------------------
void StretchableGrid::stop(){
    kinect.close();
}

//--------------------------------------------------------------
void StretchableGrid::setNormals( ofMesh &mesh ){
    
    //The number of the vertices
    int nV = mesh.getNumVertices();
    
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    
    vector<ofPoint> norm( nV );			//Array for the normals
    
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        
        //Compute the triangle's normal
        ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        norm[i].normalize();
    }
    
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals( norm );
}
