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

#include <iostream>

#include <LightSystem/ShadowSystem.hpp>
#include <LightSystem/Light.hpp>

#include <DMUtils/sfml.hpp>

namespace DMGDVT {
namespace LS {

    void ShadowSystem::addWall(const sf::ConvexShape& shape) {
        _walls.emplace_back(shape);
    }

    const std::list<sf::ConvexShape> ShadowSystem::getWalls() const {
        return _walls;
    }

    void ShadowSystem::clear() {
        _walls.clear();
    }

    void ShadowSystem::draw(const sf::View& screenView, sf::RenderTarget& target) {
        sf::Vertex points[2];
        points[0].color = sf::Color(180,180,180);
        points[1].color = points[0].color;

        for(const sf::ConvexShape& s : _walls) {
            for(int i = 0;i<s.getPointCount();++i) {
                points[0].position = s.getPoint(i);
                points[1].position = s.getPoint((i+1)%s.getPointCount());
                target.draw(points,2,sf::Lines);
            }
        }
    }

    /*********** PRIVATE ***********/

}
}
