/* DaiMysha GameDeV Tools
*  Light System
*
*
*
*
*
*   This is meant to be used with the SFML graphic library
*/

#ifndef HEADER_DMGDVT_LIGHTSYSTEM
#define HEADER_DMGDVT_LIGHTSYSTEM

#include <list>

#include <SFML/Graphics.hpp>

#include "Light.hpp"

namespace DMGDVT {
namespace LS {
    class LightSystem {
        public:
            LightSystem();
            ~LightSystem();

            void addLight(Light* l);
            //empties the lights
            void reset();

            void render(const sf::View& screen, sf::RenderTarget& target);
            void render(const sf::IntRect& screen, sf::RenderTarget& target);
            void drawAABB(const sf::View& screen, sf::RenderTarget& target);
            void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);

        private:
            std::list<Light*> _lights;
            sf::RenderStates _multiplyState;

    };
}
}

#endif // HEADER_DMGDVT_LIGHTSYSTEM

