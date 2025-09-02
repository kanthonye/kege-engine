//
//  dynamic-quad-mesh.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef dynamic_quad_mesh_hpp
#define dynamic_quad_mesh_hpp

#include <list>
#include "../../../math/algebra/vmath.hpp"
#include "../../../renderer/core/graphics.hpp"

namespace kege{

    class DynamicQuadMesh
    {
    public:

        DynamicQuadMesh();

        struct IndexArray
        {
            uint32_t indices[ 24 ];
        };

        std::vector< uint16_t   > _index_array_drawcounts;
        std::vector< IndexArray > _index_arrays;
    };

}
#endif /* dynamic_quad_mesh_hpp */
