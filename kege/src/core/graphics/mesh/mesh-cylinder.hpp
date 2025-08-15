//
//  mesh-cylinder.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#ifndef mesh_cylinder_hpp
#define mesh_cylinder_hpp

#include "../mesh/mesh.hpp"
namespace kege{

    class CylinderMesh : public kege::Mesh
    {
        public: CylinderMesh( float radius, uint32_t height, uint32_t columns );
    };

}
#endif /* mesh_cylinder_hpp */
