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
#include <LightSystem/ShadowSystem.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

namespace dm
{
namespace ls
{

    FlashLight::FlashLight(const sf::Vector2f& p, float r, float l, const sf::Color& c) : FlashLight(p,r,l,c,0.0f,2.0f*M_PIf,1.0f,0.0f,1.0f)
    {
    }

    FlashLight::FlashLight(const sf::Vector2f& p, float r, float l, const sf::Color& c, float da, float sa, float i, float b, float lf) : SpotLight(p,r,c,da,sa,i,b,lf), _length(l)
    {
        computeAABB();
    }

    void FlashLight::preRender(sf::Shader* shader)
    {
        setNegative(_intensity<0.0f);

        if(shader==nullptr) return; //oopsie, can't work without the shader

        const float diam = _radius*2.0f;

        if(_renderTexture==nullptr) _renderTexture = new sf::RenderTexture();
        if(_shadowTexture==nullptr) _shadowTexture = new sf::RenderTexture();

        bool resizeTexture = false;

        if(_resizeWhenIncrease && _renderTexture->getSize().x < diam) resizeTexture = true;
        else if(_renderTexture->getSize().x != diam) resizeTexture = true;

        if(resizeTexture)
        {
            if(!_renderTexture->create(diam,diam))
            {
                delete _renderTexture;
                _renderTexture=nullptr;
                return; //somehow texture failed, maybe too big, abort
            }
            if(!_shadowTexture->create(diam,diam))
            {
                delete _shadowTexture;
                _shadowTexture=nullptr;
                return; //somehow texture failed, maybe too big, abort
            }
        }

        sf::Vector2f center(_renderTexture->getSize().x/2.0f,_renderTexture->getSize().y/2.0f);

        _renderTexture->clear(sf::Color(255,255,255,0));
        _render(*_renderTexture,sf::RenderStates::Default,shader,center);

        computeAABB();

        _hideRegion.setSize(sf::Vector2f(_radius, _length));
        _hideRegion.setFillColor(sf::Color(255,255,255,0));
        _hideRegion.setOrigin(sf::Vector2f(_radius/2.0f, 0.0f));
        _hideRegion.setPosition(center);
        _hideRegion.setRotation(0.0f);

        _renderTexture->draw(_hideRegion);

        _renderTexture->display();

        _sprite.setOrigin(center);
        _sprite.setPosition(_position);
        _sprite.setTexture(_renderTexture->getTexture(),true);
        _sprite.setRotation(DMUtils::maths::radToDeg(_directionAngle));
    }

    void FlashLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states)
    {
        if(getIntensity() == 0.0f) return;
        if(!isActive()) return;

        sf::ConvexShape shape = getShape();

        target.draw(shape,states);
    }

    sf::Color FlashLight::getLightColor(unsigned int x, unsigned int y)
    {
        sf::ConvexShape shape;
        shape.setPointCount(4);
        for(size_t i = 0; i < 4; ++i)
        {
            shape.setPoint(i, _hideRegion.getPoint(i));
        }
        shape.setPosition(_position);
        shape.setRotation(getDirectionAngle());
        shape.setOrigin(sf::Vector2f(_radius/2.0f, 0.0f));
        if(!DMUtils::sfml::contains(shape, sf::Vector2f(x, y)))
        {
            return SpotLight::getLightColor(x, y);
        }
        return sf::Color::Black;
    }

    void FlashLight::setLength(float l)
    {
        _length = l;
    }

    float FlashLight::getLength() const
    {
        return _length;
    }

    /*** PROTECTED ***/

    //This can be optimised with a cached shape that's updated when the flaslight parameters change
    sf::ConvexShape FlashLight::_makeShape() const
    {
        sf::ConvexShape shape;

        return SpotLight::_makeShape();
    }

}
}
