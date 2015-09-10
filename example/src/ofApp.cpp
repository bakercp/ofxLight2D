// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"


void ofApp::setup()
{
//    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    noiseIndex = 0;
    noiseStep = 0.1;

    makeLights();
    makeShapes();

	rotatingLight = std::make_shared<ofx::Light2D>();
    rotatingLight->setPosition(ofVec3f(2.0f * ofGetWidth() / 3, 2.0f * ofGetHeight() / 3));
    rotatingLight->setViewAngle(ofDegToRad(120));
    lightSystem.add(rotatingLight);

    for (int i = 0; i < 3; ++i)
    {
        ofx::Light2D::SharedPtr light = std::make_shared<ofx::Light2D>();

        ofVec3f position(ofRandomWidth(), ofRandomHeight(), 0);
        ofFloatColor color(ofRandomuf(), ofRandomuf(), ofRandomuf(), 1);

        float radius = ofRandom(300, 1000);
        
        float viewAngle = (ofRandom(1) > 0.5) ? TWO_PI : (ofRandom(PI/4, PI/3));

        float angle = ofRandom(TWO_PI);

        light->setAngle(angle);
        light->setViewAngle(viewAngle);
        light->setPosition(position);
        light->setRadius(radius);
        light->setColor(color);
        light->setPosition(position);
        lightSystem.add(light);

        wanderingLights.push_back(light);
    }
}


void ofApp::update()
{
//    for (std::size_t i = 0; i < wanderingLights.size(); ++i)
//    {
//        Light2D::SharedPtr p = wanderingLights[i];
//
//        ofVec3f currentPostion = p->getPosition();
//
//        ofVec3f newPosition = currentPostion;
//
//        newPosition.x += ofSignedNoise(noiseIndex);
//        newPosition.y += ofSignedNoise(noiseIndex + i);
//
//        float angle = currentPostion.angle(newPosition);
//
//        p->setAngle(angle);
//        p->setPosition(newPosition);
//    }

    noiseIndex += noiseStep;

    rotatingLight->setAngle(ofWrapRadians(rotatingLight->getAngle() + (PI / 360.0f)));

    rotatingLight->setPosition(ofVec3f(ofGetMouseX(),
                                       ofGetMouseY(),
                                       rotatingLight->getPosition().z));
}


void ofApp::draw()
{
    ofBackgroundGradient(ofColor(10), ofColor(0));
}


void ofApp::mousePressed(int x, int y)
{
}


void ofApp::keyPressed(int key)
{
    if (key == ' ')
    {
        ofToggleFullscreen();
    }
    else if (key == 'l')
    {
        makeLights();
    }
    else if (key == 's')
    {
        makeShapes();
    }
    else if (key == 'c')
    {
        lightSystem.clearLights();
        lightSystem.clearShapes();
    }
}


void ofApp::makeLights()
{
    for (int i = 0; i < 2; ++i)
    {
        ofx::Light2D::SharedPtr light = std::make_shared<ofx::Light2D>();

        ofVec3f position(ofRandomWidth(), ofRandomHeight(), 0);
        ofFloatColor color(ofRandomuf(), ofRandomuf(), ofRandomuf(), 1);

        float radius = ofRandom(300, 1000);

        light->setPosition(position);
        light->setRadius(radius);
        light->setColor(color);


        lightSystem.add(light);
    }
}


void ofApp::makeShapes()
{
    for (int i = 0; i < 4; ++i)
    {
        ofx::Shape2D::SharedPtr shape = std::make_shared<ofx::Shape2D>();

        ofRectangle rect;
        rect.setFromCenter(ofRandomWidth(),
                           ofRandomHeight(),
                           ofRandom(10, 20),
                           ofRandom(10, 20));

        shape->setShape(ofPolyline::fromRectangle(rect));
        lightSystem.add(shape);
    }
}

