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

namespace DMGDVT {
namespace LS {

    const sf::RenderStates LightSystem::_multiplyState(sf::BlendMultiply);
    const sf::RenderStates LightSystem::_addState(sf::BlendAdd);
    //const sf::RenderStates LightSystem::_subtractState(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::One, sf::BlendMode::ReverseSubtract));
    const sf::RenderStates LightSystem::_subtractState(sf::BlendMode(sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcColor, sf::BlendMode::Add));

    LightSystem::LightSystem(bool isometric) : _ambiant(sf::Color::Black), _isometric(isometric), _autoDelete(true) {
        //this will be loaded from internal memory when lib is created
        //or loaded external crypted
        //the idea is not to allow the user to modify it
        if(!_lightAttenuationShader.loadFromFile("shaders/lightAttenuation.frag",sf::Shader::Fragment)) {
            std::cerr << "Missing light attenuation Shader. System won't work" << std::endl;
        }

    }

    LightSystem::~LightSystem() {
        reset();
    }

    void LightSystem::addLight(Light* l) {
        l->preRender(&_lightAttenuationShader);
        l->setIsometric(_isometric);//ignore what user set before
        _lights.emplace_back(l);
    }

    void LightSystem::removeLight(Light* l) {
        _lights.remove(l);
    }

    void LightSystem::reset() {
        if(_autoDelete) for(Light* l : _lights) delete l;

        _lights.empty();
    }

    void LightSystem::render(const sf::View& screenView, sf::RenderTarget& target) {
        debugRender(screenView,target,LightSystem::DEBUG_FLAGS::DEFAULT);
    }

    void LightSystem::debugRender(const sf::View& screenView, sf::RenderTarget& target, int flags) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);

        _sprite.setPosition(screen.left,screen.top);

        _renderTexture.clear(_ambiant);
        sf::RenderStates stAdd(_addState);
        sf::RenderStates stRm(_subtractState);
        sf::Transform t;
        t.translate(-_sprite.getPosition());
        stAdd.transform = t;
        stRm.transform = t;
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DEBUG_FLAGS::SHADER_OFF) l->debugRender(_renderTexture,(l->isNegative()?stRm:stAdd));
                else l->render(screen,_renderTexture,&_lightAttenuationShader,(l->isNegative()?stRm:stAdd));
            }
        }

        _renderTexture.display();

        if(flags & DEBUG_FLAGS::LIGHTMAP_ONLY) target.clear(sf::Color::White);
    }

    void LightSystem::draw(const sf::View& screenView, sf::RenderTarget& target) {
        target.draw(_sprite,_multiplyState);
    }

    void LightSystem::drawAABB(const sf::View& screen, sf::RenderTarget& target) {
        drawAABB(DMUtils::sfml::getViewInWorldAABB(screen),target);
    }

    void LightSystem::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
    }

    void LightSystem::update() {
        for(Light* ls : _lights) ls->preRender(&_lightAttenuationShader);
    }

    void LightSystem::update(Light* l) {
        l->preRender(&_lightAttenuationShader);
    }

    void LightSystem::setAmbiantLight(sf::Color c) {
        _ambiant = c;
    }

    sf::Color LightSystem::getAmbiantLight() const {
        return _ambiant;
    }

    void LightSystem::setIsometric(bool i) {
        _isometric = i;
        for(Light* l : _lights) {
            l->setIsometric(_isometric);
            update(l);
        }
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
