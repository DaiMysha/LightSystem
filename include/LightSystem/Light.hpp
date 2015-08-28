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

#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

#include <SFML/Graphics.hpp>

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            Light(const sf::Vector2f& p, const sf::Color& c);
            virtual ~Light();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) = 0;
            virtual void preRender(sf::Shader* shader) = 0;//for light manager mainly
            virtual void debugRender(sf::RenderTarget& target, const sf::RenderStates &states);
            void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);

            virtual void computeAABB() = 0;

            sf::IntRect getAABB();//returns the AABB according to the whole map

            void setPosition(const sf::Vector2f& c);
            sf::Vector2f getPosition() const;
            void move(const sf::Vector2f& delta);

            void setColor(const sf::Color& c);
            sf::Color getColor() const;

            bool isIsometric() const;
            void setIsometric(bool i);

            bool isNegative();
            void setNegative(bool n);

            void setActive(bool a);
            bool isActive() const;

        protected:
            static const char LAS_PARAM_CENTER[];
            static const char LAS_PARAM_RADIUS[];
            static const char LAS_PARAM_COLOR[];
            static const char LAS_PARAM_BLEED[];
            static const char LAS_PARAM_LINEARITY[];
            static const char LAS_PARAM_OUTLINE[];//used mainly for debug, don't mind it
            static const char LAS_PARAM_ISOMETRIC[];

            enum Attributes {
                ACTIVE      = 1,
                NEGATIVE    = 1 << 1,
                EMISSIVE    = 1 << 2,
                ISOMETRIC   = 1 << 3,
                STATIC      = 1 << 4,
                MOVABLE     = 1 << 5,
                DYNAMIC     = STATIC | MOVABLE
            };

            void _setAttribute(Light::Attributes a);
            void _unsetAttribute(Light::Attributes a);

            sf::IntRect _aabb;

            sf::Vector2f _position;
            sf::Color _color;

            sf::RenderTexture* _renderTexture;
            sf::Sprite _sprite;

        private:
            char _attributes;//do not allow user to modify it directly, use the set/unset functions
    };
}
}

#endif // HEADER_DMGDVT_LIGHT

