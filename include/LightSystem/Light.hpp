/*
Copyright DaiMysha (c) 2015, All rights reserved.
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

#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

#include <SFML/Graphics.hpp>

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            Light(bool iso = false);
            virtual ~Light();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) = 0;
            virtual void preRender(sf::Shader* shader) = 0;//for light manager mainly
            virtual void debugRender(sf::RenderTarget& target, const sf::RenderStates &states);
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) = 0;

            virtual void computeAABB() = 0;

            virtual sf::IntRect getAABB() = 0;//returns the AABB according to the whole map

            virtual bool isIsometric() const final;

            virtual void setIsometric(bool i) final;

        protected:
            static const char LAS_PARAM_CENTER[];
            static const char LAS_PARAM_RADIUS[];
            static const char LAS_PARAM_COLOR[];
            static const char LAS_PARAM_BLEED[];
            static const char LAS_PARAM_LINEARITY[];
            static const char LAS_PARAM_OUTLINE[];//used mainly for debug, don't mind it
            static const char LAS_PARAM_ISOMETRIC[];

            sf::IntRect _aabb;

            //change to unique_ptr ?
            sf::RenderTexture* _renderTexture;
            sf::Sprite _sprite;

            bool _isometric;
    };
}
}

#endif // HEADER_DMGDVT_LIGHT

