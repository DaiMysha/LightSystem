/*
Copyright DaiMysha (c) 2015, All rights reserved.
DaiMysha@gmail.com
https://github.com/DaiMysha

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/



#ifndef HEADER_DMGDVT_LIGHTSYSTEM
#define HEADER_DMGDVT_LIGHTSYSTEM

#include <list>

#include <SFML/Graphics.hpp>

#include <LightSystem/Light.hpp>

namespace DMGDVT {
namespace LS {
    ///this class is not meant to be inherited
    class ShadowSystem;

    class LightSystem {
        public:
            enum DebugFlags {
                DEFAULT = 0,
                SHADER_OFF = 1,
                LIGHTMAP_ONLY = 2,
            };

            LightSystem();
            LightSystem(const LightSystem& ls) = delete; //doesn't make sense to copy an existing light system
            LightSystem(LightSystem&& ls) = delete; //doesn't make sense to copy an existing light system
            LightSystem& operator=(const LightSystem& ls) = delete;
            LightSystem& operator=(LightSystem&& ls) = delete;

            ~LightSystem();

            void addLight(Light* l);
            template <typename T, typename ... Args>
            T* addLight(Args&& ... args) {
                T *l = new T(std::forward<Args>(args)...);
                addLight(l);
                return l;
            }
            void removeLight(Light* l);//User is responsible for memory

            void reset();//empties the lights

            void addWall(const sf::ConvexShape& s);

            //call this function to prepare the render
            void render(const sf::View& screenView, sf::RenderTarget& target);
            void debugRender(const sf::View& screenView, sf::RenderTarget& target, int flags = DebugFlags::DEFAULT);
            //call these to draw the light on the screen
            void draw(const sf::View& screenView, sf::RenderTarget& target);
            void drawAABB(const sf::View& screenView, sf::RenderTarget& target);
            void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);
            void drawWalls(const sf::View& screenView, sf::RenderTarget& target);

            void update();
            void update(Light* l);

            size_t getLightsCount() const;
            size_t getNormalLightsCount() const;
            size_t getNegativeLightsCount() const;
            size_t getEmissiveLightsCount() const;

            sf::Image getLightMap();//this function is expensive so don't call it too often

            sf::Color getLightMapPixel(const sf::View& view, unsigned int x, unsigned int y);
            sf::Color getLightMapPixel(const sf::View& view, sf::Vector2f p);

            void setAmbiantLight(sf::Color c);
            sf::Color getAmbiantLight() const;

            //void setIsometric(bool i);
            //bool isIsometric() const;

            void setAutoDelete(bool ad);

            void setView(const sf::View& view);//reallocates the texture

        protected:
            static const sf::RenderStates _multiplyState;
            static const sf::RenderStates _addState;
            static const sf::RenderStates _subtractState;

            std::list<Light*> _lights;
            std::list<Light*> _negativeLights;
            std::list<Light*> _emissiveLights;
            sf::Color _ambiant;
            sf::Shader _lightAttenuationShader;
            sf::RenderTexture _renderTexture;
            sf::RenderTexture _buffer;
            sf::Sprite _sprite;
            sf::Sprite _bufferSprite;

            bool _isometric;
            bool _autoDelete;

            bool _updateLightMapImage;
            sf::Image _lightMapImage;

            ShadowSystem* _shadowSystem;

    };
}
}

#endif // HEADER_DMGDVT_LIGHTSYSTEM

