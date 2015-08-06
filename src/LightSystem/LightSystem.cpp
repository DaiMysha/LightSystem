#include <iostream>

#include <SFML/OpenGL.hpp>

#include <DMUtils/sfml.hpp>

#include <LightSystem/LightSystem.hpp>

///See if you can do somethiing with viewports instead of moving everything by hand

namespace DMGDVT {
namespace LS {

    LightSystem::LightSystem() {
    }

    LightSystem::~LightSystem() {
        reset();
    }

    void LightSystem::addLight(Light* l) {
        _lights.push_back(l);
    }

    void LightSystem::reset() {
        for(Light* l : _lights) delete l;

        _lights.empty();
    }

    void LightSystem::render(const sf::View& screen, sf::RenderWindow& window) {
        render(DMUtils::sfml::getViewInWorldAABB(screen),window);
    }

    void LightSystem::render(const sf::IntRect& screen, sf::RenderWindow& window) {
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->render(screen,window);
        }
    }

    void LightSystem::drawAABB(const sf::View& screen, sf::RenderWindow& window) {
        drawAABB(DMUtils::sfml::getViewInWorldAABB(screen),window);
    }

    void LightSystem::drawAABB(const sf::IntRect& screen, sf::RenderWindow& window) {
        window.setActive(true);
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->drawAABB(screen,window);
        }
    }

}
}
