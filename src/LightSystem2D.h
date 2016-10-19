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


#include "Light2D.h"
#include "Shape2D.h"
#include "ofTexture.h"
#include "ofShader.h"
#include "ofFbo.h"


namespace ofx {


class LightSystem2D
{
public:
    LightSystem2D();
    virtual ~LightSystem2D();

    void setup(ofEventArgs& args);
    void update(ofEventArgs& args);
    void draw(ofEventArgs& args);

    void add(std::shared_ptr<Light2D> light);
    void add(std::shared_ptr<Shape2D> shape);

    void add(const Light2D::List& light);
    void add(const Shape2D::List& shape);

    void remove(std::shared_ptr<Light2D> light);
    void remove(std::shared_ptr<Shape2D> shape);

    void remove(const Light2D::List& lights);
    void remove(const Shape2D::List& shapes);

    void clearLights();
    void clearShapes();

    void windowResized(ofResizeEventArgs& resize);

protected:
    Light2D::List _lights;
    Shape2D::List _shapes;

    ofFbo _lightComp;
    ofFbo _sceneComp;

    static void makeMask(std::shared_ptr<Light2D> light,
                         std::shared_ptr<Shape2D> shape,
                         ofMesh& mask);

};


} // namespace ofx
