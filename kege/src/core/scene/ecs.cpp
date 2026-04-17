//
//  ecs.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#include "ecs.hpp"
#include "../utils/log.hpp"
#include "ecs-system-factory.hpp"


namespace kege{

//    void ECS::destroy( const ecs::Entity& entity )
//    {
//        for (ecs::Entity e = begin(entity); valid(e); e = next(e))
//        {
//            destroy(e);
//        }
//        _entity_manager.erase( entity );
//    }
//
//    void ECS::attach(ecs::Entity& parent, ecs::Entity& child)
//    {
//        ecs::Parent* p = get< ecs::Parent >( parent );
//        if( p == nullptr )
//        {
//            p = add< ecs::Parent >(parent);
//            p->count = 0;
//        }
//
//        ecs::Child* c = get< ecs::Child >( child );
//        if( c == nullptr )
//        {
//            c = add< ecs::Child >( child );
//        }
//
//        c->prev.index = INVALID_INDEX_U32;
//        c->next.index = INVALID_INDEX_U32;
//        c->parent = parent;
//
//        if ( p->count == 0 )
//        {
//            p->tail = p->head = child;
//        }
//        else
//        {
//            get< ecs::Child >( p->tail )->next = child;
//            c->prev = p->tail;
//            p->tail = child;
//        }
//        p->count += 1;
//    }
//
//    void ECS::detach(ecs::Entity& child)
//    {
//        ecs::Child* c = get< ecs::Child >( child );
//        if( c == nullptr )
//        {
//            return;
//        }
//        ecs::Parent* p = get< ecs::Parent >( c->parent );
//        if( p == nullptr )
//        {
//            return;
//        }
//
//        if ( p->head.version == child.version )
//        {
//            if ( p->head.index == child.index )
//            {
//                ecs::Child* c = get< ecs::Child >( p->head );
//                p->head = c->next;
//                if ( valid( p->head ) ) c->prev.index = INVALID_INDEX_U32;
//                else p->tail.index = INVALID_INDEX_U32;
//            }
//            else if ( p->tail.index == child.index )
//            {
//                ecs::Child* c = get< ecs::Child >( p->tail );
//                p->tail = c->prev;
//                if ( valid( p->tail ) ) c->next.index = INVALID_INDEX_U32;
//                else p->head.index = INVALID_INDEX_U32;
//            }
//            else
//            {
//
//                ecs::Child* c = get< ecs::Child >( child );
//                ecs::Child* p = get< ecs::Child >( c->prev );
//                ecs::Child* n = get< ecs::Child >( c->next );
//                p->next = c->next;
//                n->prev = c->prev;
//            }
//            p->count -= 1;
//        }
//    }
//
//    ecs::Entity ECS::getParent( const ecs::Entity& entity )const
//    {
//        const ecs::Child* c = get< ecs::Child >( entity );
//        return (c != nullptr)? c->parent : ecs::Entity{};
//    }
//
//    ecs::Entity ECS::getRoot( const ecs::Entity& entity )const
//    {
//        const ecs::Child* c = get< ecs::Child >( entity );
//        return (c != nullptr)? getRoot( c->parent ) : entity;
//    }
//
//    bool ECS::isParent( const ecs::Entity& entity )const
//    {
//        return get< ecs::Parent >( entity ) != nullptr;
//    }
//
//    bool ECS::isChild( const ecs::Entity& entity )const
//    {
//        return get< ecs::Child >( entity ) != nullptr;
//    }
//
//    ecs::Entity ECS::begin( const ecs::Entity& entity )const
//    {
//        const ecs::Parent* p = get< ecs::Parent >( entity );
//        return (p != nullptr)? p->head : ecs::Entity{};
//    }
//
//    ecs::Entity ECS::end( const ecs::Entity& entity )const
//    {
//        const ecs::Parent* p = get< ecs::Parent >( entity );
//        return (p != nullptr)? p->tail : ecs::Entity{};
//    }
//
//    ecs::Entity ECS::next( const ecs::Entity& entity )const
//    {
//        const ecs::Child* c = get< ecs::Child >( entity );
//        return (c != nullptr)? c->next : ecs::Entity{};
//    }
//
//    ecs::Entity ECS::prev( const ecs::Entity& entity )const
//    {
//        const ecs::Child* c = get< ecs::Child >( entity );
//        return (c != nullptr)? c->prev : ecs::Entity{};
//    }

