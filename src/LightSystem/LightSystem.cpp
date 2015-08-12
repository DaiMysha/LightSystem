#include <iostream>

#include <SFML/OpenGL.hpp>

#include <DMUtils/sfml.hpp>

#include <LightSystem/LightSystem.hpp>

///See if you can do somethiing with viewports instead of moving everything by hand

namespace DMGDVT {
namespace LS {

    LightSystem::LightSystem(bool isometric) : _ambiant(sf::Color::Black), _multiplyState(sf::BlendMultiply), _addState(sf::BlendAdd), _isometric(isometric), _autoDelete(true) {
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
        l->setIsometric(_isometric);//ignore what user set before
        _lights.emplace_back(l);
    }

    void LightSystem::reset() {
        if(_autoDelete) for(Light* l : _lights) delete l;

        _lights.empty();
    }

    //won't need that liive anymore
    void LightSystem::render(const sf::View& screenView, sf::RenderTarget& target) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);

        _sprite.setPosition(screen.left,screen.top);

        _renderTexture.clear(_ambiant);
        sf::RenderStates st(_addState);
        sf::Transform t;
        t.translate(-_sprite.getPosition());
        st.transform = t;
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) l->render(screen,_renderTexture,&_lightAttenuationShader,st);
        }

        _renderTexture.display();
    }

    void LightSystem::debugRender(const sf::View& screenView, sf::RenderTarget& target, int flags) {
        sf::IntRect screen = DMUtils::sfml::getViewInWorldAABB(screenView);

        _sprite.setPosition(screen.left,screen.top);

        _renderTexture.clear(_ambiant);
        sf::RenderStates st(_addState);
        sf::Transform t;
        t.translate(-_sprite.getPosition());
        st.transform = t;
        for(Light* l : _lights) {
            if(l->getAABB().intersects(screen)) {
                if(flags & DEBUG_FLAGS::SHADER_OFF) l->debugRender(_renderTexture,st);
                else l->render(screen,_renderTexture,&_lightAttenuationShader,st);
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
        for(Light* l : _lights) l->setIsometric(_isometric);
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
