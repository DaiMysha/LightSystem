#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

#include "SpotLight.hpp"


namespace DMGDVT {
namespace LS {

    SpotLight::SpotLight() : SpotLight(sf::Vector2f(0,0),0,sf::Color(0,0,0,0)) {
	}

    SpotLight::~SpotLight() {
        delete _renderTexture;
	}

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c) : SpotLight(ctr,r,c,0.0f,M_PIf,1.0f,0.0f,0.0f,0.0f) {
    }

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c, float da, float sa, float i, float s, float b, float lf) : Light(),
     _center(ctr), _radius(r), _color(c), _directionAngle(da), _spreadAngle(sa), _size(s), _bleed(b), _linearizationFactor(lf), _renderTexture(nullptr) {
        setSpreadAngle(sa);
        setIntensity(i);
        computeAABB();
    }

    void SpotLight::render(AABB screen, sf::RenderWindow& window) {
        if(_intensity <= 0.0f) return;

        float r = _color.r * _intensity;
        float g = _color.g * _intensity;
        float b = _color.b * _intensity;

        if(_spreadAngle == M_PIf*2.0f) {
            sf::CircleShape shape(_size);
            shape.setPosition(_center - sf::Vector2f(screen.x+_size/2,screen.y+_size/2));
            shape.setFillColor(_color);

            window.draw(shape);

            sf::Vector2f s(screen.x-_center.x, screen.y-_center.y), c(s);


            glBegin(GL_QUADS);
            glColor4f(r,g,b,_color.a);
            glVertex2f(_aabb.x-s.x, _aabb.y-s.y);
            glVertex2f(_aabb.x+_aabb.w-s.x, _aabb.y-s.y);
            glVertex2f(_aabb.x+_aabb.w-s.x, _aabb.y+_aabb.h-s.y);
            glVertex2f(_aabb.x-s.x, _aabb.y+_aabb.h-s.y);
            glEnd();

            /*glBegin(GL_TRIANGLE_FAN);

            glColor4f(r,g,b,_color.a);
            glVertex2f(c.x, c.y);

            // Set the edge color for rest of shape
            int numSubdivisions = static_cast<int>(_spreadAngle / 20.0f);
            float startAngle = _directionAngle - _spreadAngle / 2.0f;

            for(int currentSubDivision = 0; currentSubDivision <= numSubdivisions; currentSubDivision++)
            {
                float angle = startAngle + currentSubDivision * 20.0f;
                glVertex2f(_radius * cosf(angle) + c.x, _radius * sinf(angle) + c.y);
            }

            glEnd();*/
        } else {
            sf::ConvexShape shape;
            shape.setPointCount(4);
            shape.setFillColor(sf::Color(_color.r,_color.g,_color.b,125));
            sf::Vector2f v(0,_radius);
            sf::Vector2f sv(screen.x,screen.y);
            //*
            shape.setPosition(_center-sv);
            shape.setPoint(0,sf::Vector2f(0,0));
            shape.setPoint(1,DMUtils::sfml::rotate(v,-_spreadAngle/2.0f));
            shape.setPoint(2,v);
            shape.setPoint(3,DMUtils::sfml::rotate(v,_spreadAngle/2.0f));

            shape.setRotation(_directionAngle*180.0f/M_PIf);

            window.draw(shape);
        }
	}

    void SpotLight::drawAABB(AABB screen, sf::RenderWindow& window) {
        sf::Vector2f s(screen.x-_center.x, screen.y-_center.y);
        sf::Vertex lines[] = {
            sf::Vertex(sf::Vector2f(_aabb.x, _aabb.y)-s,_color),
            sf::Vertex(sf::Vector2f(_aabb.x+_aabb.w, _aabb.y)-s,_color),
            sf::Vertex(sf::Vector2f(_aabb.x+_aabb.w, _aabb.y+_aabb.h)-s,_color),
            sf::Vertex(sf::Vector2f(_aabb.x, _aabb.y+_aabb.h)-s,_color),
            sf::Vertex(sf::Vector2f(_aabb.x, _aabb.y)-s,_color)
        };

        window.draw(lines,5,sf::LinesStrip);
    }

    void SpotLight::computeAABB() {

        if(_spreadAngle == M_PIf*2.0f) {
            _aabb.x = -_radius/2.0f;
            _aabb.y = -_radius/2.0f;
            _aabb.w = _aabb.h = _radius;
        } else {
            // @TODO : move the rotation function to a real good DMGDVT::sfUtils folder
            sf::Vector2f v = DMUtils::sfml::rotate<float>(sf::Vector2f(0.0,_radius),_directionAngle);
            sf::Vector2f left = DMUtils::sfml::rotate<float>(v,-_spreadAngle/2.0f,_directionAngle);
            sf::Vector2f right = DMUtils::sfml::rotate<float>(v,_spreadAngle/2.0f,_directionAngle);

            int xmin = DMUtils::maths::min<float>(v.x, left.x, right.x, 0.0f);
            int xmax = DMUtils::maths::max<float>(v.x, left.x, right.x, 0.0f);
            int ymin = DMUtils::maths::min<float>(v.y, left.y, right.y, 0.0f);
            int ymax = DMUtils::maths::max<float>(v.y, left.y, right.y, 0.0f);

            _aabb.x = xmin;
            _aabb.y = ymin;
            _aabb.w = xmax - xmin;
            _aabb.h = ymax - ymin;
        }
    }

    /*** GETTER - SETTER ***/
    AABB SpotLight::getAABB() {
        AABB ans;
        ans.x = _center.x+_aabb.x;
        ans.y = _center.y+_aabb.y;
        ans.w = _aabb.w;
        ans.h = _aabb.h;
        return ans;
    }

    void SpotLight::setCenter(sf::Vector2f c) {
        _center = c;
	}

    sf::Vector2f SpotLight::getCenter() const {
        return _center;
	}

    void SpotLight::setRadius(float r) {
        _radius = r;
	}

    float SpotLight::getRadius() const {
        return _radius;
	}

    void SpotLight::setColor(sf::Color c) {
        _color = c;
	}

    sf::Color SpotLight::getColor() const {
        return _color;
	}

    void SpotLight::setDirectionAngle(float da) {
        _directionAngle = da;
	}

    float SpotLight::getDirectionAngle() const {
        return _directionAngle;
	}

    void SpotLight::setSpreadAngle(float sa) {
        if(sa<0) sa = -sa;
        while(sa > 2.0f*M_PIf) sa = 2.0f*M_PIf;
        _spreadAngle = sa;
    }

    float SpotLight::getSpreadAngle() const {
        return _spreadAngle;
    }

    void SpotLight::setIntensity(float i) {
        DMUtils::maths::clamp(i,0.0f,1.0f);
        _intensity = i;
	}

    float SpotLight::getIntensity() const {
        return _intensity;
	}

    void SpotLight::setSize(float s) {
        _size = s;
	}

    float SpotLight::getSize() const {
        return _size;
	}

    void SpotLight::setBleed(float b) {
        _bleed = b;
	}

    float SpotLight::getBleed() const {
        return _bleed;
	}

    void SpotLight::setLinerizationFactor(float lf) {
        _linearizationFactor = lf;
	}

    float SpotLight::getLinearisationFactor() const {
        return _linearizationFactor;
	}

	/*** PROTECTED ***/

}
}
