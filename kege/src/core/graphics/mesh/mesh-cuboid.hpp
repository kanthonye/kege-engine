//
//  cuboid.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 2/18/24.
//

#ifndef ke_graphics_cuboid_hpp
#define ke_graphics_cuboid_hpp

#include "mesh.hpp"

namespace kege{

    class CuboidMesh : public kege::Mesh
    {
        public: CuboidMesh( const kege::vec3& center, const kege::vec3& lengths );
    };

}
#endif /* ke_graphics_cuboid_hpp */
