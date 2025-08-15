//
//  mesh-rect.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef mesh_rect_hpp
#define mesh_rect_hpp

#include "mesh.hpp"

namespace kege{

    class RectMesh : public kege::Mesh
    {
        public: RectMesh( const kege::vec3& center, float width, float height );
    };

}
#endif /* mesh_rect_hpp */
