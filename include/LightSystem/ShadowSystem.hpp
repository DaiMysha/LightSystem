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

namespace DMGDVT {
namespace LS {

    class Light;

    //have to un-nest this
    struct Segment {
        Segment();
        Segment(const sf::Vector2f& pp1, const sf::Vector2f& pp2);

        sf::Vector2f p1;
        sf::Vector2f p2;
    };

    class ShadowSystem {
        public:

            void addSegment(const sf::Vector2f& p1, const sf::Vector2f& p2);
            void addSegment(const sf::ConvexShape& shape);
            void clear();

            void debugDraw(Light* l, const sf::View& screenView, sf::RenderTarget& target);
            void draw(const sf::View& screenView, sf::RenderTarget& target);

            static void castFromPoint(const sf::Vector2f& origin, const std::list<Segment>& segments, const std::list<sf::Vector2f>& points, Segment box[4], std::list<sf::Vector2f>& result);

        private:
            std::list<Segment> _segments;

            static float findClosestIntersect(const sf::Vector2f& r_p, const sf::Vector2f& r_d, const std::list<Segment>& segments, sf::Vector2f& result, Segment box[4]);
            static float findIntersect(const sf::Vector2f& r_p, const sf::Vector2f& r_d, const sf::Vector2f& s_p, const sf::Vector2f& s_d, sf::Vector2f& result);
    };

}
}

#endif // HEADER_DMGDVT_SHADOWSYSTEM


