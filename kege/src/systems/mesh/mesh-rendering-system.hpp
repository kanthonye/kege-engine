//
//  mesh-rendering-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#ifndef mesh_rendering_system_hpp
#define mesh_rendering_system_hpp

#include "../../core/system/entity-system.hpp"
#include "../../core/input/input-commands.hpp"
#include "../../core/graphics/mesh/mesh.hpp"
#include "../../core/graphics/core/graphics.hpp"

namespace kege{


    struct MeshObject
    {
        Resource resource;
    };


    class MeshRenderingSystem : public kege::EntitySystem
    {
    public:

//        typedef std::vector< ModelMatrices > ModelMatrixArray;
//        typedef std::unordered_map< MeshID, ModelMatrixArray > SubmeshGroup;
//        typedef std::unordered_map< EffectID, SubmeshGroup > MaterialSubmeshGroup;
//        typedef std::unordered_map< MeshID, MaterialSubmeshGroup > MeshMaterialGroup;
//        typedef std::unordered_map< ShaderPipelineID, MeshMaterialGroup > ShaderMeshGroup;

        void operator()( kege::RenderPassContext* context );
        bool initialize();
        void shutdown();

        MeshRenderingSystem( kege::Engine* engine );
    };
    
}


#endif /* mesh_rendering_system_hpp */
