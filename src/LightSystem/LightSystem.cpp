#include <iostream>

#include <SFML/OpenGL.hpp>

#include <DMUtils/sfml.hpp>

#include <LightSystem/LightSystem.hpp>

///See if you can do somethiing with viewports instead of moving everything by hand

namespace DMGDVT {
namespace LS {

    LightSystem::LightSystem() : _multiplyState(sf::BlendMultiply) {
        sf::Shader _lightAttenuationShader;
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

    void LightSystem::addLight(Light* l, bool dynamic) {
        if(!dynamic) l->preRender(&_lightAttenuationShader);
        _lights.push_back(l);
    }

    void LightSystem::reset() {
        for(Light* l : _lights) delete l;

        _lights.empty();
    }

    void LightSystem::render(const sf::View& screen, sf::RenderTarget& target) {
        render(DMUtils::sfml::getViewInWorldAABB(screen),target);
    }

    void LightSystem::render(const sf::IntRect& screen, sf::RenderTarget& target) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->render(screen,target,&_lightAttenuationShader,_multiplyState);
        }
    }

    void LightSystem::drawAABB(const sf::View& screen, sf::RenderTarget& target) {
        drawAABB(DMUtils::sfml::getViewInWorldAABB(screen),target);
    }

    void LightSystem::drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,target);
        }
    }

}
}
