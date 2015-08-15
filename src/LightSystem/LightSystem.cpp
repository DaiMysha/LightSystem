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

namespace DMGDVT {
namespace LS {

    const sf::RenderStates LightSystem::_multiplyState(sf::BlendMultiply);
    const sf::RenderStates LightSystem::_addState(sf::BlendAdd);
    //const sf::RenderStates LightSystem::_subtractState(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::One, sf::BlendMode::ReverseSubtract));
    const sf::RenderStates LightSystem::_subtractState(sf::BlendMode(sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcColor, sf::BlendMode::Add));

    LightSystem::LightSystem(bool isometric) : _ambiant(sf::Color::Black), _isometric(isometric), _autoDelete(true) {

        /*if(!_lightAttenuationShader.loadFromFile("shaders/lightAttenuation.frag",sf::Shader::Fragment)) {
            std::cerr << "Missing light attenuation Shader. System won't work" << std::endl;
        }*/

        if(!_lightAttenuationShader.loadFromMemory(DMGDVT::LS::StaticData::LIGHT_ATTENUATION_SHADER,sf::Shader::Fragment)) {
            std::cerr << "Missing light attenuation Shader. System won't work" << std::endl;
        }

    }

    LightSystem::~LightSystem() {
        reset();
    }

    void LightSystem::addLight(Light* l) {
        l->setIsometric(_isometric);//ignore what user set before
        l->preRender(&_lightAttenuationShader);
        if(l->isNegative()) _negativeLights.emplace_back(l);
        else _lights.emplace_back(l);
    }

    void LightSystem::addLight(EmissiveLight* l) {
        l->setIsometric(_isometric);//ignore what user set before
        l->preRender(&_lightAttenuationShader);
        _emissiveLights.emplace_back(l);
    }

    void LightSystem::removeLight(Light* l) {
        if(l->isNegative()) _negativeLights.remove(l);
        else _lights.remove(l);
    }

    void LightSystem::reset() {
        if(_autoDelete) for(Light* l : _lights) delete l;

        _lights.empty();
    }

    void LightSystem::render(const sf::View& screenView, sf::RenderTarget& target) {
        debugRender(screenView,target,LightSystem::DebugFlags::Default);
    }

    void LightSystem::debugRender(const sf::View& screenView, sf::RenderTarget& target, int flags) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);

        _sprite.setPosition(screen.left,screen.top);

        _renderTexture.clear(_ambiant);
        sf::RenderStates stAdd(_addState);
        sf::RenderStates stRm(_subtractState);
        sf::Transform t;
        t.translate(-_sprite.getPosition());
        stAdd.transform.combine(t);
        stRm.transform.combine(t);
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DebugFlags::Shader_off) l->debugRender(_renderTexture,(stAdd));
                else l->render(screen,_renderTexture,&_lightAttenuationShader,(stAdd));
            }
        }
        for(Light* l : _negativeLights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DebugFlags::Shader_off) l->debugRender(_renderTexture,(stRm));
                else l->render(screen,_renderTexture,&_lightAttenuationShader,(stRm));
            }
        }

        _renderTexture.display();

        if(flags & DebugFlags::LightMap_only) target.clear(sf::Color::White);
    }

    void LightSystem::draw(const sf::View& screenView, sf::RenderTarget& target) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);
        target.draw(_sprite,_multiplyState);
        for(EmissiveLight* l : _emissiveLights) {
            if(l->getAABB().intersects(screen)) l->render(screen,target,nullptr);
        }
    }

    void LightSystem::drawAABB(const sf::View& screen, sf::RenderTarget& target) {
        drawAABB(DMUtils::sfml::getViewInWorldAABB(screen),target);
    }

    void LightSystem::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
        for(EmissiveLight* l : _emissiveLights) if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
    }

    void LightSystem::update() {
        for(Light* l : _lights) update(l);
        for(Light* l : _negativeLights) update(l);
    }

    void LightSystem::update(Light* l) {
        bool wasNegative = l->isNegative();
        l->preRender(&_lightAttenuationShader);
        bool isNegative = l->isNegative();
        if(wasNegative xor isNegative) {
            if(isNegative) {
                _lights.remove(l);
                _negativeLights.emplace_back(l);
            } else {
                _negativeLights.remove(l);
                _lights.emplace_back(l);
            }
        }
    }

    size_t LightSystem::getLightsCount() const {
        return getNormalLightsCount() + getNegativeLightsCount();
    }

    size_t LightSystem::getNormalLightsCount() const {
        return _lights.size();
    }

    size_t LightSystem::getNegativeLightsCount() const {
        return _negativeLights.size();
    }

    sf::Image LightSystem::getLightMap() const {
        return _renderTexture.getTexture().copyToImage();
    }

    sf::Color LightSystem::getLightMapPixel(const sf::View& view, int x, int y) const {
        x -= view.getViewport().left;
        y -= view.getViewport().top;
        return getLightMap().getPixel(x,y);
    }

    sf::Color LightSystem::getLightMapPixel(const sf::View& view, sf::Vector2f p) const {
        return getLightMapPixel(view,p.x,p.y);
    }

    void LightSystem::setAmbiantLight(sf::Color c) {
        _ambiant = c;
    }

    sf::Color LightSystem::getAmbiantLight() const {
        return _ambiant;
    }

    void LightSystem::setIsometric(bool i) {
        _isometric = i;
        update();
    }

    bool LightSystem::isIsometric() const {
        return _isometric;
    }

    void LightSystem::setAutoDelete(bool ad) {
        _autoDelete = ad;
    }

    void LightSystem::setView(const sf::View& view) {
        _renderTexture.create(view.getSize().x,view.getSize().y);
        _sprite.setTexture(_renderTexture.getTexture());
    }
}
}
