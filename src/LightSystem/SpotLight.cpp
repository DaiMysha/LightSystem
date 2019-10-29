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

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

#include <LightSystem/SpotLight.hpp>

namespace dm
{
namespace ls
{

    SpotLight::~SpotLight()
    {
    }

    SpotLight::SpotLight(const sf::Vector2f& p, float r, const sf::Color& c) : SpotLight(p,r,c,0.0f,360.0f,1.0f,0.0f,1.0f)
    {
    }

    SpotLight::SpotLight(const sf::Vector2f& p, float r, const sf::Color& c, float da, float sa, float i, float b, float lf) : Light(p,c),
        _radius(r), _directionAngle(DMUtils::maths::degToRad(da)), _spreadAngle(sa), _bleed(b), _linearity(lf), _resizeWhenIncrease(false)
    {
        setSpreadAngle(sa);
        setIntensity(i);
        computeAABB();

        //32 points for a full circle, so we keep same spacing
        if(_spreadAngle > M_PIf * 3.0f/4.0f) _precision = 32;
        else if(_spreadAngle > M_PIf) _precision = 16;
        else if (_spreadAngle < M_PIf/2.0f) _precision = 4;
        else _precision = 8;
    }

    void SpotLight::preRender(sf::Shader* shader)
    {
        setIntensity((isNegative()?-1:1) * std::abs(getIntensity()));

        if(shader==nullptr) return; //oopsie, can't work without the shader

        const float diam = _radius*2.0f;

        if(_renderTexture==nullptr) _renderTexture = new sf::RenderTexture();
        if(_shadowTexture==nullptr) _shadowTexture = new sf::RenderTexture();

        bool resizeTexture = false;

        if(_resizeWhenIncrease && _renderTexture->getSize().x < diam) resizeTexture = true;
        else if(_renderTexture->getSize().x != diam) resizeTexture = true;

        if(resizeTexture)
        {
            if(!_renderTexture->create(diam,diam))
            {
                delete _renderTexture;
                _renderTexture=nullptr;
                return; //somehow texture failed, maybe too big, abort
            }
            if(!_shadowTexture->create(diam,diam))
            {
                delete _shadowTexture;
                _shadowTexture=nullptr;
                return; //somehow texture failed, maybe too big, abort
            }
        }

        sf::Vector2f center(_renderTexture->getSize().x/2.0f,_renderTexture->getSize().y/2.0f);

        _renderTexture->clear(sf::Color(255,255,255,0));
        _render(*_renderTexture,sf::RenderStates::Default,shader,center);
        _renderTexture->display();

        _sprite.setOrigin(center);
        _sprite.setPosition(_position);
        _sprite.setTexture(_renderTexture->getTexture(),true);
        _sprite.setRotation(DMUtils::maths::radToDeg(_directionAngle));
        _sprite.setColor(_color);

        computeAABB();
    }

    void SpotLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states)
    {
        if(_intensity == 0.0f) return;
        if(!isActive()) return;

        sf::RenderStates st(states);
        st.blendMode = sf::BlendAdd;
        if(_shadowTexture!=nullptr)
        {
            sf::Vector2f center(_shadowTexture->getSize().x/2.0f,_shadowTexture->getSize().y/2.0f);
            sf::Sprite spr(_shadowTexture->getTexture());
            spr.setOrigin(center);
            spr.setPosition(_position);

            target.draw(_sprite,st);
            target.draw(spr,states);
        }
        else
        {
            target.draw(_sprite,st);
        }
    }

