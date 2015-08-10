#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>

#include <Lightsystem/SpotLight.hpp>


namespace DMGDVT {
namespace LS {

    SpotLight::SpotLight(bool iso) : SpotLight(sf::Vector2f(0,0),0,sf::Color(0,0,0,0)) {
	}

    SpotLight::~SpotLight() {
	}

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c, bool iso) : SpotLight(ctr,r,c,0.0f,M_PIf,1.0f,0.0f,0.0f,0.0f) {
    }

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c, float da, float sa, float i, float b, float lf, bool iso) : Light(iso),
     _center(ctr), _radius(r), _color(c), _directionAngle(da), _spreadAngle(sa), _bleed(b), _linearity(lf) {
        setSpreadAngle(sa);
        setIntensity(i);
        computeAABB();
    }

    void SpotLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states) {
        if(_intensity <= 0.0f) return;

        debugRender(screen,target);

        if(_renderTexture!=nullptr) {
            //draw the sprite
            //_sprite.setPosition(_center);
            /*std::cout << "shape : " << _sprite.getPosition().x << ";" << _sprite.getPosition().y << std::endl;
            target.draw(_sprite,states);

            sf::CircleShape shape(_radius);
            shape.setPosition(_center);
            shape.setFillColor(_color);

            target.draw(shape);*/
        } else {
            if(_spreadAngle == M_PIf*2.0f) {
            } else {
            }
        }
	}

    void SpotLight::preRender(sf::Shader* shader) {
        if(shader==nullptr) return; //oopsie, can't work without the shader
        if(_renderTexture!=nullptr) delete _renderTexture;

        _renderTexture = new sf::RenderTexture();
        if(_renderTexture == nullptr) return; //couldn't allocate somehow, return
        if(!_renderTexture->create(_radius,_radius)) return; //somehow texture failed, maybe too big, abort

        //actual rendering code (finally!)
        sf::VertexArray rect(sf::Quads,4);
        rect[0].position = sf::Vector2f(0,0);
        rect[1].position = sf::Vector2f(0,_radius);
        rect[2].position = sf::Vector2f(_radius,_radius);
        rect[3].position = sf::Vector2f(_radius,0);
        rect[0].texCoords = sf::Vector2f(0.0,0.0);
        rect[1].texCoords = sf::Vector2f(0.0,1.0);
        rect[2].texCoords = sf::Vector2f(1.0,1.0);
        rect[3].texCoords = sf::Vector2f(1.0,0.0);

        float r = _color.r * _intensity;
        float g = _color.g * _intensity;
        float b = _color.b * _intensity;

        sf::Color c(r,g,b,1.0);

        _renderTexture->clear();
        //shader parameters
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_CENTER,sf::Vector2f(_radius/2.0f,_radius/2.0f));
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_RADIUS,_radius);
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_COLOR,c);
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_BLEED,_bleed);
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_LINEARITY,_linearity);
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_OUTLINE,true); //for debug
        shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_ISOMETRIC,_isometric); //for debug

        _renderTexture->draw(rect,shader);

        _sprite.setTexture(_renderTexture->getTexture());
    }

    //keep this as debug option, not used for now at all
    void SpotLight::debugRender(const sf::IntRect& screen, sf::RenderTarget& target) {
        if(_intensity <= 0.0f) return;

        if(_spreadAngle == M_PIf*2.0f) {
            sf::CircleShape shape(_radius);
            shape.setPosition(_center);
            shape.setOrigin(sf::Vector2f(1,1)*_radius);
            shape.setFillColor(_color);

            target.draw(shape);
        } else {
            sf::ConvexShape shape;
            shape.setPointCount(4);
            shape.setFillColor(sf::Color(_color.r,_color.g,_color.b,125));
            sf::Vector2f v(0,_radius);
            //*
            shape.setPosition(_center);
            shape.setPoint(0,sf::Vector2f(0,0));
            shape.setPoint(1,DMUtils::sfml::rotate(v,-_spreadAngle/2.0f));
            shape.setPoint(2,v);
            shape.setPoint(3,DMUtils::sfml::rotate(v,_spreadAngle/2.0f));

            shape.setRotation(_directionAngle*180.0f/M_PIf);

            target.draw(shape);
        }
	}

    void SpotLight::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        sf::IntRect box = getAABB();
        sf::Vertex lines[] = {
            sf::Vertex(sf::Vector2f(box.left, box.top),_color),
            sf::Vertex(sf::Vector2f(box.left+box.width, box.top),_color),
            sf::Vertex(sf::Vector2f(box.left+box.width, box.top+box.height),_color),
            sf::Vertex(sf::Vector2f(box.left, box.top+box.height),_color),
            lines[0]
        };

        target.draw(lines,5,sf::LinesStrip);
    }

    void SpotLight::computeAABB() {

        if(_spreadAngle == M_PIf*2.0f) {
            _aabb.left = -_radius;
            _aabb.top = -_radius;
            _aabb.width = _aabb.height = _radius*2.0f;
        } else {
            // @TODO : move the rotation function to a real good DMGDVT::sfUtils folder
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

    /*** GETTER - SETTER ***/
    sf::IntRect SpotLight::getAABB() {
        return sf::IntRect(sf::Vector2i(_aabb.left+static_cast<int>(_center.x),_aabb.top+static_cast<int>(_center.y)),sf::Vector2i(_aabb.width,_aabb.height));
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

    void SpotLight::setBleed(float b) {
        _bleed = b;
	}

    float SpotLight::getBleed() const {
        return _bleed;
	}

    void SpotLight::setLinearity(float lf) {
        _linearity = lf;
	}

    float SpotLight::getLinearity() const {
        return _linearity;
	}

	/*** PROTECTED ***/

}
}
