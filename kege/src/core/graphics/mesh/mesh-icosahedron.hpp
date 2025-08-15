//
//  mesh-icosahedron.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef mesh_icosahedron_hpp
#define mesh_icosahedron_hpp

#include "mesh.hpp"

namespace kege{

    class IcosahedronMesh : public kege::Mesh
    {
        public: IcosahedronMesh( float radius );
    };

    void generateIcosahedronMesh( float radius, std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices );

}
#endif /* mesh_icosahedron_hpp */
