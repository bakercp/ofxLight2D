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


#include "Light2D.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofEvents.h"


#define STRINGIFY(x) #x


namespace ofx {


const float Light2D::DEFAULT_RADIUS = 500;
const float Light2D::DEFAULT_RANGE = 500;
const std::string Light2D::DEFAULT_LIGHT_SHADER_FRAGMENT_SRC = STRINGIFY(

uniform vec3 lightPos;
uniform vec4 lightColor;
uniform float radius;
uniform float bleed;
uniform float linearizeFactor;

void main()
{
    // We have our camera set up such that the fragment is equivalent to
    // the pixel at the x / y position.
    // Get the distance from this pixel to the light's position.
    float dist = length(lightPos - gl_FragCoord.xyz);
    
    float attenuation = (radius - dist) * (bleed / pow(dist, 2.0) + linearizeFactor / radius);
    
    // Optional, clamp it to prevent overcoloring
    attenuation = clamp(attenuation, 0.0, 1.0);
    
    // Attenuate the pixels colored by the light.
    gl_FragColor = lightColor * attenuation;
}

);


ofShader Light2D::DEFAULT_LIGHT_SHADER;


Light2D::Light2D():
    _position(0, 0, 0),
    _angle(0),
    _viewAngle(TWO_PI),
    _radius(DEFAULT_RADIUS),
    _color(1.0, 1.0, 1.0, 1.0),
    _linearizeFactor(1),
    _bleed(0),
    _isMeshDirty(true)
{
    if (!DEFAULT_LIGHT_SHADER.isLoaded())
    {
        DEFAULT_LIGHT_SHADER.setupShaderFromSource(GL_FRAGMENT_SHADER,
                                                   DEFAULT_LIGHT_SHADER_FRAGMENT_SRC);
        if (ofIsGLProgrammableRenderer())
        {
            DEFAULT_LIGHT_SHADER.bindDefaults();
        }

        DEFAULT_LIGHT_SHADER.linkProgram();
    }
}


Light2D::~Light2D()
{
}


void Light2D::update()
{
    if (_isMeshDirty)
    {
        createMesh();
    }
}


void Light2D::draw()
{
    DEFAULT_LIGHT_SHADER.begin();

    DEFAULT_LIGHT_SHADER.setUniform4f("lightColor", _color.r, _color.g, _color.b, _color.a);
    DEFAULT_LIGHT_SHADER.setUniform3f("lightPos", _position.x, _position.y, _position.z);
    DEFAULT_LIGHT_SHADER.setUniform1f("radius", _radius);
    DEFAULT_LIGHT_SHADER.setUniform1f("bleed", _bleed);
    DEFAULT_LIGHT_SHADER.setUniform1f("linearizeFactor", _linearizeFactor);

    ofPushMatrix();
    ofTranslate(_position);
    ofRotateZ(ofRadToDeg(_angle - _viewAngle / 2.0));
    _mesh.draw();
    ofPopMatrix();

    DEFAULT_LIGHT_SHADER.end();
}


void Light2D::setPosition(const ofVec3f& position)
{
    _position = position;
    _isMeshDirty = true;
}


const ofVec3f& Light2D::getPosition() const
{
    return _position;
}


void Light2D::setAngle(float angle)
{
    _angle = angle;
    _isMeshDirty = true;
}


float Light2D::getAngle() const
{
    return _angle;
}

void Light2D::setViewAngle(float viewAngle)
{
    _viewAngle = viewAngle;
    _isMeshDirty = true;
}


float Light2D::getViewAngle() const
{
    return _viewAngle;
}


float Light2D::getRadius() const
{
    return _radius;
}


void Light2D::setRadius(float radius)
{
    _radius = radius;
}


void Light2D::setColor(const ofFloatColor& color)
{
    _color = color;
    _isMeshDirty = true;
}


ofFloatColor Light2D::getColor() const
{
    return _color;
}


float Light2D::getBleed() const
{
    return _bleed;
}


void Light2D::setBleed(float bleed)
{
    _bleed = bleed;
}

float Light2D::getLinearizeFactor() const
{
    return _linearizeFactor;
}


void Light2D::setLinearizeFactor(float linearizeFactor)
{
    _linearizeFactor = linearizeFactor;
}


void Light2D::createMesh() const
{
    _mesh.clear();
    _mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    ofVec3f position(0, 0, 0);

    ofFloatColor color(_color);

    _mesh.addVertex(position);
    _mesh.addColor(color);

    color = ofFloatColor(0, 0, 0, 0);

    float fanIncrement = TWO_PI / 32;

    for (float angle = 0.0; angle < _viewAngle; angle += fanIncrement)
    {
        if (angle > _viewAngle)
        {
            angle = _viewAngle;
        }

        position.x = cos(angle);
        position.y = sin(angle);

        position *= (_radius * _radius);

        _mesh.addVertex(position);
        _mesh.addColor(color);
    }

    position.x = _radius;
    position.y = 0;

    _mesh.addVertex(position);
    _mesh.addColor(color);

    _isMeshDirty = false;
}


} // namespace ofx
