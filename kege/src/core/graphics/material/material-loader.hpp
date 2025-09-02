//
//  material-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#ifndef material_loader_hpp
#define material_loader_hpp

#include "../../graphics/mesh/mesh.hpp"
#include "../../utils/asset-system.hpp"

namespace kege{

    class MaterialLoader{
    public:

        static bool load( kege::AssetSystem* assets, const std::string& filename );
    };

}

#endif /* material_loader_hpp */
