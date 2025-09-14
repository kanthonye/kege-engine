//
//  spherical-terrain-renderer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef spherical_terrain_renderer_hpp
#define spherical_terrain_renderer_hpp

#include "../../terrain/terrain-renderer.hpp"
#include "../../terrain/physical-terrain.hpp"
#include "dynamic-cube-mesh.hpp"
#include "spherical-terrain-tile.hpp"

namespace kege{

    struct PatchDrawBuffer
    {
        kege::ShaderResource descriptor_set;
        kege::BufferHandle patch_buffer;
        kege::BufferHandle draw_buffer;
        uint32_t instance_count;
    };

    class SphericalTerrainRenderer : public TerrainRenderer
    {
    public:

        bool initialize();
        const DynamicCubeMesh* getDynamicCubeMesh()const;

        void begin( kege::CommandEncoder* encoder, Transform* transform );
        void end();

        void draw( SphericalTerrainTile& node );


        ~SphericalTerrainRenderer();
        SphericalTerrainRenderer();

    private:

        void flush();

        /**
         * This ShaderResourceLayout will store the mesh vertices and indices
         */
        kege::ShaderResource _mesh_shader_resource;

        /**
         * This uniform buffer will store the mesh vertices
         */
        kege::BufferHandle _vertices_uniform_buffer;

        /**
         * This uniform buffer will store the mesh indices
         */
        kege::BufferHandle _indices_uniform_buffer;



        DynamicCubeMesh _cubemesh;
        kege::Graphics* _graphics;
        kege::Terrain* _terrain;


        std::vector< char > _temp_buffer;

        kege::VertexDrawCommand* _draw_param_buffer;
        kege::PatchData* _patch_buffer;
        uint32_t _max_instance_count;
        uint32_t _instance_count;


        std::vector< PatchDrawBuffer > _draw_buffers;


        uint32_t _drawparam_stride;
        uint32_t _patchdata_stride;
        uint32_t _current_drawbatch;

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
