//
//  terrain.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/4/24.
//

#include "terrain-system.hpp"

namespace kege{

    void TerrainSystem::render( double dms )
    {
//        if( !_entities ) return;
//
//        RenderObject object = {};
//        for (Entity entity : *_entities )
//        {
//            Terrain* terrain = entity.get< Terrain >();
//            Transform* transform = entity.get< Transform >();
//
//            terrain->submitVisibleGeometries();
//
//            kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
//            m[1] = kege::quatToM44( transform->orientation );
//            m[0][0] = kege::vec4( m[1][0].xyz * transform->scale.x, 0.0 );
//            m[0][1] = kege::vec4( m[1][1].xyz * transform->scale.y, 0.0 );
//            m[0][2] = kege::vec4( m[1][2].xyz * transform->scale.z, 0.0 );
//            m[0][3] = kege::vec4( transform->position, 1.0 );
//
//            m[1][3].x = terrain->getPhysicalTerrain()->getRadius();
//
//            object.constant.size = 2 * sizeof( kege::mat44 );
//            object.constant.stages = ShaderStageFlag::Vertex;

//            for ( Ref< Mesh >& source : terrain->getTerrainRenderer()->sources )
//            {
//                object.mesh = source;
//                object.material = terrain->getTerrainMaterial();
//
//                getRenderExecutor()->submit( object );
//            }
//        }
    }
    
    void TerrainSystem::update( double dms )
    {
//        if( !_entities ) return;
//
//        vec3 camera_position;
//        if ( getScene()->getCameraEntity() )
//        {
//            camera_position = getScene()->getCameraEntity().get< Transform >()->position;
//        }
//        else
//        {
//            camera_position = vec3( 0.f );
//        }
//
//        for (Entity entity : *_entities )
//        {
//            Terrain* terrain = entity.get< Terrain >();
//            Transform* transform = entity.get< Transform >();
//
//            terrain->setOrientation( transform->orientation );
//            terrain->setPosition( transform->position );
//            terrain->update( camera_position );
//        }
    }

    bool TerrainSystem::initialize()
    {
        //_signature = createEntitySignature< Terrain, Transform >();
        return ecs::System::initialize();
    }

    TerrainSystem::~TerrainSystem()
    {}

    TerrainSystem::TerrainSystem( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "terrain-system", REQUIRE_UPDATE | REQUIRE_RENDER  )
    {}


    KEGE_REGISTER_ENTITY_SYSTEM( TerrainSystem, "terrain" );
}
