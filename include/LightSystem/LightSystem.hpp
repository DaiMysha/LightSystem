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
    ///this class is not meant to be inherited
    class LightSystem {
        public:
            LightSystem(bool isometric = false);
            LightSystem(const LightSystem& ls) = delete; //doesn't make sense to copy an existing light system
            LightSystem(LightSystem&& ls) = delete; //doesn't make sense to copy an existing light system
            Lightsystem& operator=(const LightSystem& ls) = delete;
            Lightsystem& operator=(LightSystem&& ls) = delete;

            ~LightSystem();

            void addLight(Light* l, bool dynamic = false);
            //empties the lights
            void reset();

            void render(const sf::View& screen, sf::RenderTarget& target);
            void render(const sf::IntRect& screen, sf::RenderTarget& target);
            void drawAABB(const sf::View& screen, sf::RenderTarget& target);
            void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);

            void setIsometric(bool i);
            bool isIsometric() const;

            void setAutoDelete(bool ad);

        protected:

            std::list<Light*> _lights;
            sf::RenderStates _multiplyState;
            sf::Shader _lightAttenuationShader;

            bool _isometric;
            bool _autoDelete;

    };
}
}

#endif // HEADER_DMGDVT_LIGHTSYSTEM

