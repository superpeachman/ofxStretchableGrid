//
//  GridMovers.cpp
//
//
//  Created by Toshiya Momota on 9/12/16.
//
//

#include "GridMovers.h"

GridMovers::GridMovers(){
    location = *new ofVec3f(0,0,0);
    velocity = *new ofVec3f(0,0,0);
    acceleration = *new ofVec3f(0,0,0);
}
void GridMovers::setInitPos(ofVec3f &_initPos){
    initPos = &_initPos;
    location = *initPos;
    //cout << "_initPos.x:" << _initPos.x << " _initPos.y:" << _initPos.y << " _initPos.z:" << _initPos.z << "\n";
}

// Standard Euler integration
void GridMovers::update() {
    velocity += acceleration;
    if(damp0 != true) velocity *= damping;
    location += velocity;
    acceleration *= 0;
}

// Newton's law: F = M * A
void GridMovers::applyForce(ofVec3f force) {
    acceleration += force/mass;
}
ofVec3f GridMovers::getLocation() {
    return location;
}
ofVec3f GridMovers::getInitPos(){
    return *initPos;
}
void GridMovers::setDamp(float _damp){
    damping = _damp;
    //cout << "damping:" << damping << "\n";
}
void GridMovers::setClearVel(){
    velocity *= 0;
    //cout << "damping:" << damping << "\n";
}
void GridMovers::setDamp0(bool _damp0){
    damp0 = _damp0;
    //cout << "damping:" << damping << "\n";
}
void GridMovers::draw() {
    ofPushMatrix();
    ofTranslate(location.x, location.y, location.z);
    sphere.setRadius(mass);
    sphere.draw();
    ofPopMatrix();
}

