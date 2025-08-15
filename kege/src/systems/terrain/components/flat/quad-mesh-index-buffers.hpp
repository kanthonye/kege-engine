//
//  flat-landscape-patch.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/27/24.
//

#ifndef quad_mesh_index_buffers_hpp
#define quad_mesh_index_buffers_hpp

#include <list>
#include "vmath.hpp"
#include "graphics.h"

namespace kege{

    class QuadMeshIndexBuffers
    {
    public:

        uint16_t getIndexBufferDrawCount(int i);
        ResourceSet& getGeometryBuffer();
        void destroy();
        bool initialize();
        
    private:

        struct IndexArray
        {
            uint32_t indices[ 24 ];
        };

        std::vector< uint16_t   > _index_array_drawcounts;
        std::vector< IndexArray > _index_arrays;

        kege::Buffer _index_uniform_buffer;
        ResourceSet _resource_set;
    };


//    QuadMeshIndexBufferSet

}
#endif /* flat_landscape_patch_hpp */
