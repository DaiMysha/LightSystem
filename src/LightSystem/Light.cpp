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

    Light::Light(bool iso) : _aabb(), _renderTexture(nullptr), _isometric(iso), _active(true) {
    }

    Light::~Light() {
        delete _renderTexture;
    }

    void Light::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
    }

    bool Light::isIsometric() const {
        return _isometric;
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
