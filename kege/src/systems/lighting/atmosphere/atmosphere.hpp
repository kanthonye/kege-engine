//
//  atmosphere.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef kege_atmosphere_hpp
#define kege_atmosphere_hpp

#include "vectors.hpp"

namespace kege{

    struct Atmosphere
    {
        double inner_radius;
        double outer_radius;

        struct Rayleigh
        {
            vec3 scattering_coefficient;
            float scale_height;
        }
        rayleigh;

        struct Mie
        {
            vec3 scattering_coefficient;
            float scale_height;
            float anisotropy;
        }
        mie;

        struct Absorption
        {
            vec3 scattering_coefficient;
            float ozone_center_height;
        }
        absorption;

        struct Rendering
        {
            vec3 ground_color;
            vec3 sky_color;
            bool enable_multiple_scattering;
            int samples;
        }
        rendering;
    };

}
#endif /* kege_atmosphere_hpp */
