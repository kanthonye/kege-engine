//
//  physics-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#include "physics-system.hpp"

namespace kege{

    physics::Simulation& PhysicsSystem::getPhysicsSimulation()
    {
        return _simulation;
    }

    void PhysicsSystem::update( double dms )
    {
        _simulation.simulate( dms );
    }

    bool PhysicsSystem::initialize()
    {
        ComponentCacheT< Rigidbody >* rigidbodies = Entity::getManager().getComponentManager< Rigidbody >();

        _simulation.initialize( rigidbodies );
        _signature = kege::createEntitySignature< kege::Rigidbody, kege::Transform >();
        return EntitySystem::initialize();
    }

    void PhysicsSystem::shutdown()
    {
        Setting::singleton().remove( "physics-system-iteration-count" );
        _simulation.shutdown();
        return EntitySystem::shutdown();
    }

    PhysicsSystem::PhysicsSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "physics-system", REQUIRE_UPDATE )
    {}

    KEGE_REGISTER_SYSTEM( PhysicsSystem, "physics" );
}


