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


#include "LightSystem2D.h"
#include "ofGraphics.h"
#include "ofImage.h"
#include "ofLog.h"
#include "ofAppRunner.h"
#include "ofEvents.h"


namespace ofx {


LightSystem2D::LightSystem2D()
{
    ofAddListener(ofEvents().setup, this, &LightSystem2D::setup);
    ofAddListener(ofEvents().update, this, &LightSystem2D::update);
    ofAddListener(ofEvents().draw, this, &LightSystem2D::draw);
    ofAddListener(ofEvents().windowResized, this, &LightSystem2D::windowResized);
}


LightSystem2D::~LightSystem2D()
{
    ofRemoveListener(ofEvents().setup, this, &LightSystem2D::setup);
    ofRemoveListener(ofEvents().update, this, &LightSystem2D::update);
    ofRemoveListener(ofEvents().draw, this, &LightSystem2D::draw);
    ofRemoveListener(ofEvents().windowResized, this, &LightSystem2D::windowResized);
}


void LightSystem2D::setup(ofEventArgs& args)
{
    ofResizeEventArgs resize(ofGetWidth(), ofGetHeight());
    windowResized(resize);
}


void LightSystem2D::update(ofEventArgs& args)
{
    Light2D::List::const_iterator lightIter = _lights.begin();

    while (lightIter != _lights.end())
    {
        (*lightIter)->update();
        ++lightIter;
    }

    Shape2D::List::const_iterator shapeIter = _shapes.begin();

    while (shapeIter != _shapes.end())
    {
        (*shapeIter)->update();
        ++shapeIter;
    }
}


void LightSystem2D::draw(ofEventArgs& args)
{
    _sceneComp.begin();
    ofClear(0, 0, 0, 0);
    _sceneComp.end();

    Light2D::List::const_iterator lightIter = _lights.begin();

    while (lightIter != _lights.end())
    {
        Shape2D::List::const_iterator shapeIter = _shapes.begin();

        _lightComp.begin();
        ofClear(0, 0, 0, 0);

        (*lightIter)->draw();

        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        while (shapeIter != _shapes.end())
        {
            ofMesh mesh;
            makeMask(*lightIter, *shapeIter, mesh);
            mesh.draw();
            ++shapeIter;
        }
        ofPopStyle();

        _lightComp.end();

        _sceneComp.begin();
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        _lightComp.draw(0, 0);
        ofPopStyle();
        _sceneComp.end();

        ++lightIter;
    }

    _sceneComp.begin();

    Shape2D::List::const_iterator shapeIter = _shapes.begin();

    while (shapeIter != _shapes.end())
    {
        (*shapeIter)->draw();
        ++shapeIter;
    }
    _sceneComp.end();

    _sceneComp.draw(0, 0);

}


void LightSystem2D::add(Light2D::SharedPtr light)
{
    _lights.push_back(light);
}


void LightSystem2D::add(Shape2D::SharedPtr shape)
{
    _shapes.push_back(shape);
}


void LightSystem2D::add(const Light2D::List& lights)
{
    _lights.insert(_lights.end(), lights.begin(), lights.end());
}


void LightSystem2D::add(const Shape2D::List& shapes)
{
    _shapes.insert(_shapes.end(), shapes.begin(), shapes.end());
}



void LightSystem2D::remove(Light2D::SharedPtr light)
{
    Light2D::List::iterator iter = std::find(_lights.begin(), _lights.end(), light);

    if (iter != _lights.end())
    {
        _lights.erase(iter);
    }
}


void LightSystem2D::remove(Shape2D::SharedPtr shape)
{
    Shape2D::List::iterator iter = std::find(_shapes.begin(), _shapes.end(), shape);

    if (iter != _shapes.end())
    {
        _shapes.erase(iter);
    }
}


void LightSystem2D::remove(const Light2D::List& lights)
{
    Light2D::List::const_iterator lightIter = lights.begin();

    while (lightIter != lights.end())
    {
        remove(*lightIter);
        ++lightIter;
    }

}


void LightSystem2D::remove(const Shape2D::List& shapes)
{
    Shape2D::List::const_iterator shapeIter = shapes.begin();

    while (shapeIter != shapes.end())
    {
        remove(*shapeIter);
        ++shapeIter;
    }
}


void LightSystem2D::clearLights()
{
    _lights.clear();
}


void LightSystem2D::clearShapes()
{
    _shapes.clear();
}


void LightSystem2D::makeMask(Light2D::SharedPtr light,
                             Shape2D::SharedPtr shape,
                             ofMesh& mask)
{
    const ofPolyline& poly = shape->getShape();

    // Create a list of all poly points that represent a "back facing" edge.
    std::vector<bool> backFacing(poly.size());

    for (std::size_t i = 0; i < poly.size(); ++i)
    {
        std::size_t next = (i + 1) % poly.size();

        ofVec2f firstVertex(poly[i].x,
                            poly[i].y);

        ofVec2f secondVertex(poly[next].x,
                             poly[next].y);

        ofVec2f middle = (firstVertex + secondVertex) / 2;

        ofVec2f lightVector = light->getPosition() - middle;

        ofVec2f edgeNormal;

        edgeNormal.x = - (secondVertex.y - firstVertex.y);
        edgeNormal.y =    secondVertex.x - firstVertex.x;

        backFacing[i] = (edgeNormal.dot(lightVector) > 0);
    }

    std::size_t firstBoundaryIndex = std::numeric_limits<std::size_t>::max();
    std::size_t secondBoundaryIndex = std::numeric_limits<std::size_t>::max();

    for (std::size_t current = 0; current < poly.size(); ++current)
    {
        std::size_t next = (current + 1) % poly.size();

        if (backFacing[current] != backFacing[next])
        {
            if (backFacing[next])
            {
                secondBoundaryIndex = next;
            }
            else
            {
                firstBoundaryIndex = next;
            }
        }
    }

    if (firstBoundaryIndex != std::numeric_limits<std::size_t>::max() &&
        secondBoundaryIndex != std::numeric_limits<std::size_t>::max())
    {
        mask.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

        int numBoundaryEdges = firstBoundaryIndex - secondBoundaryIndex;

        if (numBoundaryEdges < 0)
        {
            numBoundaryEdges += poly.size();
        }

		for(std::size_t offset = 0; offset <= numBoundaryEdges; ++offset)
        {
            int boundaryIndex = (secondBoundaryIndex + offset) % poly.size();

            const ofVec2f& boundaryPoint = poly[boundaryIndex];
            const ofVec2f& lightPoistion = light->getPosition();

            // Create normalized ray from the light to the boundary point.
            ofVec2f ray = boundaryPoint - lightPoistion;

            // Normalize the ray.
            ray.normalize();

            // Scale the ray by the light's radius.
            ray *= light->getRadius();

            // Offset the ray to the boundary point.
            ray += boundaryPoint;

            mask.addVertex(boundaryPoint);
            mask.addColor(ofFloatColor::black);
            mask.addVertex(ray);
            mask.addColor(ofFloatColor::black);
        }
    }
}


void LightSystem2D::windowResized(ofResizeEventArgs& resize)
{
    _lightComp.allocate(resize.width, resize.height, GL_RGBA);
    _sceneComp.allocate(resize.width, resize.height, GL_RGBA);
}


} // namespace ofx
