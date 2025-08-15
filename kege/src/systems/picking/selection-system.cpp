//
//  selection-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#include "selection-system.hpp"

namespace kege{

    void EntitySelectionSystem::operator()( const MappedInputs& inputs )
    {
        if ( inputs[ kege::ACTION_CAST_RAY ] )
        {
            _make_selection = true;
        }
    }
    void EntitySelectionSystem::update( double dms )
    {
        if ( !_engine->getScene()->getCameraEntity() )
        {
            return;
        }

        if ( !_make_selection )
        {
            return;
        }

        _make_selection = false;
        kege::vec3 ray = _engine->getScene()->getSceneRay();
        kege::vec3 origin = _engine->getScene()->getCameraEntity().get< kege::Transform >()->position;

        std::vector< std::pair< kege::Entity, double > > selections;

        // test ray against entities. store the entities hit by the ray, also store the distance from origin

        for ( kege::Entity& entity : *_entities )
        {
            kege::Rigidbody* body = entity.get< kege::Rigidbody >();
            if( body->collider )
            {
                if( body->collider->shape_type != RIGID_SHAPE_PLANE )
                {
                    kege::algo::RayHit result;
                    if( kege::algo::rayhit({origin, ray}, body->collider.ref(), &result ) )
                    {
                        selections.push_back({ entity, kege::magn( body->center - origin ) });
                    }
                }
            }
        }

        if ( !selections.empty() )
        {
            // sort entity fromt closest to farest from origin.
            std::sort
            (
                selections.begin(),
                selections.end(),
                []( const std::pair< kege::Entity, double >& a, const std::pair< kege::Entity, double >& b )
                {
                    return a.second < b.second;
                }
            );

            // select the closest entity.
            _comm.broadcast< const MsgEntitySelection& >({ selections[0].first });
            //std::cout <<"entity: "<< selections[0].first <<"\n";
        }
    }
    
    bool EntitySelectionSystem::initialize()
    {
        _comm.add< const MappedInputs&, EntitySelectionSystem >( this );
        return kege::EntitySystem::initialize();
    }

    void EntitySelectionSystem::shutdown()
    {
        _comm.remove< const MappedInputs&, EntitySelectionSystem >( this );
        kege::EntitySystem::shutdown();
    }

    EntitySelectionSystem::EntitySelectionSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "entity-selection-system", REQUIRE_UPDATE | REQUIRE_INPUT )
    ,   _make_selection( false )
    {
        _signature = kege::createEntitySignature< kege::Rigidbody, kege::Transform >();
    }

    KEGE_REGISTER_SYSTEM( EntitySelectionSystem, "entity-selecter" );
};




