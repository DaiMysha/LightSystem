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

#include <SFML/OpenGL.hpp>

#include <DMUtils/sfml.hpp>

#include <LightSystem/LightSystem.hpp>
#include <LightSystem/staticData/staticData.hpp>
#include <LightSystem/ShadowSystem.hpp>

namespace dm {
namespace ls {

    const sf::RenderStates LightSystem::_multiplyState(sf::BlendMultiply);
    const sf::RenderStates LightSystem::_addState(sf::BlendAdd);
    const sf::RenderStates LightSystem::_subtractState(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::One, sf::BlendMode::ReverseSubtract));

    LightSystem::LightSystem() : _ambiant(sf::Color::Black), _isometric(false), _autoDelete(true), _updateLightMapImage(true) {

        /*if(!_lightAttenuationShader.loadFromFile("shaders/lightAttenuation.frag",sf::Shader::Fragment)) {
            std::cerr << "Missing light attenuation Shader. System won't work" << std::endl;
        }*/

        if(!_lightAttenuationShader.loadFromMemory(dm::ls::StaticData::LIGHT_ATTENUATION_SHADER,sf::Shader::Fragment)) {
            std::cerr << "Missing light attenuation Shader. System won't work" << std::endl;
        }

        _shadowSystem = new ShadowSystem();

    }

    LightSystem::~LightSystem() {
        reset();
        delete _shadowSystem;
    }

    void LightSystem::addLight(Light* l) {
        if(l==nullptr) return;

        l->setIsometric(_isometric);//ignore what user set before
        l->preRender(&_lightAttenuationShader);

        if(l->isEmissive()) _emissiveLights.emplace_back(l);
        else if(l->isNegative()) _negativeLights.emplace_back(l);
        else _lights.emplace_back(l);

        l->setSystem(this);

        _updateLightMapImage = true;
    }

    void LightSystem::removeLight(Light* l) {
        if(l==nullptr) return;

        if(l->isEmissive()) _emissiveLights.remove(l);
        else if(l->isNegative()) _negativeLights.remove(l);
        else _lights.remove(l);
    }

    void LightSystem::reset() {

        for(Light* l : _lights) {
            l->setSystem(nullptr);
            if(_autoDelete) delete l;
        }
        for(Light* l : _negativeLights) {
            l->setSystem(nullptr);
            if(_autoDelete) delete l;
        }
        for(Light* l : _emissiveLights) {
            l->setSystem(nullptr);
            if(_autoDelete) delete l;
        }

        _lights.empty();
        _negativeLights.empty();
        _emissiveLights.empty();

        if(_shadowSystem) _shadowSystem->clear();
    }

    void LightSystem::addWall(const sf::ConvexShape& s) {
        if(_shadowSystem) _shadowSystem->addWall(s);
    }

    void LightSystem::render(const sf::View& screenView, sf::RenderTarget& target) {
        debugRender(screenView,target,LightSystem::DebugFlags::DEFAULT);
    }

