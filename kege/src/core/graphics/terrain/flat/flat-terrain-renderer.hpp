//
//  flat-terrain-renderer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/12/25.
//

#ifndef flat_terrain_renderer_hpp
#define flat_terrain_renderer_hpp

#include "../../../io/virtual-directory.hpp"
#include "../../mesh/mesh.hpp"
#include "flat-terrain-node.hpp"
#include "../terrain/terrain-renderer.hpp"
#include "../terrain/image-layer-manager.hpp"

namespace kege{

    class FlatTerrain;

    class FlatTerrainRenderer : public TerrainRenderer
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
            ref::ShaderSet resource_set;

            /**
             * a collection of DrawParams for each patch.
             */
            ref::Buffer draw_buffer;
            ref::Buffer storage_buffer;
            
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
            VertexDrawCommand* params;

            /**
             * The patch
             */
            FlatTerrainPatch* patchs;
        };

        enum{ MAX_INSTANCE_BUFFER_SIZE = 8196, MAX_INSTANCE_BUFFER_COUNT = 10 };

    public:

        void begin( kege::CommandEncoder* encoder, Transform* transform );
        void end();

        kege::ImageLayerManager& getImageLayerManager();
        void submit( const FlatTerrainPatch& node );
        uint32_t flush( int image_index );

        bool initialize( const kege::TerrainSettings* settings );
        void shutdown();

        ~FlatTerrainRenderer();
        FlatTerrainRenderer( kege::Graphics* graphics );

    private:

        bool initializeMesh();
        
        kege::ImageLayerManager _image_layer_manager;
        const kege::TerrainSettings* _settings;
        kege::CommandEncoder* _encoder;
        kege::Graphics* _graphics;


        std::vector< IndexDrawCommand > _index_draw_commands;
        ref::Buffer _vertex_buffer;
        ref::Buffer _index_buffer;

        /*
         The shader used to render the landscape.
         */
        kege::PipelineHandle _shader_pipeline;

        /*
         The instance-buffers used for rendering multiple instances of the same geometry
         in one draw call. Each InstanceBuffer results in a draw call.
         */
        std::vector< Batch > _batchs;

        uint32_t _total_instances;
        uint32_t _batch_count;
        uint32_t _curr_batch;
    };

}
#endif /* flat_terrain_renderer_hpp */
