//
//  light.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/8/24.
//

#ifndef light_hpp
#define light_hpp

#include "../../memory/ref.hpp"
#include "../../math/algebra/vmath.hpp"

namespace kege{

    struct DirectionalLight
    {
        kege::vec3 color;
        float intensity;

        kege::vec3 direction;
    };

    struct PointLight
    {
        // Common light properties
        kege::vec3 color;
        float intensity;

        // Point light specific properties
        kege::vec3 position;
        float point_range;

        float linear_attenuation;
        float quadratic_attenuation;
    };

    struct SpotLight
    {
        // Common light properties
        kege::vec3 color;
        float intensity;

        // Point light specific properties
        kege::vec3 position;
        float point_range;

        // Directional and Spot light specific properties
        kege::vec3 direction;
        int type;

        // Spot light specific properties
        float spot_cutoff;
        float spot_exponent;

        float linear_attenuation;
        float quadratic_attenuation;
    };

    struct Light : public kege::RefCounter
    {
        enum : uint32_t
        {
            SPOT,
            POINT,
            DIRECTIONAL
        };

        Light( const DirectionalLight& light );
        Light( const PointLight& light );
        Light( const SpotLight& light );
        Light(){}

        // Common light properties
        kege::vec3 color;
        float intensity;

        // Point light specific properties
        kege::vec3 position;
        float point_range;

        // Directional and Spot light specific properties
        kege::vec3 direction;
        int type;

        // Spot light specific properties
        float spot_cutoff;
        float spot_exponent;

        float linear_attenuation;
        float quadratic_attenuation;
    };


}
#endif /* light_hpp */
