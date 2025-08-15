//
//  physics-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#ifndef physics_system_hpp
#define physics_system_hpp

#include "entity-system.hpp"
#include "physics-simulation.hpp"

namespace kege{

    class PhysicsSystem : public kege::EntitySystem
    {
    public:

        physics::Simulation& getPhysicsSimulation();
        void update( double dms );
        bool initialize();
        void shutdown();

        PhysicsSystem( kege::Engine* engine );

    private:

        physics::Simulation _simulation;
    };

}

#endif /* physics_system_hpp */
