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
        if( !_entities ) return;

        for (Entity entity : *_entities )
        {
            Terrain* terrain = entity.get< Terrain >();
            terrain->submitVisibleGeometries();
        }
    }
    
    void TerrainSystem::update( double dms )
    {
        if( !_entities ) return;

        vec3 camera_position;
        if ( _engine->scene().getScene()->getCameraEntity() )
        {
            camera_position = _engine->scene().getScene()->getCameraEntity().get< Transform >()->position;
        }
        else
        {
            camera_position = vec3( 0.f );
        }

        for (Entity entity : *_entities )
        {
            Terrain* terrain = entity.get< Terrain >();
            Transform* transform = entity.get< Transform >();

            terrain->setOrientation( (quatd) transform->orientation );
            terrain->setPosition( (vec3d) transform->position );
            terrain->update( (vec3d) camera_position );
        }
    }

    bool TerrainSystem::initialize()
    {
        _signature = createEntitySignature< Terrain, Transform >();
        return EntitySystem::initialize();
    }

    TerrainSystem::~TerrainSystem()
    {}

    TerrainSystem::TerrainSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "terrain-system", REQUIRE_UPDATE | REQUIRE_RENDER )
    {}


    KEGE_REGISTER_ENTITY_SYSTEM( TerrainSystem, "terrain" );
}
