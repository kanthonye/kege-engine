//
//  mesh-capsule.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/16/25.
//

#ifndef mesh_capsule_hpp
#define mesh_capsule_hpp

#include "../mesh/mesh.hpp"

namespace kege{

    class CapsuleMesh : public kege::Mesh
    {
        public: CapsuleMesh( float height, float radius, int columns, int rings );
    };

}
#endif /* mesh_capsule_hpp */
