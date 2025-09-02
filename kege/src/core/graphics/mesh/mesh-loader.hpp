//
//  mesh-loader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 6/30/25.
//

#ifndef mesh_loader_hpp
#define mesh_loader_hpp

#include "../../graphics/mesh/mesh.hpp"
#include "../../utils/asset-system.hpp"

namespace kege{

    class MeshLoader {
    public:

        static bool load( kege::AssetSystem* assets, const std::string& filename );
    };

}

#endif /* mesh_loader_hpp */
