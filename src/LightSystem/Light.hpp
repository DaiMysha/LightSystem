#ifndef HEADER_DMGDVT_LIGHT
#define HEADER_DMGDVT_LIGHT

namespace DMGDVT {
namespace LS {

    //this is temporary
    struct AABB {
        float x,y,w,h;

        bool collides(const AABB& other) {
            return !(other.x > x+w || other.y > y+h || other.x+other.w < x || other.y+other.h < y);
        }
    };

    class Light {
        public:
            //empty body here to avoid compilation error
            //will move to .cpp as soon as functions appear here
            Light() {
                _aabb.x=_aabb.y=_aabb.w=_aabb.h=0.0f;
            }
            virtual ~Light() {}

            virtual void render(AABB screen, sf::RenderWindow& window) = 0;
            virtual void drawAABB(AABB screen, sf::RenderWindow& window) = 0;

            virtual void computeAABB() = 0;

            //returns the AABB according to the whole game
            virtual AABB getAABB() = 0;

        protected:
            AABB _aabb;

    };
}
}

#endif // HEADER_DMGDVT_LIGHT

