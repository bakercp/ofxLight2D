// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
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
    ofSetFrameRate(60);

    noiseIndex = 0;
    noiseStep = 0.1;

    makeLights();
    makeShapes();

	rotatingLight = std::make_shared<ofx::Light2D>();

    rotatingLight->setPosition(glm::vec3(2.0f * ofGetWidth() / 3,
                                         2.0f * ofGetHeight() / 3,
                                         0));

    rotatingLight->setViewAngle(ofDegToRad(120));
    lightSystem.add(rotatingLight);

    for (int i = 0; i < 10; ++i)
    {
        auto light = std::make_shared<ofx::Light2D>();

        glm::vec3 position(ofRandomWidth(), ofRandomHeight(), 0);
        ofFloatColor color(ofRandomuf(), ofRandomuf(), ofRandomuf(), 1);

        float radius = ofRandom(300, 1000);
        
        float viewAngle = (ofRandom(1) > 0.5) ? glm::two_pi<float>() : (ofRandom(glm::quarter_pi<float>(),
                                                                                 glm::pi<float>() * glm::third<float>()));

        float angle = ofRandom(glm::two_pi<float>());

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
    for (std::size_t i = 0; i < wanderingLights.size(); ++i)
    {
		auto p = wanderingLights[i];

        auto currentPostion = p->getPosition();

        auto newPosition = currentPostion;

        newPosition.x += 2 * ofSignedNoise(noiseIndex);
        newPosition.y += 2 * ofSignedNoise(noiseIndex + i);

        float angle = glm::angle(currentPostion, newPosition);

        p->setAngle(angle);
        p->setPosition(newPosition);
    }

    noiseIndex += noiseStep;

    rotatingLight->setAngle(ofWrapRadians(rotatingLight->getAngle() + (glm::pi<float>() / 360.0f)));

    rotatingLight->setPosition(glm::vec3(ofGetMouseX(),
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
        auto light = std::make_shared<ofx::Light2D>();

        glm::vec3 position(ofRandomWidth(), ofRandomHeight(), 0);
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
        auto shape = std::make_shared<ofx::Shape2D>();

        ofRectangle rect;
        rect.setFromCenter(ofRandomWidth(),
                           ofRandomHeight(),
                           ofRandom(10, 20),
                           ofRandom(10, 20));

        shape->setShape(ofPolyline::fromRectangle(rect));
        lightSystem.add(shape);
    }
}

