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


#include "Shape2D.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"


namespace ofx {


Shape2D::Shape2D():
    _color(.5, 1),
    _isMeshDirty(true)
{
}


Shape2D::~Shape2D()
{
}


void Shape2D::update()
{
    if (_isMeshDirty)
    {
        createMesh();
    }
}


void Shape2D::draw()
{
    _mesh.draw();
}


void Shape2D::setShape(const ofPolyline& shape)
{
    _shape = shape;
    _position = _shape.getCentroid2D();
}


const ofPolyline& Shape2D::getShape() const
{
    return _shape;
}


ofVec3f Shape2D::getCenter() const
{
    return _position;
}


void Shape2D::setColor(const ofFloatColor& color)
{
    _color = color;
    _isMeshDirty = true;
}


ofFloatColor Shape2D::getColor() const
{
    return _color;
}


void Shape2D::createMesh() const
{
    _mesh.clear();
    _mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    ofVec3f position(_position);

    _mesh.addVertex(position);

    ofFloatColor color(_color);
    color = color * .8;

    _mesh.addColor(_color);

	for (std::size_t i = 0; i < _shape.size(); ++i)
    {
        _mesh.addVertex(_shape[i]);
        _mesh.addColor(color);
    }

    _mesh.addVertex(_shape[0]);
    _mesh.addColor(color);

    _isMeshDirty = false;
}


} // namespace ofx
