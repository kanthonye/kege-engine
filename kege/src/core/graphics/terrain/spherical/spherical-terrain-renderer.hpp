//
//  spherical-terrain-renderer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef spherical_terrain_renderer_hpp
#define spherical_terrain_renderer_hpp

#include "../common/all.hpp"
#include "dynamic-cube-mesh.hpp"
#include "spherical-terrain-tile.hpp"

namespace kege{

    struct PatchDrawBuffer
    {
        ref::ShaderSet descriptor_set;
        ref::Buffer patch_buffer;
        ref::Buffer draw_buffer;
        uint32_t instance_count;
    };

    class SphericalTerrainRenderer : public TerrainRenderer
    {
    public:

        const kege::mat33& getFaceAxies( int face_index )const;
        const kege::vec4* getFaceVertices( int face_index )const;

        bool initialize();

        void submit( int face_id, int index_buffer_id, fvec4& patch );
        void begin();
        void end();


        ~SphericalTerrainRenderer();
        SphericalTerrainRenderer( Graphics* graphics );


        struct Batch
        {
            kege::IndexDrawCommand* draw_commands;
            kege::vec4* patchs;
            uint32_t capacity;
            int32_t index;
        };

    private:

        void generateCubeVertces
        (
            float scale,
            std::array< kege::mat33, MAX_CUBE_FACES >& face_axies,
            std::vector< kege::vec4 >& face_vertices
        );

        void generateCubeIndices
        (
            std::vector< int >& face_indices,
            std::array< IndexDrawCommand, 16 >& draw_commands
        );

        Batch mapBatch( int index );
        Batch newBatch();
        void endBatch();
        void flush();

    private:

        kege::ref::Mesh _mesh;

        std::array< IndexDrawCommand, 16 > _draw_commands;
        std::array< kege::mat33, MAX_CUBE_FACES > _face_axies;
        std::vector< kege::vec4 > _face_vertices;

        Batch _current_batch;
        uint32_t _patch_count;
        uint32_t _batch_count;

        kege::Graphics* _graphics;
        kege::Terrain* _terrain;


        uint32_t _draw_command_stride;
        uint32_t _patch_stride;

        uint32_t _max_draw_count;

        /**
         * Exponent value is used to compute buffer size
         */
        uint32_t _exponent;

        /**
         * Base value is used to compute buffer size
         */
        uint32_t _base;

        /**
         * The node_count is used to track the count off all nodes in this quadtree.
         * It is just used for informational purposes.
         */
        uint32_t _total_draws;
    };

}
#endif /* spherical_terrain_renderer_hpp */
