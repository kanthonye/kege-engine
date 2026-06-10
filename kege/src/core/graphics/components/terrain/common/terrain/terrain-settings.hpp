//
//  landscape-settings.h
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#ifndef terrain_settings_hpp
#define terrain_settings_hpp

#include "../dependencies.hpp"

namespace kege{

    struct TerrainSettings
    {
        enum{ MAX_LOD_LEVELS };

        double   max_terrain_resolution;
        uint32_t max_image_array_layers;
        uint32_t max_terrain_depth;
        uint32_t min_terrain_depth;
        uint32_t heightmap_diameter;
        uint32_t landscape_diameter;

        uint32_t terrain_diameter;
        uint32_t terrain_tile_diameter;

        uint32_t patch_diameter;
        uint32_t view_radius;
        double max_height;
        double min_height;

        kege::dvec3 position;

        uint16_t resolutions[ MAX_LOD_LEVELS ];

        kege::GraphicsDevice* graphics;
    };
}
#endif /* terrain_settings_hpp */
