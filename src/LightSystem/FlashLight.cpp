/*
Copyright DaiMysha (c) 2015, All rights reserved.
DaiMysha@gmail.com
https://github.com/DaiMysha

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <iostream>

#include <LightSystem/FlashLight.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

namespace DMGDVT {
namespace LS {

    FlashLight::FlashLight(const sf::Vector2f& p, float r, float w, const sf::Color& c) : FlashLight(p,r,w,c,0.0f,2.0f*M_PIf,1.0f,0.0f,1.0f) {
    }

    FlashLight::FlashLight(const sf::Vector2f& p, float r, float w, const sf::Color& c, float da, float sa, float i, float b, float lf) : SpotLight(p,r,c,da,sa,i,b,lf), _width(w) {
        computeAABB();
    }

    void FlashLight::preRender(sf::Shader* shader) {

        setNegative(_intensity<0.0f);

        if(shader==nullptr) return; //oopsie, can't work without the shader

        const float diam = DMUtils::maths::max(_width,_radius);

        if(_renderTexture==nullptr) _renderTexture = new sf::RenderTexture();

        bool resizeTexture = false;

        if(_resizeWhenIncrease && _renderTexture->getSize().x < diam) resizeTexture = true;
        else if(_renderTexture->getSize().x != diam) resizeTexture = true;

        if(resizeTexture && !_renderTexture->create(diam,diam)) {
            delete _renderTexture;
            _renderTexture=nullptr;
            return; //somehow texture failed, maybe too big, abort
        }

        _renderTexture->clear();
        _render(*_renderTexture,sf::RenderStates::Default,shader,sf::Vector2f(_renderTexture->getSize().x/2.0f,_renderTexture->getSize().y),sf::Vector2f(_renderTexture->getSize().x/2.0f,0.0f));
        _renderTexture->display();

        _sprite.setPosition(_position);
        _sprite.setOrigin(sf::Vector2f(_renderTexture->getSize().x/2.0f,0.0f));
        _sprite.setTexture(_renderTexture->getTexture());
        _sprite.setRotation(DMUtils::maths::radToDeg(_directionAngle));

        computeAABB();
    }

    void FlashLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
        if(getIntensity() == 0.0f) return;
        if(!isActive()) return;

        sf::ConvexShape shape = _makeShape();
        shape.setPosition(getPosition());
        shape.setRotation(getDirectionAngle());

        target.draw(shape,states);
    }

    void FlashLight::computeAABB() {
        sf::ConvexShape shape = _makeShape();
        if(shape.getPointCount()==0) return;

        float xmin = shape.getPoint(0).x;
        float xmax = shape.getPoint(0).x;
        float ymin = shape.getPoint(0).y;
        float ymax = shape.getPoint(0).y;

        for(size_t i=0;i<shape.getPointCount();++i) {
            sf::Vector2f p = DMUtils::sfml::rotate(shape.getPoint(i),_directionAngle,sf::Vector2f(0.0f,0.0f));
            xmin = DMUtils::maths::min(xmin,p.x);
            xmax = DMUtils::maths::max(xmax,p.x);
            ymin = DMUtils::maths::min(ymin,p.y);
            ymax = DMUtils::maths::max(ymax,p.y);
        }

        _aabb.left = xmin;
        _aabb.top = ymin;
        _aabb.width = xmax - xmin;
        _aabb.height = ymax - ymin;
     }

    void FlashLight::setWidth(float w) {
        _width = w;
    }

    float FlashLight::getWidth() const {
        return _width;
    }

     /*** PROTECTED ***/

    //This can be optimised with a cached shape that's updated when the flaslight parameters change
    sf::ConvexShape FlashLight::_makeShape() {
        sf::ConvexShape shape;

        shape.setPointCount(4);
        shape.setPoint(0,sf::Vector2f(-_width/2.0f,0.0f));
        shape.setPoint(1,sf::Vector2f(_width/2.0f,0.0f));

        shape.setPoint(2,DMUtils::sfml::rotate(sf::Vector2f(_width/2.0f,getRadius()),-_spreadAngle,shape.getPoint(1)));
        shape.setPoint(3,DMUtils::sfml::rotate(sf::Vector2f(-_width/2.0f,getRadius()),_spreadAngle,shape.getPoint(0)));

        shape.setPointCount(_precision+2);
        shape.setPoint(_precision+1,sf::Vector2f(-_width/2.0f,0.0f));
        shape.setPoint(0,sf::Vector2f(_width/2.0f,0.0f));

        float deltaAngle = _spreadAngle / (float)(_precision-1);

        for(int i=0;i<_precision;++i) {
            float angle = - _spreadAngle/2.0f + (float)i*deltaAngle;
            sf::Vector2f base(-_width/2.0f+(i+1)*_width/(float)(getPrecision()),0.0f);

            shape.setPoint(i+1,DMUtils::sfml::rotate(sf::Vector2f(0.0f,_radius*1.1)+base,angle,base));
        }

        return shape;
    }

}
}
