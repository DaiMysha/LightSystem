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

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c, bool iso) : SpotLight(ctr,r,c,0.0f,2.0f*M_PIf,1.0f,0.0f,1.0f,iso) {
    }

    SpotLight::SpotLight(sf::Vector2f ctr, float r, sf::Color c, float da, float sa, float i, float b, float lf, bool iso) : Light(iso),
     _position(ctr), _radius(r), _color(c), _directionAngle(da), _spreadAngle(sa), _bleed(b), _linearity(lf) {
        setSpreadAngle(sa);
        setIntensity(i);
        computeAABB();

        //32 points for a full circle, so we keep same spacing
        if(_spreadAngle > M_PIf * 3.0f/4.0f) _precision = 32;
        else if(_spreadAngle > M_PIf) _precision = 16;
        else if (_spreadAngle < M_PIf/2.0f) _precision = 4;
        else _precision = 8;
    }

    void SpotLight::preRender(sf::Shader* shader) {
        if(shader==nullptr) return; //oopsie, can't work without the shader

        const float diam = _radius*2.0f;

        if(_renderTexture==nullptr) _renderTexture = new sf::RenderTexture();

        float r = _color.r * _intensity;
        float g = _color.g * _intensity;
        float b = _color.b * _intensity;

        if(diam != _renderTexture->getSize().x && !_renderTexture->create(diam,diam)) {
            std::cerr << "Error : couldn't create a texture of size " << diam << " x " << diam << std::endl;
            delete _renderTexture;
            _renderTexture=nullptr;
            return; //somehow texture failed, maybe too big, abort
        }

        sf::Color c(r,g,b,255);

        if(_spreadAngle==M_PIf*2.0f) {

            _sprite.setTexture(_renderTexture->getTexture());
            _sprite.setOrigin(sf::Vector2f(_radius,_radius));
            _sprite.setPosition(_position);

            sf::RectangleShape rect(sf::Vector2f(diam,diam));

            _renderTexture->clear();
            //shader parameters
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_CENTER,sf::Vector2f(_radius,_radius));
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_RADIUS,_radius);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_COLOR,c);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_BLEED,_bleed);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_LINEARITY,_linearity);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_OUTLINE,false); //for debug
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_ISOMETRIC,_isometric);

            _renderTexture->draw(rect,shader);
            _renderTexture->display();
        } else {

            _sprite.setTexture(_renderTexture->getTexture());
            _sprite.setPosition(_position);
            _sprite.setOrigin(sf::Vector2f(_radius,_radius));
            _renderTexture->clear();

            sf::ConvexShape shape;

            float deltaAngle = _spreadAngle / (float)(_precision-1);

            shape.setPointCount(_precision+1);
            shape.setPoint(0,sf::Vector2f(_radius,_radius));

            for(int i=0;i<_precision;++i) {
                float angle = _directionAngle - _spreadAngle/2.0f + (float)i*deltaAngle;
                shape.setPoint(i+1,DMUtils::sfml::rotate(shape.getPoint(0)+sf::Vector2f(0.0f,_radius),angle,shape.getPoint(0)));
            }

            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_CENTER,sf::Vector2f(_radius,_radius));
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_RADIUS,_radius);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_COLOR,c);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_BLEED,_bleed);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_LINEARITY,_linearity);
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_OUTLINE,true); //for debug
            shader->setParameter(DMGDVT::LS::Light::LAS_PARAM_ISOMETRIC,_isometric);

            _renderTexture->draw(shape,shader);
            _renderTexture->display();
        }
    }

    void SpotLight::render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states) {
        if(_intensity <= 0.0f) return;

        if(_renderTexture!=nullptr) {
            //draw the sprite
            target.draw(_sprite,states);
        } else {
            if(_spreadAngle == M_PIf*2.0f) {
            } else {
            }
        }
	}

    void SpotLight::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
        if(_intensity <= 0.0f) return;

        if(_spreadAngle == M_PIf*2.0f) {
            sf::CircleShape shape(_radius);
            shape.setPosition(_position);
            shape.setOrigin(sf::Vector2f(1,1)*_radius);
            shape.setFillColor(_color);

            target.draw(shape,states);
        } else {
            sf::ConvexShape shape;
            shape.setPointCount(4);
            shape.setFillColor(sf::Color(_color.r,_color.g,_color.b,125));
            sf::Vector2f v(0,_radius);
            //*
            shape.setPosition(_position);
            shape.setPoint(0,sf::Vector2f(0,0));
            shape.setPoint(1,DMUtils::sfml::rotate(v,-_spreadAngle/2.0f));
            shape.setPoint(2,v);
            shape.setPoint(3,DMUtils::sfml::rotate(v,_spreadAngle/2.0f));

            shape.setRotation(DMUtils::maths::radToDeg(_directionAngle));

            target.draw(shape,states);
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
        return sf::IntRect(sf::Vector2i(_aabb.left+static_cast<int>(_position.x),_aabb.top+static_cast<int>(_position.y)),sf::Vector2i(_aabb.width,_aabb.height));
    }

    void SpotLight::setPosition(sf::Vector2f c) {
        _position = c;
        _sprite.setPosition(c);
	}

    sf::Vector2f SpotLight::getPosition() const {
        return _position;
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

    void SpotLight::rotate(float delta) {
        _directionAngle += delta;
        while(_directionAngle > 2.0*M_PIf) _directionAngle -= 2.0*M_PIf;
        while(_directionAngle < 0) _directionAngle += 2.0*M_PIf;
        _sprite.setRotation(DMUtils::maths::radToDeg(_directionAngle)-180.0f);
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

    void SpotLight::setPrecision(int p) {
        _precision = p;
    }

    int SpotLight::getPrecision() const {
        return _precision;
    }

	/*** PROTECTED ***/

}
}
