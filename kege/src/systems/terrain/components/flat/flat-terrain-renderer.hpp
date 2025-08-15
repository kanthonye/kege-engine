//
//  flat-terrain-renderer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/12/25.
//

#ifndef flat_terrain_renderer_hpp
#define flat_terrain_renderer_hpp

#include "landscape.hpp"
#include "quad-mesh-index-buffers.hpp"

namespace kege{

    class FlatTerrain;

    struct FlatTerrainPatch
    {
        struct{ int x, y, radius, index_buffer_id; };
        struct{ int image_index, image_layer, tile_position[2]; };
        kege::vec4 color;
    };

    class FlatTerrainRenderer
    {
    private:

        /**
         * The Batch is a structure used for storing per-instance data, where
         * multiple instances of the same geometry (e.g., meshes) are rendered efficiently
         * by minimizing redundant draw calls. Each InstanceBuffer is one draw call.
         */
        struct Batch
        {
            /**
             * a collection of Patch to render
             */
            kege::ResourceSet resource_set;

            /**
             * a collection of DrawParams for each patch.
             */
            kege::Buffer draw_buffer;

            /**
             * The total number of patch to draw.
             */
            uint32_t instance_count;

            /**
             * The texture array image index
             */
            uint32_t image_index;

            /**
             * The draw parameters for each patch
             */
            MapBuffer< DrawParams > params;

            /**
             * The patch
             */
            MapBuffer< FlatTerrainPatch > patchs;
        };

    private:

        enum{ MAX_INSTANCE_BUFFER_SIZE = 8196, MAX_INSTANCE_BUFFER_COUNT = 10 };
        uint32_t newBatch( uint32_t image_index );

    public:

        void beginRender( kege::CommandBuffer& command_buffer );
        void endRender( kege::CommandBuffer& command_buffer );
        void submit( const FlatTerrainPatch& patch );

        bool initialize();
        void shutdown();

        FlatTerrainRenderer( FlatTerrain* terrain );
        ~FlatTerrainRenderer();

    private:

        /*
         the index buffers to load into shader
         */
        QuadMeshIndexBuffers _quad_mesh_index_buffers;

        /*
         The shader used to render the landscape.
         */
        kege::ShaderPipeline _shader_pipeline;

        /*
         The instance-buffers used for rendering multiple instances of the same geometry
         in one draw call. Each InstanceBuffer results in a draw call.
         */
        std::vector< Batch > _batchs;

        FlatTerrain* _terrain;

        uint32_t _total_instances;
        uint32_t _batch_count;
        uint32_t _curr_batch;
    };

}
#endif /* flat_terrain_renderer_hpp */
