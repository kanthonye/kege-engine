//
//  mesh-circle.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#ifndef mesh_circle_hpp
#define mesh_circle_hpp

#include "mesh.hpp"

namespace kege{

    class CircleMesh : public kege::Mesh
    {
        public: CircleMesh( const vec3& center, float radius, int divs );
    };

}
#endif /* mesh_circle_hpp */
