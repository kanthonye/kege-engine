//
//  simulation.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "../simulators/force-applier.hpp"
#include "../simulators/force-integrator.hpp"
#include "../simulators/collision-detector.hpp"
#include "../simulators/net-force-zeroer.hpp"
#include "../simulators/motion-dampener.hpp"
#include "../simulators/grounded-detector.hpp"
#include "../simulators/contact-impulse-solver.hpp"
#include "../simulators/collision-position-solver.hpp"

#include "physics-simulation.hpp"

namespace kege::physics{

//    Rigidbody* Key::operator ->()
//    {
//        return _simulator->getRigidbody( *this );
//    }

    void Simulation::addSimulator( Stage stage, Simulator* simulator )
    {
        _simulators[ stage ].push_back( simulator );
        simulator->_simulator = this;
    }

    ComponentCacheT< Rigidbody >& Simulation::rigidbodies()
    {
        return *_rigidbodies;
    }

//    Rigidbody* Simulation::getRigidbody( Key id )
//    {
//        return &_rigidbodies[ id._index ];
//    }
//
//    void Simulation::deleteRigidbody( Key id )
//    {}
//
//    Key Simulation::createRigidbody()
//    {
//        return {};
//    }

    kege::CollisionRegistry& Simulation::getCollisionRegistry()
    {
        return _collisions;
    }

    void Simulation::update( Stage stage, double dms )
    {
        for ( Ref< Simulator >& simulator : _simulators[ stage ] )
        {
            simulator->simulate( dms );
        }
    }

    void Simulation::simulate( double dms )
    {
        update( PRE_UPDATE, dms );
        const double time_step = dms / double( _iterations );
        for (int i=0; i < _iterations; ++i )
        {
            update( ON_UPDATE, time_step );
        }
        update( POST_UPDATE, dms );
    }

    bool Simulation::initialize( ComponentCacheT< Rigidbody >* rigidbodies )
    {
        if ( _rigidbodies != nullptr )
        {
            return false;
        }
        
        _rigidbodies = rigidbodies;
        _collisions.resize( 500 );

        addSimulator( PRE_UPDATE,  new ForceApplier() );
        addSimulator( ON_UPDATE,   new ForceIntegrator() );
        addSimulator( ON_UPDATE,   new CollisionDetector() );
        addSimulator( ON_UPDATE,   new ContactImpulseSolver() );
        addSimulator( ON_UPDATE,   new PositionCorrectionSolver() );
        addSimulator( POST_UPDATE, new NetForceZeroer() );
        addSimulator( POST_UPDATE, new MotionDampener() );
        addSimulator( POST_UPDATE, new GroundedDetector() );

        _iterations = 4;
        return true;
    }

    void Simulation::shutdown()
    {
        for (int i=0; i < MAX_STAGES; ++i )
        {
            _simulators[ i ].clear();
        }
        _rigidbodies = nullptr;
        //_collisions.clear();
    }

    Simulation::~Simulation()
    {
        shutdown();
    }

    Simulation::Simulation()
    :   _iterations( 0 )
    {}

}


