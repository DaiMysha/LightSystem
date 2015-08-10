#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

#include <SFML/Graphics.hpp>

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            //empty body here to avoid compilation error
            //will move to .cpp as soon as functions appear here
            Light(bool iso = false) : _aabb(), _renderTexture(nullptr), _isometric(iso) {
            }
            virtual ~Light() {}

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default) = 0;
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target) = 0;

            virtual void computeAABB() = 0;

            //returns the AABB according to the whole game
            virtual sf::IntRect getAABB() = 0;

            virtual bool isIsometric() const final {
                return _isometric;
            }

            virtual void setIsometric(bool i) final {
                _isometric = i;
            }

        protected:
            sf::IntRect _aabb;

            sf::RenderTexture* _renderTexture;
            sf::Sprite _sprite;

            //used later
            bool _isometric;
    };
}
}

#endif // HEADER_DMGDVT_LIGHT

