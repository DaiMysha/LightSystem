#ifndef HEADER_DMGDVT_SPOTLIGHT
#define HEADER_DMGDVT_SPOTLIGHT

#include <SFML/Graphics.hpp>

#include "Light.hpp"

namespace DMGDVT {
namespace LS {

    /**
    * Would have preferred to do that with something like
    * template <typename float SpreadAngle>
    * to have simply PointLight define as typedef SpotLight<M_PIf*2.0f> PointLight
    * but ah well :<
    **/
    class SpotLight : public Light {
        public:
            //Don't create spotlights with spread angles > pi, it doesn't work
            SpotLight(bool iso=false);
            SpotLight(sf::Vector2f ctr, float r, sf::Color c,bool iso=false);
            SpotLight(sf::Vector2f ctr, float r, sf::Color c, float da, float sa, float i, float s, float b, float lf,bool iso=false);
            virtual ~SpotLight();

            virtual void render(const sf::IntRect& screen, sf::RenderTarget& target, sf::Shader* shader, const sf::RenderStates &states=sf::RenderStates::Default);
            virtual void preRender(sf::Shader* shader);
            virtual void debugRender(const sf::IntRect& screen, sf::RenderTarget& target);
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderTarget& target);

            virtual void computeAABB();

            /*** GETTER - SETTER ***/
            virtual sf::IntRect getAABB();

            void setCenter(sf::Vector2f c);
            sf::Vector2f getCenter() const;

            void setRadius(float r);
            float getRadius() const;

            void setColor(sf::Color c);
            sf::Color getColor() const;

            void setDirectionAngle(float da);
            float getDirectionAngle() const;

            void setSpreadAngle(float sa);
            float getSpreadAngle() const;

            void setIntensity(float i);
            float getIntensity() const;

            void setBleed(float b);
            float getBleed() const;

            void setLinearity(float lf);
            float getLinearity() const;

        protected:
            sf::Vector2f _center;
            float _radius;
            sf::Color _color;
            float _directionAngle; //angle light is pointing, rad
            float _spreadAngle; //how wide the light covers, rad
            float _intensity; //how bright light is
            float _bleed; //radius of the light halo
            float _linearity;


            sf::RenderTexture* _renderTexture;

    };
}
}

#endif // HEADER_DMGDVT_SPOTLIGHT

