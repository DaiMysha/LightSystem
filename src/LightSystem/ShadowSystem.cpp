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

    void ShadowSystem::castShadowsFromPoint(const sf::Vector2f& origin, const std::list<sf::ConvexShape>& walls, const sf::FloatRect& screenRect, std::list<sf::ConvexShape>& result) {
        sf::ConvexShape screen;
        screen.setPointCount(5);
        screen.setPoint(0,sf::Vector2f(screenRect.left,screenRect.top));
        screen.setPoint(1,sf::Vector2f(screenRect.left + screenRect.width,screenRect.top));
        screen.setPoint(2,sf::Vector2f(screenRect.left + screenRect.width,screenRect.top+screenRect.height));
        screen.setPoint(3,sf::Vector2f(screenRect.left,screenRect.top+screenRect.height));
        screen.setPoint(4,screen.getPoint(0));

        for(const sf::ConvexShape& s : walls) {
            std::list<int> resultPointsList;
            std::vector<bool> visiblePoints;
            int size = s.getPointCount();
            if(size < 3) continue;
            visiblePoints.resize(size,false);
            int closest = 0;
            float distClosest = dist(origin,s.getPoint(closest));
            for(int i=1;i<size;++i) {
                float d = dist(origin,s.getPoint(i));
                if(d < distClosest) {
                    distClosest = d;
                    closest = i;
                }
            }
            resultPointsList.emplace_back(closest);
            visiblePoints[closest] = true;

            bool visible = true;
            sf::Vector2f tmp;
            bool goRight = true;
            bool goLeft = true;

            int id = (closest - 1 + size)%size;
            int id2 = (closest + 1) % size;
            if(id >= 0) {
                visible = isVisibleFrom(origin,s.getPoint(id2),s.getPoint(closest),s.getPoint(id),tmp);
                if(visible) {
                    resultPointsList.emplace_back(id2);
                    visiblePoints[id2] = true;
                }
                goRight = visible;
            }
            if(id2 < size) {
                visible = isVisibleFrom(origin,s.getPoint(id),s.getPoint(closest),s.getPoint(id2),tmp);
                if(visible) {
                    resultPointsList.emplace_front(id);
                    visiblePoints[id] = true;
                }
                goLeft = visible;
            }


            if(goLeft) {
                id = (closest - 2 + size)%size;
                visible = true;
                while(visible && id >= 0) {
                    auto it = resultPointsList.begin();
                    auto it2 = it++;
                    while(it != resultPointsList.end() && visible) {
                        if(id == *it || id == *it2) visible = false;
                        else visible = isVisibleFrom(origin,s.getPoint(id),s.getPoint(*it),s.getPoint(*it2),tmp);
                        if(visible) {
                            it2 = it++;
                        }
                    }
                    if(visible) {
                        visiblePoints[id] = true;
                        resultPointsList.emplace_front(id);
                    } else if(DMUtils::maths::abs(closest-id)<=1) {
                        visiblePoints[id] = false;
                        if(id != *it && id != *it2) {
                            resultPointsList.remove(id);
                        }
                    }
                    --id;
                }
            }

            if(goRight) {
                int id = (closest + 2) % size;
                visible = true;
                while(visible && id < size) {
                    auto it = resultPointsList.begin();
                    auto it2 = it++;
                    while(it != resultPointsList.end() && visible) {
                        if(id == *it || id == *it2) visible = false;
                        else visible = isVisibleFrom(origin,s.getPoint(id),s.getPoint(*it),s.getPoint(*it2),tmp);
                        if(visible) {
                            it2 = it++;
                        }
                    }
                    if(visible) {
                        visiblePoints[id] = true;
                        resultPointsList.emplace_back(id);
                    } else if(DMUtils::maths::abs(closest-id)<=1) {
                        visiblePoints[id] = false;
                        if(id != *it && id != *it2) {
                            resultPointsList.remove(id);
                        }
                    }
                    ++id;
                }
            }

            auto it = resultPointsList.begin();
            auto it_prev = resultPointsList.end();
            while(it != resultPointsList.end()) {
                auto it2 = it;
                ++it2;
                if(it2==resultPointsList.end()) it2 = resultPointsList.begin();
                visible = true;
                while(visible && it2 != resultPointsList.end()) {
                    auto it3 = it2++;
                    if(it2 == resultPointsList.end()) {
                        it2 = resultPointsList.begin();
                    }
                    if(DMUtils::maths::abs(*it2 - *it3) != 1 && !(*it2==0&&*it3==size-1) && !(*it3==0&&*it2==size-1)) {
                        continue;
                    }
                    if(it3 == it || it2 == it) {
                        break;
                    }
                    if(!isVisibleFrom(origin,s.getPoint(*it),s.getPoint(*it2),s.getPoint(*it3),tmp)) {
                        //found a collision, the thiing isn't visible
                        visible = false;
                        visiblePoints[*it] = false;
                    }
                }
                if(!visible) {
                    visiblePoints[*it] = false;
                    if(it_prev==resultPointsList.end()) {
                        resultPointsList.pop_front();
                        it = resultPointsList.begin();
                    } else {
                        resultPointsList.erase(it);
                        it = it_prev;
                    }
                }
                it_prev = it++;
            }

            //pushes edges
            it = resultPointsList.begin();
            while(it!=resultPointsList.end()) {
                if(!visiblePoints[(*it+1)%size]) {
                    resultPointsList.erase(it);
                    resultPointsList.emplace_back(*it);
                    break;
                }
                ++it;
            }
            it = resultPointsList.begin();
            while(it!=resultPointsList.end()) {
                if(!visiblePoints[(*it-1+size)%size]) {
                    resultPointsList.erase(it);
                    resultPointsList.emplace_front(*it);
                    break;
                }
                ++it;
            }

            sf::Vector2f furtherLeft = s.getPoint(*resultPointsList.begin());
            sf::Vector2f furtherRight = s.getPoint(*resultPointsList.rbegin());

            sf::ConvexShape shapeResult;
            //shapeResult.setFillColor(sf::Color(0,0,255,127));
            shapeResult.setFillColor(sf::Color::Black);
            shapeResult.setPointCount(resultPointsList.size()+2);
            int shapeResult_vid = 0;
            for(auto vid : resultPointsList) {
                shapeResult.setPoint(1+shapeResult_vid++,s.getPoint(vid));
            }

            int closestToLeft=0;
            int closestToRight=0;

            //extrapolations
            for(int i=0;i<5;++i) {
                if(intersect(origin,furtherLeft-origin,screen.getPoint(i),screen.getPoint(i+1),tmp)) {
                    shapeResult.setPoint(0,tmp);
                    closestToLeft = i+1;
                }
                if(intersect(origin,furtherRight-origin,screen.getPoint(i),screen.getPoint(i+1),tmp)) {
                    shapeResult.setPoint(shapeResult.getPointCount()-1,tmp);
                    closestToRight = i+1;
                }
            }

            //corners
            if(!(furtherLeft.x <= screenRect.left && furtherRight.x <= screenRect.left) || (furtherLeft.x <= screenRect.left + screenRect.width && furtherRight.x <= screenRect.left + screenRect.width) ||
               !(furtherLeft.y <= screenRect.top && furtherRight.y <= screenRect.top) || (furtherLeft.y <= screenRect.top + screenRect.height && furtherRight.y <= screenRect.top + screenRect.height)) {

                closest = 0;

                std::list<int> cornerList;

                id = closestToLeft;
                int i = 0;
                while(i < 4) {
                    if(intersect(origin,screen.getPoint(id)-origin,furtherLeft,furtherRight,tmp)) {
                        cornerList.emplace_back(id);
                    }
                    id = (id+1)%4;
                    ++i;
                }

                if(cornerList.size()) {
                    int ind = shapeResult.getPointCount();
                    shapeResult.setPointCount(shapeResult.getPointCount()+cornerList.size());
                    for(auto i : cornerList) {
                        shapeResult.setPoint(ind++,screen.getPoint(i));
                    }
                }
            }
            result.emplace_back(shapeResult);
        }
    }

    /*********** PRIVATE ***********/

    float ShadowSystem::dist(const sf::Vector2f& a, const sf::Vector2f& b) {
        return DMUtils::sfml::norm2(b-a);
    }

    bool ShadowSystem::isVisibleFrom(const sf::Vector2f& origin, const sf::Vector2f& target, const sf::Vector2f& s1, const sf::Vector2f& s2, sf::Vector2f& tmp) {
        float d = intersect(origin,target-origin,s1,s2,tmp);
        if(d==0.0f) return true;
        d = DMUtils::sfml::norm2(target-origin)*d*d;
        float d2 = dist(origin,target);
        return d > d2;
    }

    float ShadowSystem::intersect(const sf::Vector2f& c, const sf::Vector2f& u, const sf::Vector2f& a, const sf::Vector2f& b, sf::Vector2f& result) {
        float k1, k2;
        if(u.x == 0.0f) {
            float med = DMUtils::maths::abs(b.y + a.y) / 2.0f;
            if(u.y * (med - c.y) < 0) {
                return 0.0f;
            }

            if(c.x < DMUtils::maths::min(a.x,b.x) || c.x > DMUtils::maths::max(a.x,b.x)) {
                return 0.0f;
            }

            result.x = c.x;
            result.y = med;
            return c.y - med;
        }

        k1 = ( u.x * (c.y - a.y) + u.y * (a.x - c.x) ) / ( u.x * (b.y - a.y) + u.y * (a.x - b.x) );
        k2 = ( a.x + k1 * (b.x - a.x) - c.x ) / u.x;

        if(k1 < 0 || k1 > 1 || k2 <= 0) return 0.0f;

        result.x = c.x + k2 * u.x;
        result.y = c.y + k2 * u.y;

        return k2;
    }

}
}
