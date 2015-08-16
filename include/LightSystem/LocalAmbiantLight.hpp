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

#ifndef HEADER_DMGDVT_LOCALAMBIANTLIGHT
#define HEADER_DMGDVT_LOCALAMBIANTLIGHT

#include <SFML/Graphics.hpp>

#include <LightSystem/Light.hpp>

namespace DMGDVT {
namespace LS {

    class LocalAmbiantLight : public Light {
        public:
            LocalAmbiantLight(sf::Vector2f p, sf::ConvexShape s, sf::Color c, bool negative = false, bool iso = false);
            virtual ~LocalAmbiantLight();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) override;
            virtual void preRender(sf::Shader* shader) override;
            virtual void debugRender(sf::RenderTarget& target, const sf::RenderStates &states) override;

            virtual void computeAABB() override;

            virtual bool isNegative() const override;

        protected:
            sf::Vector2f _size;
            sf::ConvexShape _shape;
            bool _negative;
    };
}
}

#endif // HEADER_DMGDVT_LOCALAMBIANTLIGHT


