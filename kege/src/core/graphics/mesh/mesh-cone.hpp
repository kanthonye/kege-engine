//
//  mesh-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/16/25.
//

#ifndef mesh_cone_hpp
#define mesh_cone_hpp

#include "../mesh/mesh.hpp"

namespace kege{

    class ConeMesh : public kege::Mesh
    {
        public: ConeMesh( float radius, float height, uint32_t columns );
    };

}
#endif /* mesh_cone_hpp */
