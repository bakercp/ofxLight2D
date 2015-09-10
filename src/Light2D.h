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


#pragma once


#include "ofVec2f.h"
#include "ofColor.h"
#include "ofMesh.h"
#include "ofShader.h"


namespace ofx {


class Light2D
{
public:
	typedef std::shared_ptr<Light2D> SharedPtr;
	typedef std::vector<SharedPtr> List;

    Light2D();
    virtual ~Light2D();

    virtual void update();
    virtual void draw();

    void setPosition(const ofVec3f& position);
    const ofVec3f& getPosition() const;

    void setAngle(float angle);
    float getAngle() const;

    void setViewAngle(float viewAngle);
    float getViewAngle() const;

    void setRadius(float radius);
    float getRadius() const;

    void setColor(const ofFloatColor& color);
    ofFloatColor getColor() const;

    float getBleed() const;
    void setBleed(float bleed);

    float getLinearizeFactor() const;
    void setLinearizeFactor(float linearizeFactor);

    static const float DEFAULT_RADIUS;
    static const float DEFAULT_RANGE;
    static const std::string DEFAULT_LIGHT_SHADER_FRAGMENT_SRC;

    static ofShader DEFAULT_LIGHT_SHADER;

protected:
    ofVec3f _position;
    float _angle;

    float _radius;

    float _viewAngle;
    ofFloatColor _color;
    float _bleed;
    float _linearizeFactor;

    void createMesh() const;
    mutable bool _isMeshDirty;
    mutable ofMesh _mesh;
};


} // namespace ofx
