#include <iostream>

#include <SFML/OpenGL.hpp>

#include "LightSystem.hpp"

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

    void LightSystem::render(AABB screen, sf::RenderWindow& window) {
        for(Light* l : _lights) {
            if(l->getAABB().collides(screen)) l->render(screen,window);
        }
    }

    void LightSystem::render(int x, int y, int w, int h, sf::RenderWindow& window) {
        AABB box;
        box.x=x;
        box.y=y;
        box.w=w;
        box.h=h;
        render(box,window);
    }

    void LightSystem::drawAABB(AABB screen, sf::RenderWindow& window) {
        window.setActive(true);
        for(Light* l : _lights) {
            if(l->getAABB().collides(screen)) l->drawAABB(screen,window);
        }
    }

    void LightSystem::drawAABB(int x, int y, int w, int h, sf::RenderWindow& window) {
        AABB box;
        box.x=x;
        box.y=y;
        box.w=w;
        box.h=h;
        drawAABB(box,window);
    }

}
}
