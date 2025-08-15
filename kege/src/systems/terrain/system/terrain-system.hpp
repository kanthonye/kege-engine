//
//  terrain.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/4/24.
//

#ifndef kege_system_terrain_hpp
#define kege_system_terrain_hpp

#include "entity-components.hpp"
#include "spherical-cube-terrain.hpp"

namespace kege{

    struct TerrainPushConstBlockData
    {
        kege::mat44 transform;
        float spherical_radius;
        float pddn[3];
    };

    struct TerrainTransformPushConstant
    {
        TerrainTransformPushConstant& operator=( const Transform& transform )
        {
            data.transform = kege::quatToM44( transform.rotation );
            data.transform[ 0 ] *= transform.scale.x;
            data.transform[ 1 ] *= transform.scale.y;
            data.transform[ 2 ] *= transform.scale.z;
            data.transform[ 3 ]  = kege::vec4( transform.position, 1.0 );
            return *this;
        }

        TerrainTransformPushConstant()
        {
            push_constant.info.size = sizeof( data );
            push_constant.info.stages = kege::VERTEX_SHADER;
            push_constant.info.offset = 0;
            push_constant.data = &data;
        }

        PushConstant push_constant;
        TerrainPushConstBlockData data;
    };

    class TerrainSystem : public kege::System
    {
    public:
        void execute( CommandBuffer* command_buffer );
        void update( double dms );
        void prepareGeometries();
        bool initialize();

        std::vector< char > temp_render_buffer;
    };

}

#endif /* kege_system_terrain_hpp */
