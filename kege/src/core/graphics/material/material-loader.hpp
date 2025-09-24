//
//  material-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#ifndef material_loader_hpp
#define material_loader_hpp

#include "../../graphics/mesh/mesh.hpp"
#include "../../utils/asset-manager.hpp"

namespace kege{

    class MaterialLoader{
    public:

        static bool load( kege::AssetManager* assets, const std::string& filename );
    };

}

#endif /* material_loader_hpp */
