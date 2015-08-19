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

#include <LightSystem/LocalAmbiantLight.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

namespace DMGDVT {
namespace LS {

    LocalAmbiantLight::~LocalAmbiantLight() {
    }

    LocalAmbiantLight::LocalAmbiantLight(const sf::Vector2f& p, const sf::ConvexShape& s, const sf::Color& c, bool negative, bool iso) : Light(p,c,iso), _shape(s), _negative(negative) {

        computeAABB();
    }

    void LocalAmbiantLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states) {
        if(!isActive()) return;

        target.draw(_shape,states);
    }

    void LocalAmbiantLight::preRender(sf::Shader* shader) {
        _shape.setFillColor(_color);
        _shape.setPosition(_position);

        computeAABB();
    }

    void LocalAmbiantLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
        render(target.getViewport(target.getView()),target,nullptr,states);
    }

    void LocalAmbiantLight::computeAABB() {
        float minx = _shape.getPoint(0).x, maxx = minx;
        float miny = _shape.getPoint(0).y, maxy = miny;
        for(size_t i=0;i<_shape.getPointCount();++i) {
            minx = DMUtils::maths::min(minx,_shape.getPoint(i).x);
            maxx = DMUtils::maths::max(maxx,_shape.getPoint(i).x);

            miny = DMUtils::maths::min(miny,_shape.getPoint(i).y);
            maxy = DMUtils::maths::max(maxy,_shape.getPoint(i).y);
        }

        _aabb.left = minx;
        _aabb.top = miny;
        _aabb.width = maxx - minx;
        _aabb.height = maxy - miny;
    }

    bool LocalAmbiantLight::isNegative() const {
        return _negative;
    }

    void LocalAmbiantLight::setShape(const sf::ConvexShape& s) {
        _shape = s;
    }

    sf::ConvexShape LocalAmbiantLight::getShape() const {
        return _shape;
    }
}
}
