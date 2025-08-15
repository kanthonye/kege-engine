//
//  spherical-cube-terrain.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/6/24.
//

#ifndef cube_sphere_quadtree_terrain_hpp
#define cube_sphere_quadtree_terrain_hpp

#include "terrain.hpp"
#include "cube-mesh-shader-resource.hpp"

namespace kege{

    class SphericalCubeTerrain : public kege::Terrain
    {
    public:

        enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
        enum { NORTH, EAST, SOUTH, WEST };

        void init( QuadtreePatchNode& node, const kege::vec3& position, float scale, double radius, int depth, int face_id  );

        /**
         * @fn prepare
         *
         * Update the geometric render data of the surface.
         *
         * @note This function must be called before the draw function
         * @param temp_render_buffer The temp data to use while preparing the geometries.
         */
        void prepareGeometries( std::vector< char >& temp_render_buffer );
        
        /**
         * @fn prepare
         *
         * Prepare the geometries for rendering
         *
         * @param node The node to process the geometric data for
         */
        void prepareGeometries( QuadtreePatchNode& node );

        /**
         * @fn submit
         *
         * Upload the geometric data to the GPU
         */
        void submitDrawParamsAndPatchBuffer();


        void draw( kege::CommandBuffer* command_buffer )const;
        void bind( kege::CommandBuffer* command_buffer )const;

        void setCameraPosition( const kege::vec3& position );
        void setRotation( const kege::quat& rotation );
        void setPosition( const kege::vec3& position );


        float getHeight( const kege::vec3& point );

        /**
         * @fn canSubDivide
         *
         * Test whether a node can be sub-divided. This function uses distance and size to determine the result.
         *
         * @param node The node to test
         * @return true if the node is too large or false if it is not.
         *
         */
        bool canSubDivide( QuadtreePatchNode& node );

        /**
         * @fn splitable
         *
         * Test whether a node can be split or not. This function uses the depth and children to determine the result.
         *
         * @param node The node to test
         * @return true if the node can be split or false if it can not.
         */
        bool splitable( QuadtreePatchNode& node );

        /**
         * @fn split
         *
         * Splits the quadtree node into smaller quadtree node.
         *
         * @param node The quadtree node to be split.
         */
        void split(  int face_id, QuadtreePatchNode& node );

        /**
         * @fn merge
         *
         * Merge the quadtree node child nodes into the parent quadtree node.
         *
         * @param node The quadtree node to be merge.
         */
        void merge( QuadtreePatchNode& node );

        /**
         * @fn update
         *
         * Update the geometric surface
         *
         * @param node The current node to update
         */
        void update( int16_t face_id, QuadtreePatchNode& node, QuadtreePatchNode* neighbors[4] );

        /**
         * @fn update
         *
         * Update the quadtree surface
         */
        void update();

        double radius()const;

        /**
         * @fn init
         *
         * Initializes this object
         */
        void initialize();

        /**
         * @fn SphericalCubeTerrain
         *
         * The constructor
         */
        SphericalCubeTerrain
        (
            double radius,
            double resolution,
            uint32_t min_depth,
            uint32_t max_depth,
            double min_height,
            double max_height
        );

    private:

        /**
         */
        kege::PatchData* _patch_buffer;
        kege::DrawParams* _draw_param_buffer;
        uint32_t _max_instance_count;
        uint32_t _instance_count;


        const CubeMeshShaderResource* _cubemesh;

        /**
         * The 6 quadtree faces that make up the cube
         */
        QuadtreePatch _cube_faces[ MAX_CUBE_FACES ];

        /**
         * This objects orientation
         */
        kege::dquat _rotation;

        kege::dvec3  _center_to_camera;
        
        /**
         * The camera position / point of interest
         */
        kege::dvec3  _camera_position;

        /**
         * This object's position
         */
        kege::dvec3  _position;

        std::vector< PatchDrawBuffer > _draw_buffers;


        /**
         * This radius of the planet
         */
        double _planet_radius;

        /**
         * The maximum_resolution is used to control the surface level of detain.
         * If a quadtree surface is larger than the maximum resolution base on its
         * distance from the camera it will be divided into smaller surfaces.
         */
        double _maximum_resolution;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double _minimum_depth;

        /**
         * The maximum_depth is also used to control the surface level of detain.
         * Where as the maximum_resolution regulate the level of detial base on the
         * distance from the camera, the maximum_depth control the level of detail
         * by limit the depth a quadtree node can reach.
         */
        double _maximum_depth;

        /**
         * The minimum_height is used set the minimum height of the height map.
         */
        double _minimum_height;

        /**
         * The maximum_height is used set the maximum height of the height map.
         */
        double _maximum_height;


        const uint32_t _drawparam_stride;
        const uint32_t _patchdata_stride;
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
        uint32_t _total_nodes;
        int16_t _total_levels;
        uint32_t _total_leaves;
        uint32_t _total_draws;
    };

}
#endif /* cube_sphere_quadtree_terrain_hpp */
