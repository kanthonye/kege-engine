//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "../../../io/virtual-directory.hpp"
#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::render( double dms )
    {
        
    }

    MeshRenderingSystem::MeshRenderingSystem( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "mesh-rendering-system", REQUIRE_RENDER  )
    {
    }


    KEGE_REGISTER_ENTITY_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
