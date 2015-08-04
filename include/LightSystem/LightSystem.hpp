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

            void render(AABB screen, sf::RenderWindow& window);
            void render(int x, int y, int w, int h, sf::RenderWindow& window);
            void drawAABB(AABB screen, sf::RenderWindow& window);
            void drawAABB(int x, int y, int w, int h, sf::RenderWindow& window);

        private:
            std::list<Light*> _lights;

    };
}
}

#endif // HEADER_DMGDVT_LIGHTSYSTEM

