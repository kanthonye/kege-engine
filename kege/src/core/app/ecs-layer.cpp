//
//  ecs-layer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#include "ecs-layer.hpp"


namespace kege{

    ECSLayer::ECSLayer( ref::ECS ecs, ref::AssetManager am, const ref::ProjectManager& pm )
    :   kege::AppLayer( "ECSLayer" )
    //,   _input_context_manager( icm )
    ,   _project_manager( pm )
    ,   _asset_manager( am )
    ,   _ecs( ecs )
    {}

    bool ECSLayer::initialize()
    {
//        _ecs = new kege::ECS;
//        if ( !_ecs->initialize() )
//        {
//            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntityManager" << Log::nl;
//            return false;
//        }
        //Entity::setManager( _entity_manager.ref() );

//        _entity_systems = new kege::EntitySystemManager
//        (
//            _input_context_manager.ref(),
//            _render_graph->getRenderExecutor().ref(),
//            _project_manager.ref()
//        );

        _ecs->addSystem( "camera-controller" );
        _ecs->addSystem( "lookat" );
        _ecs->addSystem( "follow" );
        _ecs->addSystem( "movement-controller" );

        _ecs->addSystem( "compute-camera-ray" );
        _ecs->addSystem( "entity-selecter" );
        _ecs->addSystem( "entity-dragging" );

        _ecs->addSystem( "particle-emitter-updater" );
        _ecs->addSystem( "particle-effect-updater" );

        _ecs->addSystem( "physics" );
        _ecs->addSystem( "rigidbody-to-transform" );

        _ecs->addSystem( "terrain" );
        _ecs->addSystem( "update-scene-camera" );
        _ecs->addSystem( "mesh-rendering" );

        if ( !_ecs->initialize() )
        {
            kege::Log::error << "( INITIALIZATION_FAILED ) -> EntitySystemManager" << Log::nl;
            return false;
        }
        return true;
    }

    void ECSLayer::shutdown()
    {
        //_input_context_manager.clear();
        _project_manager.clear();
        //_render_graph.clear();
        _asset_manager.clear();
        _ecs.clear();
    }

    bool ECSLayer::update()
    {
        if ( _project_manager->empty() ) return false;

        Project* project = _project_manager->getProject();
        if ( project == nullptr ) return false;


        if ( project->scene_manager->getScene() )
        {
            _ecs->update(0);
        }
        return true;
    }

    bool ECSLayer::render()
    {
        if ( !_project_manager->empty() )
        {
            //if ( _project_manager->getSceneManager()->getScene() )
            {
                _ecs->render(0);
            }
        }
        return true;
    }

    ECSLayer::~ECSLayer()
    {
        shutdown();
    }
}
