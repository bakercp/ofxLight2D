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
    for (auto light: _lights) light->update();
    for (auto shape: _shapes) shape->update();
}


void LightSystem2D::draw(ofEventArgs& args)
{
    _sceneComp.begin();
    ofClear(0, 0, 0, 0);
    _sceneComp.end();

    for (const auto& light: _lights)
    {
        _lightComp.begin();
        ofClear(0, 0, 0, 0);

        light->draw();

        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        for (const auto& shape: _shapes)
        {
            ofMesh mesh;
            makeMask(light, shape, mesh);
            mesh.draw();
        }
        ofPopStyle();

        _lightComp.end();

        _sceneComp.begin();
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        _lightComp.draw(0, 0);
        ofPopStyle();
        _sceneComp.end();
    }

    _sceneComp.begin();

    for (const auto& shape: _shapes) shape->draw();

    _sceneComp.end();

    _sceneComp.draw(0, 0);

}


void LightSystem2D::add(std::shared_ptr<Light2D> light)
{
    _lights.push_back(light);
}


void LightSystem2D::add(std::shared_ptr<Shape2D> shape)
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


void LightSystem2D::remove(std::shared_ptr<Light2D> light)
{
    _lights.erase(std::find(_lights.begin(), _lights.end(), light));
}


void LightSystem2D::remove(std::shared_ptr<Shape2D> shape)
{
    _shapes.erase(std::find(_shapes.begin(), _shapes.end(), shape));
}


void LightSystem2D::remove(const Light2D::List& lights)
{
    auto lightIter = lights.begin();

    while (lightIter != lights.end())
    {
        remove(*lightIter);
        ++lightIter;
    }

}


void LightSystem2D::remove(const Shape2D::List& shapes)
{
    auto shapeIter = shapes.begin();

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


void LightSystem2D::makeMask(std::shared_ptr<Light2D> light,
                             std::shared_ptr<Shape2D> shape,
                             ofMesh& mask)
{
    const auto& poly = shape->getShape();

    // Create a list of all poly points that represent a "back facing" edge.
    std::vector<bool> backFacing(poly.size());

    for (std::size_t i = 0; i < poly.size(); ++i)
    {
        std::size_t next = (i + 1) % poly.size();

        glm::vec3 firstVertex(poly[i].x, poly[i].y, 0);

        glm::vec3 secondVertex(poly[next].x, poly[next].y, 0);

        glm::vec3 middle = (firstVertex + secondVertex) / 2;

        glm::vec3 lightVector = light->getPosition() - middle;

        glm::vec3 edgeNormal;

        edgeNormal.x = - (secondVertex.y - firstVertex.y);
        edgeNormal.y =    secondVertex.x - firstVertex.x;

        backFacing[i] = (glm::dot(edgeNormal, lightVector) > 0);
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
            std::size_t boundaryIndex = (secondBoundaryIndex + offset) % poly.size();

            const auto& boundaryPoint = poly[boundaryIndex];
            const auto& lightPoistion = light->getPosition();

            // Create normalized ray from the light to the boundary point.
            auto ray = boundaryPoint - lightPoistion;

            // Normalize the ray.
            ray = glm::normalize(ray);

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
