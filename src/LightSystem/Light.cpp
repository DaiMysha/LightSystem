

#include <LightSystem/Light.hpp>

namespace DMGDVT {
namespace LS {

    const char Light::LAS_PARAM_CENTER[] = "center";
    const char Light::LAS_PARAM_RADIUS[] = "radius";
    const char Light::LAS_PARAM_COLOR[] = "color";
    const char Light::LAS_PARAM_BLEED[] = "bleed";
    const char Light::LAS_PARAM_LINEARITY[] = "linearFactor";
    const char Light::LAS_PARAM_OUTLINE[] = "outline";//used mainly for debug, don't mind it
    const char Light::LAS_PARAM_ISOMETRIC[] = "iso";

    Light::Light(bool iso) : _aabb(), _renderTexture(nullptr), _isometric(iso) {
    }

    Light::~Light() {
        delete _renderTexture;
    }

    void Light::debugRender(sf::RenderTarget& target, const sf::RenderStates &states) {
    }

    bool Light::isIsometric() const {
        return _isometric;
    }

    void Light::setIsometric(bool i) {
        _isometric = i;
    }


}
}
