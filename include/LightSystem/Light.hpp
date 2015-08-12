#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

#include <SFML/Graphics.hpp>

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            Light(bool iso = false);
            virtual ~Light();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) = 0;
            virtual void preRender(sf::Shader* shader) = 0;//for light manager mainly
            virtual void debugRender(sf::RenderTarget& target, const sf::RenderStates &states);
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) = 0;

            virtual void computeAABB() = 0;

            virtual sf::IntRect getAABB() = 0;//returns the AABB according to the whole map

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

            //change to unique_ptr ?
            sf::RenderTexture* _renderTexture;
            sf::Sprite _sprite;

            bool _isometric;
    };
}
}

#endif // HEADER_DMGDVT_LIGHT

