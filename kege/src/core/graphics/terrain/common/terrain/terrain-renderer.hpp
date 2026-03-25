//
//  terrain-renderer.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef terrain_renderer_hpp
#define terrain_renderer_hpp

#include "terrain-settings.hpp"

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


    class TerrainRenderer : public kege::Mesh
    {
    public:

        virtual void submit( int face_id, int index_buffer_id, fvec4& patch ) = 0;
        virtual void begin() = 0;
        virtual void end() = 0;
        virtual ~TerrainRenderer(){}
    };

}

#endif /* terrain_renderer_hpp */
