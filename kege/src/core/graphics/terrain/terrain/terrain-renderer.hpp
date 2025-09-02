//
//  terrain-renderer.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef terrain_renderer_hpp
#define terrain_renderer_hpp

#include "quadtree.h"
#include "landscape-layer.hpp"
#include "landscape-settings.h"
#include "image-layer-manager.hpp"

namespace kege{

    struct Stats
    {
        uint32_t total_landscape_node = 0;
        uint32_t total_terrain_node = 0;
        uint32_t total_terrain = 0;
        uint32_t total_memory = 0;
        uint32_t drawcount = 0;
        uint32_t instances = 0;
    };


    class TerrainRenderer : public RefCounter
    {
    public:


        virtual ~TerrainRenderer();

    protected:

    };

}

#endif /* terrain_renderer_hpp */
