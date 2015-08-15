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
#include <LightSystem/LocalAmbiantLight.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

namespace DMGDVT {
namespace LS {

    LocalAmbiantLight::~LocalAmbiantLight() {
    }

    LocalAmbiantLight::LocalAmbiantLight(sf::Vector2f p, sf::Vector2f s, sf::Color c, bool negative, bool iso) : Light(iso), _position(p), _size(s), _color(c), _negative(negative) {

        computeAABB();
    }

    void LocalAmbiantLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states) {
        if(!isActive()) return;

        target.draw(_shape,states);
    }

    void LocalAmbiantLight::preRender(sf::Shader* shader) {
        _shape.setPointCount(4);
        _shape.setFillColor(_color);
        _shape.setPosition(_position);

        sf::Vector2f points[4];
        points[0] = sf::Vector2f(0.0f,0.0f);
        points[1] = sf::Vector2f(_size.x,0.0f);
        points[2] = sf::Vector2f(_size.x,_size.y);
        points[3] = sf::Vector2f(0.0f,_size.y);

        if(isIsometric()) {
            for(int i=1;i<4;++i) {
                points[i] = DMUtils::sfml::rotate(points[i],DMUtils::maths::degToRad(45.0f),points[0]);
                points[i].y /= 2.0f;
            }

        }

        for(int i=1;i<4;++i) {
            _shape.setPoint(i,points[i]);
        }

        computeAABB();
    }

    void LocalAmbiantLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
        render(target.getViewport(target.getView()),target,nullptr,states);
    }

    void LocalAmbiantLight::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        sf::IntRect box = getAABB();
        sf::Vertex lines[] = {
            sf::Vertex(sf::Vector2f(box.left, box.top),_color),
            sf::Vertex(sf::Vector2f(box.left+box.width, box.top),_color),
            sf::Vertex(sf::Vector2f(box.left+box.width, box.top+box.height),_color),
            sf::Vertex(sf::Vector2f(box.left, box.top+box.height),_color),
            lines[0]
        };

        target.draw(lines,5,sf::LinesStrip);
    }

    void LocalAmbiantLight::computeAABB() {
        if(isIsometric()) {
            sf::Vector2f points[4];
            points[0] = _position;
            points[1] = _position + sf::Vector2f(_size.x,0.0f);
            points[2] = _position + sf::Vector2f(_size.x,_size.y);
            points[3] = _position + sf::Vector2f(0.0f,_size.y);

            for(int i=1;i<4;++i) {
                points[i] = DMUtils::sfml::rotate(points[i],M_PIf/4.0f,points[0]);
            }

            _aabb.left = DMUtils::maths::min(points[0].x,points[1].x,points[2].x,points[3].x);
            _aabb.top = DMUtils::maths::min(points[0].y,points[1].y,points[2].y,points[3].y);
            _aabb.width = DMUtils::maths::max(points[0].x,points[1].x,points[2].x,points[3].x) - _aabb.left;
            _aabb.height = (DMUtils::maths::max(points[0].y,points[1].y,points[2].y,points[3].y) - _aabb.top)/2.0f;
        } else {
            _aabb.left = _position.x;
            _aabb.top = _position.y;
            _aabb.width = _size.x;
            _aabb.height = _size.y;
        }
    }

    sf::IntRect LocalAmbiantLight::getAABB() {
        return _aabb;
    }

    bool LocalAmbiantLight::isNegative() const {
        return _negative;
    }

}
}
