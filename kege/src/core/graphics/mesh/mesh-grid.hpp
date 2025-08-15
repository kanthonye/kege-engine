//
//  mesh-grid.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#ifndef mesh_grid_hpp
#define mesh_grid_hpp

#include "mesh.hpp"

namespace kege{

    class GridMesh : public kege::Mesh 
    {
        public: GridMesh( int width, float height, int cols, int rows );
    };

}
#endif /* mesh_grid_hpp */
