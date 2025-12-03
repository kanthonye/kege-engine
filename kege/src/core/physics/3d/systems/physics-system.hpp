//
//  physics-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#ifndef physics_system_hpp
#define physics_system_hpp

#include "../../../math/algebra/vmath.hpp"
#include "../../../scene/ecs-system-factory.hpp"
#include "../simulation/physics-simulation.hpp"

namespace kege{

    class PhysicsSystem : public kege::ecs::System
    {
    public:

        physics::Simulation& getPhysicsSimulation();
        void update( double dms );
        //bool initialize();
        //void shutdown();

        PhysicsSystem( kege::ECS* ecs );

    private:

        physics::Simulation _simulation;
    };

}

#endif /* physics_system_hpp */
