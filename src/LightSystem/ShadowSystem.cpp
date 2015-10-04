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

    ShadowSystem::Segment::Segment() {
    }

    ShadowSystem::Segment::Segment(const sf::Vector2f& pp1, const sf::Vector2f& pp2) : p1(pp1), p2(pp2) {
    }

    void ShadowSystem::addSegment(const sf::Vector2f& p1, const sf::Vector2f& p2) {
        _segments.emplace_back(Segment(p1,p2));
    }

    void ShadowSystem::addSegment(const sf::ConvexShape& shape) {
        for(size_t i=0;i<shape.getPointCount()-1;++i) {
            addSegment(shape.getPoint(i),shape.getPoint(i+1));
        }
        addSegment(shape.getPoint(shape.getPointCount()),shape.getPoint(0));
    }

    void ShadowSystem::clear() {
        _segments.clear();
    }

        ///forward that to the actual light
        ///find a way to give it the list of the segments
        ///issue with end of screen
    void ShadowSystem::debugDraw(Light* l, const sf::View& screenView, sf::RenderTarget& target) {
        const sf::Vector2f& origin(l->getPosition());

        const sf::IntRect aabb = l->getAABB();

        std::list<sf::Vector2f> collisionPoints;
        std::list<sf::Vector2f> points;

        Segment box[4];
        box[0].p1 = sf::Vector2f(aabb.left,aabb.top);
        box[0].p2 = sf::Vector2f(aabb.left+aabb.width,aabb.top);
        box[1].p1 = sf::Vector2f(aabb.left+aabb.width,aabb.top);
        box[1].p2 = sf::Vector2f(aabb.left+aabb.width,aabb.top+aabb.height);
        box[2].p1 = sf::Vector2f(aabb.left+aabb.width,aabb.top+aabb.height);
        box[2].p2 = sf::Vector2f(aabb.left,aabb.top+aabb.height);
        box[3].p1 = sf::Vector2f(aabb.left,aabb.top+aabb.height);
        box[3].p2 = sf::Vector2f(aabb.left,aabb.top);

        for(const Segment& s : _segments) {
            if(aabb.contains(s.p1.x,s.p1.y) || aabb.contains(s.p2.x,s.p2.y)) {
                points.emplace_back(s.p1);
                points.emplace_back(DMUtils::sfml::rotate(s.p1,0.0001));
                points.emplace_back(DMUtils::sfml::rotate(s.p1,-0.0001));
                points.emplace_back(s.p2);
                points.emplace_back(DMUtils::sfml::rotate(s.p2,0.0001));
                points.emplace_back(DMUtils::sfml::rotate(s.p2,-0.0001));
            }
        }
        for(int i = 0;i<4;++i) {
            points.emplace_back(box[i].p1);
            points.emplace_back(box[i].p2);
        }

        castFromPoint(origin,points,box,collisionPoints);

        sf::Vertex line[2];
        line[0].color = sf::Color(255,180,180);
        line[1].color = line[0].color;
            line[0].position = origin;


        collisionPoints.sort([origin](const sf::Vector2f& a, const sf::Vector2f& b) {
            sf::Vector2f o(1,0);
            float angle_a = DMUtils::sfml::getAngleBetweenVectors(a-origin,o);
            float angle_b = DMUtils::sfml::getAngleBetweenVectors(b-origin,o);
            return angle_a < angle_b;
        });

        std::list<sf::ConvexShape> result;

        sf::ConvexShape s;
        s.setFillColor(sf::Color(255,255,255,127));
        s.setPointCount(3);
        s.setPoint(0,origin);
        int iii=1;
        result.clear();
        for(sf::Vector2f p: collisionPoints) {
            s.setPoint(iii,p);
            ++iii;
            if(iii==3) {
                result.push_back(s);
                s.setPoint(1,p);
                iii=2;
            }
        }
        s.setPoint(1,*collisionPoints.rbegin());
        s.setPoint(2,*collisionPoints.begin());
        result.push_back(s);

        for(const sf::ConvexShape& s : result) {
            target.draw(s);
        }

        for(const sf::Vector2f& p : collisionPoints) {
            line[1].position = p;
            //target.draw(line,2,sf::Lines);
        }
    }

    void ShadowSystem::draw(const sf::View& screenView, sf::RenderTarget& target) {
        sf::Vertex points[2];
        points[0].color = sf::Color(180,180,180);
        points[1].color = points[0].color;

        for(const Segment& s : _segments) {
            points[0].position = s.p1;
            points[1].position = s.p2;
            target.draw(points,2,sf::Lines);
        }
    }

    /*********** PRIVATE ***********/

    void ShadowSystem::castFromPoint(const sf::Vector2f& origin, const std::list<sf::Vector2f>& points, Segment box[4], std::list<sf::Vector2f>& result) {
        float t = 0.0f;
        sf::Vector2f tmp, tmpClosest;
        sf::Vector2f r_p,r_d;

        for(sf::Vector2f p : points) {
            p = p - origin;
            t = findClosestIntersect(origin,p,tmp,box);
            if(t != 0.0f) {
                result.push_back(tmp);
            }
        }
    }

    float ShadowSystem::findClosestIntersect(const sf::Vector2f& r_p, const sf::Vector2f& r_d, sf::Vector2f& result, Segment box[4]) {
        sf::Vector2f s_p,s_d;
        float t1 = 0.0f;
        float t;
        sf::Vector2f tmp;
        for(const Segment& s : _segments) {
            s_p = s.p1;
            s_d = s.p2 - s_p;
            t = findIntersect(r_p,r_d,s_p,s_d,tmp);
            if(t1 == 0.0f || (t != 0.0f && t < t1)) {
                t1 = t;
                result = tmp;
            }
        }
        for(int i=0;i<4;++i) {
            s_p = box[i].p1;
            s_d = box[i].p2 - s_p;
            t = findIntersect(r_p,r_d,s_p,s_d,tmp);
            if(t1 == 0.0f || (t != 0.0f && t < t1)) {
                t1 = t;
                result = tmp;
            }
        }

        return t1;
    }

    float ShadowSystem::findIntersect(const sf::Vector2f& r_p, const sf::Vector2f& r_d, const sf::Vector2f& s_p, const sf::Vector2f& s_d, sf::Vector2f& result) {
        if(r_d == s_d) {
            return 0.0f;
        }

        float t1, t2;
        t2 = (r_d.x*(s_p.y-r_p.y) + r_d.y*(r_p.x-s_p.x))/(s_d.x*r_d.y - s_d.y*r_d.x);
        t1 = (s_p.x+s_d.x*t2-r_p.x)/r_d.x;
        //*
        if(t1<=0 || t2<0 || t2>1) {
        /*/
        if(t1<0 || t2<=0 || t2>=1 ) {
        //*/
            return 0.0f;
        }

        result = sf::Vector2f(r_p.x + t1*r_d.x, r_p.y + t1*r_d.y);
        return t1;
    }

}
}
