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

namespace dm
{
namespace ls
{

    void ShadowSystem::addWall(const sf::ConvexShape& shape)
    {
        _walls.emplace_back(shape);
    }

    const std::list<sf::ConvexShape> ShadowSystem::getWalls() const
    {
        return _walls;
    }

    void ShadowSystem::clear()
    {
        _walls.clear();
    }

    void ShadowSystem::draw(const sf::View& screenView, sf::RenderTarget& target)
    {
        sf::Vertex points[2];
        points[0].color = sf::Color(180,180,180);
        points[1].color = points[0].color;

        for(const sf::ConvexShape& s : _walls)
        {
            for(size_t i = 0; i<s.getPointCount(); ++i)
            {
                points[0].position = s.getPoint(i);
                points[1].position = s.getPoint((i+1)%s.getPointCount());
                target.draw(points,2,sf::Lines);
            }
        }
    }

    void ShadowSystem::castShadowsFromPoint(const sf::Vector2f& origin, const std::list<sf::ConvexShape>& walls, const sf::FloatRect& screenRect, std::list<sf::ConvexShape>& result)
    {
        sf::Vector2f tmp;

        for(const sf::ConvexShape& s : walls)
        {
            const int size = s.getPointCount();

            std::vector<bool> backFacing(size);

            for (int i = 0; i < size; i++)
            {
                sf::Vector2f firstVertex(s.getPoint(i).x, s.getPoint(i).y);
                int secondIndex = (i + 1) % size;
                sf::Vector2f secondVertex(s.getPoint(secondIndex).x, s.getPoint(secondIndex).y);
                sf::Vector2f middle = (firstVertex + secondVertex);
                middle.x /= 2;
                middle.y /= 2;

                sf::Vector2f u = origin - middle;

                sf::Vector2f normal;
                normal.x = - (secondVertex.y - firstVertex.y);
                normal.y = secondVertex.x - firstVertex.x;

                if (DMUtils::sfml::dot(normal,u) > 0)
                    backFacing[i] = false;
                else
                    backFacing[i] = true;
            }

            int firstBoundaryIndex = 0;
            int secondBoundaryIndex = 0;

            for (int i = 0; i < size; i++)
            {
                int currentEdge = i;
                int nextEdge = (i + 1) % size;

                if (backFacing[currentEdge] && !backFacing[nextEdge])
                    firstBoundaryIndex = nextEdge;

                if (!backFacing[currentEdge] && backFacing[nextEdge])
                    secondBoundaryIndex = nextEdge;
            }

            int shapeSize = 0;
            if(secondBoundaryIndex < firstBoundaryIndex)
            {
                shapeSize = DMUtils::maths::abs(secondBoundaryIndex - firstBoundaryIndex) + 1;
            }
            else
            {
                shapeSize = DMUtils::maths::abs(size - secondBoundaryIndex + firstBoundaryIndex) + 1;
            }

            if(shapeSize) {
                sf::ConvexShape resultShape;
                resultShape.setFillColor(sf::Color::Black);
                resultShape.setPointCount(shapeSize);

                int id = 0;
                int i = firstBoundaryIndex;
                while(i != secondBoundaryIndex) {
                    resultShape.setPoint(id++,s.getPoint(i));
                    --i;
                    if(i<0) i+=size;
                    else if(i>size) i-=size;
                }
                resultShape.setPoint(id++,s.getPoint(secondBoundaryIndex));

                i = secondBoundaryIndex;
                while(i != firstBoundaryIndex)
                {

                    sf::Vector2f delta = s.getPoint(i) - origin;
                    float n = DMUtils::sfml::norm(delta);
                    delta.x /= n;
                    delta.y /= n;

                    delta.x *= screenRect.width;
                    delta.y *= screenRect.height;

                    resultShape.setPointCount(resultShape.getPointCount()+1);
                    resultShape.setPoint(resultShape.getPointCount()-1, s.getPoint(i) + delta);
                    i = (i-1 + s.getPointCount())%s.getPointCount();
                }
                {
                    sf::Vector2f delta = s.getPoint(firstBoundaryIndex) - origin;
                    float n = DMUtils::sfml::norm(delta);
                    delta.x /= n;
                    delta.y /= n;

                    delta.x *= screenRect.width;
                    delta.y *= screenRect.height;

                    resultShape.setPointCount(resultShape.getPointCount()+1);
                    resultShape.setPoint(resultShape.getPointCount()-1, s.getPoint(firstBoundaryIndex) + delta);
                }

                result.emplace_back(resultShape);
            }
        }
    }

    /*********** PRIVATE ***********/

    float ShadowSystem::dist(const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return DMUtils::sfml::norm2(b-a);
    }

    bool ShadowSystem::isVisibleFrom(const sf::Vector2f& origin, const sf::Vector2f& target, const sf::Vector2f& s1, const sf::Vector2f& s2, sf::Vector2f& tmp)
    {
        float d = intersect(origin,target-origin,s1,s2,tmp);
        if(d==0.0f) return true;
        d = DMUtils::sfml::norm2(target-origin)*d*d;
        float d2 = dist(origin,target);
        return d > d2;
    }

    float ShadowSystem::intersect(const sf::Vector2f& c, const sf::Vector2f& u, const sf::Vector2f& a, const sf::Vector2f& b, sf::Vector2f& result)
    {
        float k1, k2;
        if(u.x == 0.0f)
        {
            float med = DMUtils::maths::abs(b.y + a.y) / 2.0f;
            if(u.y * (med - c.y) < 0)
            {
                return 0.0f;
            }

            if(c.x < DMUtils::maths::min(a.x,b.x) || c.x > DMUtils::maths::max(a.x,b.x))
            {
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