    void SpotLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states)
    {
        if(_intensity == 0.0f) return;
        if(!isActive()) return;

        if(_spreadAngle == M_PIf*2.0f)
        {
            sf::CircleShape shape(_radius);
            shape.setPosition(_position);
            shape.setOrigin(sf::Vector2f(1,1)*_radius);
            shape.setFillColor(_color);
            if(isIsometric()) shape.setScale(1.0f,0.5f);
            target.draw(shape,states);
        }
        else
        {
            sf::ConvexShape shape = getShape();
            sf::ConvexShape shape2;
            shape2.setPointCount(shape.getPointCount());
            sf::Transform t = shape.getTransform();
            for(size_t i = 0; i < shape.getPointCount(); ++i)
            {
                shape2.setPoint(i, t.transformPoint(shape.getPoint(i)));
            }

            target.draw(shape2, states);
        }
    }

    void drawLine(sf::RenderTarget& target, const sf::Vector2f& a, const sf::Vector2f& b, const sf::Color& c)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(a,c),
            sf::Vertex(b,c)
        };
        target.draw(line,2,sf::Lines);
    }

    sf::Vector2f exportPoint(const sf::Vector2f& origin, const sf::Vector2f& point, float distance)
    {
        sf::Vector2f ans = point - origin;
        float norm = DMUtils::sfml::norm(ans);
        ans.x = ans.x / norm * distance + origin.x;
        ans.y = ans.y / norm * distance + origin.y;

        return ans;
    }

    void SpotLight::calcShadow(const std::list<Filter>& filters)
    {
        if(!_renderTexture || !_shadowTexture) return;
        //if(getSpreadAngle()!=360.0f) return;
        _shadowTexture->clear(sf::Color::White);

        const sf::Vector2f origin(getPosition());

        _shadows.clear();

        sf::FloatRect bounds = getBoundaries();
        //this is good if spreadAngle = 360 && boundaries is light->getAABB()
        sf::Vector2f screenDelta(-bounds.left,-bounds.top);

        float shadowLength = getRadius() * getRadius();
        sf::ConvexShape shp;
        shp.setPointCount(5);

        float radDiff;
        float radSum;
        float dist;

        for(const Light::Filter& f : filters)
        {
            {
                shp.setFillColor(f.filterColor);

                shp.setPoint(0, f.points[0]);
                shp.setPoint(1, exportPoint(origin, f.points[0], shadowLength));
                shp.setPoint(2, exportPoint(origin, f.middle, shadowLength));
                shp.setPoint(3, exportPoint(origin, f.points[1], shadowLength));
                shp.setPoint(4, f.points[1]);

                _shadows.emplace_back(shp);
                shp.setPosition(screenDelta);
                _shadowTexture->draw(shp);
            }
        }

        _shadowTexture->display();
    }

    void SpotLight::computeAABB()
    {
        if(_spreadAngle == M_PIf*2.0f)
        {
            _aabb.left = -_radius;
            _aabb.top = -_radius;
            _aabb.width = _aabb.height = _radius*2.0f;
        }
        else
        {
            // @TODO : move the rotation function to a real good dm::sfUtils folder
            sf::Vector2f v = DMUtils::sfml::rotate<float>(sf::Vector2f(0.0,_radius),_directionAngle);
            sf::Vector2f left = DMUtils::sfml::rotate<float>(v,-_spreadAngle/2.0f,_directionAngle);
            sf::Vector2f right = DMUtils::sfml::rotate<float>(v,_spreadAngle/2.0f,_directionAngle);

            int xmin = DMUtils::maths::min<float>(v.x, left.x, right.x, 0.0f);
            int xmax = DMUtils::maths::max<float>(v.x, left.x, right.x, 0.0f);
            int ymin = DMUtils::maths::min<float>(v.y, left.y, right.y, 0.0f);
            int ymax = DMUtils::maths::max<float>(v.y, left.y, right.y, 0.0f);

            _aabb.left = xmin;
            _aabb.top = ymin;
            _aabb.width = xmax - xmin;
            _aabb.height = ymax - ymin;
        }
    }

    sf::Color SpotLight::getLightColor(unsigned int x, unsigned int y)
    {
        sf::Color c;
        bool in = false;
        sf::Vector2f dist = sf::Vector2f(x,y) - _position;
        sf::Vector2f point(x, y);

//        std::cout << "shadow size : " << _shadows.size() << std::endl;
//        std::cout << "Point : " << x << ";" << y << std::endl;

        for(const sf::ConvexShape& shadow : _shadows)
        {
//            std::cout << shadow.getPoint(0).x << ";" << shadow.getPoint(0).y << std::endl;
            if(DMUtils::sfml::contains(shadow, point))
            {
                return sf::Color::Black;
            }
        }

//        std::cout << std::endl;

        if(_spreadAngle == M_PIf*2.0f)
        {
            in = (DMUtils::sfml::norm2(dist) <= _radius*_radius);
        }
        else
        {
            //this can be optimized
            in = DMUtils::sfml::contains(getShape(), point);
        }

        if(in)
        {
            float distance = DMUtils::sfml::norm(dist);
            float distFromFalloff = _radius - distance;
            float att =  distFromFalloff * (_bleed / (distance*distance) + _linearity / _radius);

            att = DMUtils::maths::clamp(att, 0.0f, 1.0f);

            c = sf::Color(att*_color.r*_intensity, att*_color.g*_intensity, att*_color.b*_intensity, 1.0f);
        }

        return c;
    }

    sf::FloatRect SpotLight::getBoundaries()
    {
        if(_spreadAngle == M_PIf*2.0f)
        {
            const sf::IntRect rect = getAABB();
            return sf::FloatRect(rect.left,rect.top,rect.width,rect.height);
        }
        return sf::FloatRect(_position.x - _radius, _position.y - _radius, _radius*2.0f, _radius*2.0f);
    }

    void SpotLight::setRadius(float r)
    {
        _radius = r;
    }

    float SpotLight::getRadius() const
    {
        return _radius;
    }

    void SpotLight::setDirectionAngle(float da)
    {
        _directionAngle = da;
        _sprite.setRotation(DMUtils::maths::radToDeg(_directionAngle));

        computeAABB();
    }

    float SpotLight::getDirectionAngle() const
    {
        return DMUtils::maths::radToDeg(_directionAngle);
    }

    void SpotLight::rotate(float delta)
    {
        float a =  _directionAngle + DMUtils::maths::degToRad(delta);
        while(a > 2.0*M_PIf) a -= 2.0*M_PIf;
        while(a < 0) a += 2.0*M_PIf;
        setDirectionAngle(a);
    }

    void SpotLight::setSpreadAngle(float sa)
    {
        sa = DMUtils::maths::degToRad(sa);
        if(sa<0) sa = -sa;
        while(sa > 2.0f*M_PIf) sa = 2.0f*M_PIf;
        _spreadAngle = sa;
    }

    float SpotLight::getSpreadAngle() const
    {
        return DMUtils::maths::radToDeg(_spreadAngle);
    }

    void SpotLight::setIntensity(float i)
    {
        _intensity = DMUtils::maths::clamp(i,-1.0f,1.0f);;
        setNegative(_intensity < 0.0f);
    }

    float SpotLight::getIntensity() const
    {
        return DMUtils::maths::abs(_intensity);
    }

    void SpotLight::setBleed(float b)
    {
        _bleed = b;
    }

    float SpotLight::getBleed() const
    {
        return _bleed;
    }

    void SpotLight::setLinearity(float lf)
    {
        _linearity = lf;
    }

    float SpotLight::getLinearity() const
    {
        return _linearity;
    }

    void SpotLight::setPrecision(int p)
    {
        _precision = p;
    }

    int SpotLight::getPrecision() const
    {
        return _precision;
    }

    bool SpotLight::getResizeWhenIncrease() const
    {
        return _resizeWhenIncrease;
    }

    void SpotLight::setResizeWhenIncrease(bool r)
    {
        _resizeWhenIncrease = r;
    }

    sf::ConvexShape SpotLight::getShape() const
    {
        sf::ConvexShape shape(_makeShape());
        shape.setRotation(getDirectionAngle());
        shape.setPosition(_position);
        shape.setOrigin(sf::Vector2f(_radius, _radius));

        return shape;
    }

    /*** PROTECTED ***/

    void SpotLight::_render(sf::RenderTarget& target, const sf::RenderStates& states, sf::Shader* shader, sf::Vector2f center, sf::Vector2f shapePosition, sf::Vector2f shapeOrigin, float shapeRotation)
    {

        sf::RenderStates st(states);
        st.shader = shader;

        shader->setParameter(dm::ls::Light::LAS_PARAM_CENTER,center);
        shader->setParameter(dm::ls::Light::LAS_PARAM_RADIUS,_radius);
        shader->setParameter(dm::ls::Light::LAS_PARAM_BLEED,_bleed);
        shader->setParameter(dm::ls::Light::LAS_PARAM_LINEARITY,_linearity);
        shader->setParameter(dm::ls::Light::LAS_PARAM_INTENSITY,std::abs(_intensity));

        sf::ConvexShape shape = _makeShape();

        if(_spreadAngle != M_PIf*2.0f)
            shape.setRotation(shapeRotation);

        shape.setPosition(shapePosition);
        shape.setOrigin(shapeOrigin);
        target.draw(shape,st);
    }

    sf::ConvexShape SpotLight::_makeShape() const
    {
        sf::ConvexShape shape;
        if(_spreadAngle==M_PIf*2.0f)
        {

            float diam = _radius*2.0f;

            shape.setPointCount(4);
            shape.setPoint(0,sf::Vector2f(0.0f,0.0f));
            shape.setPoint(1,sf::Vector2f(diam,0.0f));
            shape.setPoint(2,sf::Vector2f(diam,diam));
            shape.setPoint(3,sf::Vector2f(0.0f,diam));

        }
        else
        {
            float deltaAngle = _spreadAngle / (float)(_precision-1);

            shape.setPointCount(_precision+1);
            shape.setPoint(0,sf::Vector2f(_radius,_radius));

            for(int i=0; i<_precision; ++i)
            {
                float angle = - _spreadAngle/2.0f + (float)i*deltaAngle;
                shape.setPoint(i+1,DMUtils::sfml::rotate(shape.getPoint(0)+sf::Vector2f(0.0f,_radius),angle,shape.getPoint(0)));
            }
        }

        return shape;
    }
}
}
