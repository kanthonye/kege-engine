//
//  landscape-settings.h
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#ifndef landscape_settings_h
#define landscape_settings_h

#include "vectors.hpp"

namespace kege{

    struct LandscapeSettings
    {
        enum{ MAX_LOD_LEVELS };
        double   max_terrain_resolution;
        uint32_t max_image_array_layers;
        uint32_t max_terrain_depth;
        uint32_t min_terrain_depth;
        uint32_t heightmap_diameter;
        uint32_t landscape_diameter;
        uint32_t terrain_diameter;
        uint32_t patch_diameter;
        uint32_t view_radius;
        double max_height;
        double min_height;
        kege::dvec3 position;
        uint16_t resolutions[ MAX_LOD_LEVELS ];
    };

}
#endif /* landscape_settings_h */
