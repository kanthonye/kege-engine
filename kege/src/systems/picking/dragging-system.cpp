//
//  dragging-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#include "dragging-system.hpp"

namespace kege{

    void EntityDraggingSystem::operator()( const MappedInputs& inputs )
    {
        if ( inputs[ kege::ACTION_CAST_RAY ] )
        {
            _drag_entity = true;
        }
//        for (int i=0; i<inputs.size(); ++i)
//        {
//            if ( inputs[i].type == kege::Input::MOUSEKEY )
//            {
//                if ( inputs[i].key.code == 0 && inputs[i].key.state != 0 )
//                {
//                    _drag_entity = true;
//                }
//                else if ( inputs[i].key.code == 0 && inputs[i].key.state == 0 )
//                {
//                    _selected_entity = {};
//                    _drag_entity = false;
//                }
//            }
//            else if ( inputs[i].type == kege::Input::KEYBOARD )
//            {
//                kege::Rigidbody* body = _selected_entity.get< kege::Rigidbody >();
//                if ( body )
//                {
//                    float a = 0.2;
//                    switch ( inputs[i].key.code )
//                    {
//                        default:break;
//                        case kege::KEY_X: {
//                            body->angular.velocity += {a,0,0};
//                            break;
//                        }
//                        case kege::KEY_S: {
//                            body->angular.velocity -= {a,0,0};
//                            break;
//                        }
//                        case kege::KEY_Y: {
//                            body->angular.velocity += {0,a,0};
//                            break;
//                        }
//                        case kege::KEY_T: {
//                            body->angular.velocity -= {0,a,0};
//                            break;
//                        }
//                        case kege::KEY_Z: {
//                            body->angular.velocity += {0,0,a};
//                            break;
//                        }
//                        case kege::KEY_A: {
//                            body->angular.velocity -= {0,0,a};
//                            break;
//                        }
//                    }
//                }
//            }
//        }
    }
    
    void EntityDraggingSystem::operator()( const MsgEntitySelection& msg )
    {
        _selected_entity = msg.entity;

        kege::Rigidbody* body = _selected_entity.get< kege::Rigidbody >();
        kege::vec3 origin = _engine->getScene()->getCameraEntity().get< kege::Transform >()->position;

        _distance = kege::magn( body->center - origin );
        _comm.broadcast< const MsgEntitySelectionDistance& >({ _distance });
    }
    
    void EntityDraggingSystem::update( double dms )
    {
        if ( !_drag_entity || !_selected_entity ) return;

        _drag_entity = false;
        kege::vec3 ray = _engine->getScene()->getSceneRay();
        kege::vec3 origin = _engine->getScene()->getCameraEntity().get< kege::Transform >()->position;
        kege::Rigidbody* body = _selected_entity.get< kege::Rigidbody >();

        if ( body == nullptr ) return;
        if ( body->immovable ) return;

        body->prev = body->center;
        body->center = origin + ray * _distance;
        body->linear.velocity = {0.f,0.f,0.f};
//        if ( body->collider )
//        {
//            body->collider->integrate( body );
//        }
    }

    bool EntityDraggingSystem::initialize()
    {
        _comm.add< const MappedInputs&, EntityDraggingSystem>( this );
        _comm.add< const MsgEntitySelection&, EntityDraggingSystem>( this );
        return kege::EntitySystem::initialize();
    }

    void EntityDraggingSystem::shutdown()
    {
        _comm.remove< const MsgEntitySelection&, EntityDraggingSystem>( this );
        _comm.remove< const MappedInputs&, EntityDraggingSystem>( this );
        kege::EntitySystem::shutdown();
    }

    EntityDraggingSystem::EntityDraggingSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "entity-dragging-system", REQUIRE_UPDATE | REQUIRE_INPUT )
    ,   _drag_entity( false )
    ,   _selected_entity({})
    {}

    KEGE_REGISTER_SYSTEM( EntityDraggingSystem, "entity-dragging" );
}
