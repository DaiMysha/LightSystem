#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

#include <SFML/Graphics.hpp>

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            //empty body here to avoid compilation error
            //will move to .cpp as soon as functions appear here
            Light(bool iso = false);
            virtual ~Light();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) = 0;
            virtual void preRender(sf::Shader* shader) = 0;//for light manager mainly
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) = 0;

            virtual void computeAABB() = 0;

            //returns the AABB according to the whole game
            virtual sf::IntRect getAABB() = 0;

            virtual bool isIsometric() const final;

            virtual void setIsometric(bool i) final;

        protected:
            static const char LAS_PARAM_CENTER[];
            static const char LAS_PARAM_RADIUS[];
            static const char LAS_PARAM_COLOR[];
            static const char LAS_PARAM_BLEED[];
            static const char LAS_PARAM_LINEARITY[];
            static const char LAS_PARAM_OUTLINE[];//used mainly for debug, don't mind it
            static const char LAS_PARAM_ISOMETRIC[];

            sf::IntRect _aabb;

            sf::RenderTexture* _renderTexture;
            sf::Sprite _sprite;

            //used later
            bool _isometric;
    };
}
}

#endif // HEADER_DMGDVT_LIGHT

