//
//  simulation.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef simulation_hpp
#define simulation_hpp

#include "../simulators/simulator.hpp"
#include "../bodies/rigidbody.hpp"
#include "../../../ecs/component-cache.hpp"

namespace kege::physics{

    class Simulation
    {
    public:

        enum Stage{PRE_UPDATE, ON_UPDATE, POST_UPDATE, MAX_STAGES };
        void addSimulator( Stage stage, Simulator* simulator );

        kege::CollisionRegistry& getCollisionRegistry();

        ComponentCacheT< Rigidbody >& rigidbodies();

//        Rigidbody* getRigidbody( Key id );
//        void deleteRigidbody( Key id );
//        Key createRigidbody();

        void simulate( double dms );
        bool initialize( ComponentCacheT< Rigidbody >* components );
        void shutdown();

        ~Simulation();
        Simulation();

    private:

        void update( Stage stage, double dms );

    private:

        std::vector< Ref< Simulator > > _simulators[ MAX_STAGES ];
        ComponentCacheT< Rigidbody >* _rigidbodies;
        kege::CollisionRegistry _collisions;
        int _iterations;

        friend class System;
    };
}

#endif /* simulation_hpp */
