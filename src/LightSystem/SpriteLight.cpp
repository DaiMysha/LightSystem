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
GOODS OR SERVICES {
	} LOSS OF USE, DATA, OR PROFITS {
	} OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <iostream>

#include <LightSystem/SpriteLight.hpp>

#include <DMUtils/maths.hpp>

namespace dm
{
namespace ls
{

    SpriteLight::SpriteLight(const sf::Vector2f& p, const sf::Color& c, float a, const sf::Sprite& spr) : Light(p,c), _angle(DMUtils::maths::degToRad(a))
    {
        _sprite = spr;
        preRender(nullptr);
    }

    SpriteLight::~SpriteLight()
    {
    }

    void SpriteLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states)
    {
        target.draw(_sprite);
    }

    void SpriteLight::preRender(sf::Shader* shader)
    {
        sf::Vector2u s = _sprite.getTexture()->getSize();
        _sprite.setColor(_color);
        _sprite.setOrigin(sf::Vector2f(s.x/2.0f,s.y/2.0f));
        _sprite.setRotation(DMUtils::maths::radToDeg(_angle));
        _sprite.setPosition(_position);

        computeAABB();
    }

    void SpriteLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states)
    {
        render(target.getViewport(target.getView()),target,nullptr,states);
    }

    void SpriteLight::computeAABB()
    {
        sf::FloatRect f(_sprite.getGlobalBounds());
        _aabb.left = f.left - _position.x;
        _aabb.top = f.top - _position.y;
        _aabb.width = f.width;
        _aabb.height = f.height;
    }

    void SpriteLight::setAngle(float a)
    {
        _angle = DMUtils::maths::degToRad(a);
    }

    float SpriteLight::getAngle() const
    {
        return DMUtils::maths::radToDeg(_angle);
    }

    void SpriteLight::setSprite(const sf::Sprite& s)
    {
        _sprite = s;
    }

}
}
