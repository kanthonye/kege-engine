//
//  mesh-rendering-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#ifndef mesh_rendering_system_hpp
#define mesh_rendering_system_hpp

#include "../../ecs/entity-system.hpp"
#include "../../input/input-commands.hpp"
#include "../../graphics/mesh/mesh.hpp"
#include "../render/core/graphics.hpp"

namespace kege{

    class MeshRenderingSystem : public kege::EntitySystem
    {
    public:

        void render( double dms );
        bool initialize();
        void shutdown();

        MeshRenderingSystem( kege::EntitySystemManager* esm );


        Ref< Material > _default_material;
    };
    
}


#endif /* mesh_rendering_system_hpp */
