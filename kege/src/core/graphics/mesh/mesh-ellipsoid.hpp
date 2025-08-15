//
//  mesh-ellipsoid.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#ifndef mesh_ellipsoid_hpp
#define mesh_ellipsoid_hpp

#include "mesh.hpp"

namespace kege{

    class EllipsoidMesh : public kege::Mesh
    {
        public: EllipsoidMesh( float x_radius, float y_radius, int cols, int rows );
    };

}
#endif /* mesh_ellipsoid_hpp */