    void ECS::addSystem( kege::Ref< kege::ecs::System > system )
    {
        _systems.push_back( system );

        if ( system->checkFlag( kege::ecs::System::REQUIRE_UPDATE ) )
        {
            _system_updates.push_back( system.ref() );
        }

        if ( system->checkFlag( kege::ecs::System::REQUIRE_RENDER ) )
        {
            _system_renders.push_back( system.ref() );
        }

        if ( system->checkFlag( kege::ecs::System::REQUIRE_INPUT ) )
        {
            _system_inputs.push_back( system.ref() );
        }
    }

    void ECS::addSystem( const std::string& name )
    {
        kege::Ref< kege::ecs::System > system = ecs::EntitySystemFactory::instance().create( name, this );
        if ( system )
        {
            addSystem( system );
        }
        else
        {
            kege::Log::warning << "system create function for -> " << name <<" dont exist." <<kege::Log::nl;
        }
    }

    const kege::ecs::EntityManager* ECS::getEntityManager()const
    {
        return &_entity_manager;
    }

    kege::ecs::EntityManager* ECS::getEntityManager()
    {
        return &_entity_manager;
    }

    void ECS::update( double dms )
    {
        for ( kege::ecs::System* system : _system_updates )
        {
            system->update( dms );
        }
    }

    void ECS::render( double dms )
    {
        for ( kege::ecs::System* system : _system_renders )
        {
            system->render( dms );
        }
    }

    void ECS::input( double dms )
    {
        for ( kege::ecs::System* system : _system_inputs )
        {
            system->input( dms );
        }
    }

    bool ECS::initialize()
    {
        kege::Log::info << "[ EntitySystemManager ]: initializing..." <<"\n";
        for (kege::Ref< kege::ecs::System >& system : _systems )
        {
            kege::Log::info << "- initializing -> " << system->getName() <<"... ";
            if ( !system->initialize() )
            {
                kege::Log::info << "FAILED... ";
                return false;
            }
            kege::Log::info <<"complete" <<"\n";
        }
        kege::Log::info << "[ EntitySystemManager ]: initialization complete."<<Log::nl;
        return true;
    }

    void ECS::shutdown()
    {
        kege::Log::info << "[ EntitySystemManager ]: shuting-down" <<"\n";
        std::vector< kege::Ref< kege::ecs::System > >::reverse_iterator syst;
        for ( syst = _systems.rbegin(); syst != _systems.rend(); syst++ )
        {
            Log::info << "- shuting-down -> " << (*syst)->getName() <<".\n";
            (*syst)->shutdown();
        }
        _systems.clear();
        _system_updates.clear();
        _system_renders.clear();
        _system_inputs.clear();
        kege::Log::info << "[ EntitySystemManager ]: shutdown complete."<<Log::nl;
    }

    void ECS::setScene( kege::Scene* scene )
    {
        _scene = scene;
        for (kege::Ref< kege::ecs::System >& system : _systems )
        {
            system->onSceneChanged();
        }
    }

    kege::Scene* ECS::getScene()
    {
        return _scene;
    }

//    kege::RenderExecutor* ECS::getRenderExecutor()
//    {
//        return _render_executor;
//    }
//
//    kege::AssetManager* ECS::getAssetManager()
//    {
//        return _project_manager->getAssetManager().ref();
//    }
//
//    kege::MappedInputs& ECS::getMappedInputs()
//    {
//        return _input_context_manager->getMappedInputs();
//    }

    ECS:: ~ECS()
    {
        shutdown();
    }

    ECS::ECS()
    {}

}