    void LightSystem::debugRender(const sf::View& screenView, sf::RenderTarget& target, int flags) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);

        _sprite.setPosition(screen.left,screen.top);

        _renderTexture.clear(_ambiant);
        sf::RenderStates stAdd(_addState);
        sf::RenderStates stRm(_subtractState);
        sf::RenderStates stMp(_multiplyState);

        sf::Transform t;
        t.translate(-_sprite.getPosition());
        stAdd.transform.combine(t);
        stRm.transform.combine(t);
        stMp.transform.combine(t);

        sf::FloatRect screenRect(screen);

        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DebugFlags::SHADER_OFF) l->debugRender(_renderTexture,stAdd);
                else {
                    _buffer.clear(sf::Color::Black);
                    //sf::FloatRect rect(l->getAABB().left,l->getAABB().top,l->getAABB().width,l->getAABB().height);
                    l->calcShadow(_shadowSystem->getWalls());
                    //l->render(screen,_renderTexture,&_lightAttenuationShader,stAdd);
                    l->render(screen,_buffer,&_lightAttenuationShader,stMp);
                    _buffer.display();
                    _renderTexture.draw(_bufferSprite,_addState);
                }
            }
        }
        for(Light* l : _negativeLights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DebugFlags::SHADER_OFF) l->debugRender(_renderTexture,stRm);
                else {
                    _buffer.clear(sf::Color::Black);
                    //sf::FloatRect rect(l->getAABB().left,l->getAABB().top,l->getAABB().width,l->getAABB().height);
                    l->calcShadow(_shadowSystem->getWalls());
                    //l->render(screen,_renderTexture,&_lightAttenuationShader,stAdd);
                    l->render(screen,_buffer,&_lightAttenuationShader,stMp);
                    _buffer.display();
                    _renderTexture.draw(_bufferSprite,_subtractState);
                }
            }
        }

        _renderTexture.display();

        _updateLightMapImage = true;
        if(flags & DebugFlags::LIGHTMAP_ONLY) target.clear(sf::Color::White);
    }

    void LightSystem::draw(const sf::View& screenView, sf::RenderTarget& target) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);
        target.draw(_sprite,_multiplyState);
        for(Light* l : _emissiveLights) {
            if(l->getAABB().intersects(screen)) {
                l->render(screen,target,nullptr);
            }
        }
    }

    void LightSystem::drawAABB(const sf::View& screen, sf::RenderTarget& target) {
        drawAABB(DMUtils::sfml::getViewInWorldAABB(screen),target);
    }

    void LightSystem::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
        for(Light* l : _negativeLights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
        for(Light* l : _emissiveLights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
    }

    void LightSystem::drawWalls(const sf::View& screenView, sf::RenderTarget& target) {
        //sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);
        if(_shadowSystem) {
            //for(Light* l : _lights) if(l->getAABB().intersects(screen)) _shadowSystem->debugDraw(l,screenView,target);
            //for(Light* l : _negativeLights) if(l->getAABB().intersects(screen)) _shadowSystem->debugDraw(l,screenView,target);
            _shadowSystem->draw(screenView,target);
        }
    }

    void LightSystem::update() {
        for(Light* l : _lights) update(l);
        for(Light* l : _negativeLights) update(l);
        for(Light* l : _emissiveLights) update(l);
    }

    void LightSystem::update(Light* l) {
        l->preRender(&_lightAttenuationShader);
        _updateLightMapImage = true;
    }

    size_t LightSystem::getLightsCount() const {
        return getNormalLightsCount() + getNegativeLightsCount() + getEmissiveLightsCount();
    }

    size_t LightSystem::getNormalLightsCount() const {
        return _lights.size();
    }

    size_t LightSystem::getNegativeLightsCount() const {
        return _negativeLights.size();
    }

    size_t LightSystem::getEmissiveLightsCount() const {
        return _emissiveLights.size();
    }

    sf::Image LightSystem::getLightMap() {
        if(_updateLightMapImage) {
            _lightMapImage = _renderTexture.getTexture().copyToImage();
            _updateLightMapImage = false;
        }
        return _lightMapImage;
    }

    sf::Color LightSystem::getLightMapPixel(const sf::View& view, unsigned int x, unsigned int y) {
        sf::Image lightMap = getLightMap();
        x -= view.getViewport().left;
        y -= view.getViewport().top;
        if(x>=0&&y>=0&&x<lightMap.getSize().x&&y<lightMap.getSize().y) return lightMap.getPixel(x,y);
        else return sf::Color::Black;
    }

    sf::Color LightSystem::getLightMapPixel(const sf::View& view, sf::Vector2f p) {
        return getLightMapPixel(view,p.x,p.y);
    }

    void LightSystem::setAmbiantLight(sf::Color c) {
        _ambiant = c;
    }

    sf::Color LightSystem::getAmbiantLight() const {
        return _ambiant;
    }

    /*void LightSystem::setIsometric(bool i) {
        _isometric = i;
        update();
    }

    bool LightSystem::isIsometric() const {
        return _isometric;
    }*/

    void LightSystem::setAutoDelete(bool ad) {
        _autoDelete = ad;
    }

    void LightSystem::setView(const sf::View& view) {
        _renderTexture.create(view.getSize().x,view.getSize().y);
        _buffer.create(view.getSize().x,view.getSize().y);

        _sprite.setTexture(_renderTexture.getTexture(),true);
        _bufferSprite.setTexture(_buffer.getTexture(),true);
    }
}
}
