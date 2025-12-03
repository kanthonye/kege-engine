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

namespace kege::physics{

    class Simulation
    {
    public:

        enum Stage{PRE_UPDATE, ON_UPDATE, POST_UPDATE, MAX_STAGES };
        void addSimulator( Stage stage, Simulator* simulator );

        kege::CollisionRegistry& getCollisionRegistry();
        ecs::ViewT< Rigidbody >& rigidbodies();

//        Rigidbody* getRigidbody( Key id );
//        void deleteRigidbody( Key id );
//        Key createRigidbody();

        void simulate( double dms, ecs::ViewT< Rigidbody >& rigidbodies );
        bool initialize();
        void shutdown();

        ~Simulation();
        Simulation();

    private:

        void update( Stage stage, double dms );

    private:

        std::vector< Ref< Simulator > > _simulators[ MAX_STAGES ];
        kege::CollisionRegistry _collisions;
        ecs::ViewT< Rigidbody >* _rigidbodies;
        int _iterations;
        friend class System;
    };
}

#endif /* simulation_hpp */
