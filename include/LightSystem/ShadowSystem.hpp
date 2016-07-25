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

#ifndef HEADER_DMGDVT_SHADOWSYSTEM
#define HEADER_DMGDVT_SHADOWSYSTEM

#include <list>

#include <SFML/Graphics.hpp>

namespace dm {
namespace ls {

    class Light;

    class ShadowSystem {
        public:
            void addWall(const sf::ConvexShape& shape);
            const std::list<sf::ConvexShape> getWalls() const;

            void clear();

            void draw(const sf::View& screenView, sf::RenderTarget& target);

            static void castShadowsFromPoint(const sf::Vector2f& origin, const std::list<sf::ConvexShape>& walls, const sf::FloatRect& screenRect, std::list<sf::ConvexShape>& result);

        private:
            static float dist(const sf::Vector2f& a, const sf::Vector2f& b);
            static bool isVisibleFrom(const sf::Vector2f& origin, const sf::Vector2f& target, const sf::Vector2f& s1, const sf::Vector2f& s2, sf::Vector2f& tmp);
            static float intersect(const sf::Vector2f& c, const sf::Vector2f& u, const sf::Vector2f& a, const sf::Vector2f& b, sf::Vector2f& result);

            std::list<sf::ConvexShape> _walls;
    };

}
}

#endif // HEADER_DMGDVT_SHADOWSYSTEM


