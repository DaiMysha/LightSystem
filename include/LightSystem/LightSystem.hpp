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
            LightSystem& operator=(const LightSystem& ls) = delete;
            LightSystem& operator=(LightSystem&& ls) = delete;

            ~LightSystem();

            void addLight(Light* l, bool dynamic = false);
            /*template <typename T, bool D, typename ... Args>
            void addLight(Args&& ... args) {
                addLight(new T(std::forward<Args>(args)...),D);
            }*/
            //empties the lights
            void reset();

            //call this function to prepare the render
            void render(const sf::View& screenView, sf::RenderTarget& target);
            void debugRender(const sf::View& screenView, sf::RenderTarget& target);
            void draw(const sf::View& screenView, sf::RenderTarget& target);
            void drawAABB(const sf::View& screenView, sf::RenderTarget& target);
            void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);

            void setIsometric(bool i);
            bool isIsometric() const;

            void setAutoDelete(bool ad);

            void setView(const sf::View& view);//reallocates the texture

        protected:

            std::list<Light*> _lights;
            sf::RenderStates _multiplyState;
            sf::RenderStates _addState;
            sf::Shader _lightAttenuationShader;
            sf::RenderTexture _renderTexture;
            sf::Sprite _sprite;

            bool _isometric;
            bool _autoDelete;

    };
}
}

#endif // HEADER_DMGDVT_LIGHTSYSTEM

