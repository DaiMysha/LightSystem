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

#ifndef HEADER_DMGDVT_FLASHLIGHT
#define HEADER_DMGDVT_FLASHLIGHT

#include <SFML/Graphics.hpp>

#include <LightSystem/Light.hpp>
#include <LightSystem/SpotLight.hpp>

namespace dm {
namespace ls {

    class FlashLight : public SpotLight {
        public:
            FlashLight(const sf::Vector2f& p, float r, float l, const sf::Color& c);
            FlashLight(const sf::Vector2f& p, float r, float l, const sf::Color& c, float da, float sa, float i, float b, float lf);

            virtual void preRender(sf::Shader* shader) override;
            virtual void debugRender(sf::RenderTarget& target, const sf::RenderStates &states) override;

            void setLength(float l);
            float getLength() const;

        protected:
            sf::ConvexShape _makeShape() override;

            float _length; //length from origin to start of light

    };
}
}

#endif //HEADER_DMGDVT_FLASHLIGHT
