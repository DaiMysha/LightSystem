#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

namespace DMGDVT {
namespace LS {

    class Light {
        public:
            //empty body here to avoid compilation error
            //will move to .cpp as soon as functions appear here
            Light() : _aabb() {
            }
            virtual ~Light() {}

            virtual void render(const sf::IntRect& screen, sf::RenderWindow& window) = 0;
            virtual void drawAABB(const sf::IntRect& screen, sf::RenderWindow& window) = 0;

            virtual void computeAABB() = 0;

            //returns the AABB according to the whole game
            virtual sf::IntRect getAABB() = 0;

        protected:
            sf::IntRect _aabb;

    };
}
}

#endif // HEADER_DMGDVT_LIGHT

