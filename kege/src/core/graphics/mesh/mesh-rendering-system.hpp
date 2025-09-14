//
//  mesh-rendering-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#ifndef mesh_rendering_system_hpp
#define mesh_rendering_system_hpp

#include "../../esm/entity-system.hpp"
#include "../../input/input-commands.hpp"
#include "../../graphics/mesh/mesh.hpp"
#include "../../renderer/core/graphics.hpp"

namespace kege{

    class MeshRenderingSystem : public kege::EntitySystem
    {
    public:

        void render( double dms );
        bool initialize();
        void shutdown();

        MeshRenderingSystem( kege::Engine* engine );
    };
    
}


#endif /* mesh_rendering_system_hpp */
