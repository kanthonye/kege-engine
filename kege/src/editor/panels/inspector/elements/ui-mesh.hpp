//
//  ui-mesh.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#ifndef ui_mesh_hpp
#define ui_mesh_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui{

    bool mesh( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    
}
#endif /* ui_mesh_hpp */
