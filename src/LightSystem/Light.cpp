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

#include <LightSystem/Light.hpp>

namespace DMGDVT {
namespace LS {

    const char Light::LAS_PARAM_CENTER[] = "center";
    const char Light::LAS_PARAM_RADIUS[] = "radius";
    const char Light::LAS_PARAM_COLOR[] = "color";
    const char Light::LAS_PARAM_BLEED[] = "bleed";
    const char Light::LAS_PARAM_LINEARITY[] = "linearFactor";
    const char Light::LAS_PARAM_OUTLINE[] = "outline";
    const char Light::LAS_PARAM_ISOMETRIC[] = "iso";

    Light::Light(const sf::Vector2f& p, const sf::Color& c, bool iso) : _aabb(), _position(p), _color(c), _renderTexture(nullptr), _isometric(iso), _active(true) {
    }

    Light::~Light() {
        delete _renderTexture;
    }

    void Light::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
    }

    void Light::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
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

    bool Light::isIsometric() const {
        return _isometric;
    }

    sf::IntRect Light::getAABB() {
        return sf::IntRect(sf::Vector2i(_aabb.left+static_cast<int>(_position.x),_aabb.top+static_cast<int>(_position.y)),sf::Vector2i(_aabb.width,_aabb.height));
    }

    void Light::setPosition(const sf::Vector2f& c) {
        _position = c;
        _sprite.setPosition(c);
	}

    sf::Vector2f Light::getPosition() const {
        return _position;
	}

    void Light::move(const sf::Vector2f& delta) {
        _position += delta;
    }

    void Light::setColor(const sf::Color& c) {
        _color = c;
	}

    sf::Color Light::getColor() const {
        return _color;
	}

    void Light::setIsometric(bool i) {
        _isometric = i;
    }

    void Light::setActive(bool a) {
        _active = a;
    }

    bool Light::isActive() const {
        return _active;
    }

}
}
