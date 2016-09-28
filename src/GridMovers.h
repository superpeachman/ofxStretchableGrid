//
//  GridMovers.h
//
//
//  Created by Toshiya Momota on 9/12/16.
//
//

#pragma once
#include "ofMain.h"

class GridMovers {
public:
    GridMovers();
    void setInitPos(ofVec3f &_initPos);
    void update();
    void draw();
    void applyForce(ofVec3f force);
    ofVec3f getLocation();
    ofVec3f getInitPos();
    void setDamp(float _damp);
    void setClearVel();
    void setDamp0(bool _damp0);

private:
    ofVec3f *initPos;
    float x, y, z;
    
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    float mass = 8;
    
    float damping = 0.90;
    
    ofSpherePrimitive sphere;
    bool damp0 = false;
};
