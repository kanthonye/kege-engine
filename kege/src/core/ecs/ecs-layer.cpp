//
//  ecs-layer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#include "ecs-layer.hpp"


namespace kege{

    ECSLayer::ECSLayer( ref::InputContextManager icm, ref::AssetManager am, ref::RenderGraph rg, const ref::ProjectManager& pm )
    :   kege::AppLayer( "ECSLayer" )
    ,   _input_context_manager( icm )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    ,   _asset_manager( am )
    {}

    bool ECSLayer::initialize()
    {
        _entity_manager = new kege::EntityManager;
        if ( !_entity_manager->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntityManager" << Log::nl;
            return false;
        }
        Entity::setManager( _entity_manager.ref() );


        _entity_systems = new kege::EntitySystemManager
        (
            _input_context_manager.ref(),
            _render_graph->getRenderExecutor().ref(),
            _project_manager.ref()
        );

        _entity_systems->addSystem( "camera-controller" );
        _entity_systems->addSystem( "lookat" );
        _entity_systems->addSystem( "follow" );
        _entity_systems->addSystem( "movement-controller" );

        _entity_systems->addSystem( "compute-camera-ray" );
        _entity_systems->addSystem( "entity-selecter" );
        _entity_systems->addSystem( "entity-dragging" );

        _entity_systems->addSystem( "particle-emitter-updater" );
        _entity_systems->addSystem( "particle-effect-updater" );

        _entity_systems->addSystem( "physics" );
        _entity_systems->addSystem( "rigidbody-to-transform" );

        _entity_systems->addSystem( "terrain" );
        _entity_systems->addSystem( "update-scene-camera" );
        _entity_systems->addSystem( "mesh-rendering" );

        if ( !_entity_systems->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntitySystemManager" << Log::nl;
            return false;
        }
        return true;
    }

    void ECSLayer::shutdown()
    {
        _entity_manager.clear();
        _entity_systems.clear();
        _project_manager.clear();
        _render_graph.clear();
    }

    void ECSLayer::update()
    {
        if ( !_project_manager->empty() )
        {
            _entity_systems->update(0);
        }
    }

    ECSLayer::~ECSLayer()
    {
        shutdown();
    }
}
