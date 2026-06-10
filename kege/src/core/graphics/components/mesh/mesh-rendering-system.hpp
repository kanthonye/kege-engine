//
//  mesh-rendering-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#ifndef mesh_rendering_system_hpp
#define mesh_rendering_system_hpp

#include "../../../scene/ecs-system-factory.hpp"
#include "../../../input/input-commands.hpp"
#include "../../../graphics/components/mesh/mesh.hpp"
#include "../../render/renderer.hpp"

namespace kege{

    class MeshRenderingSystem : public kege::ecs::System
    {
    public:

        void render( double dms );
        MeshRenderingSystem( kege::ECS* esm );
    };
    
}


#endif /* mesh_rendering_system_hpp */
