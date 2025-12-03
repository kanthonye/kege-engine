//
//  rigidbody-to-transform.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 7/4/25.
//

#ifndef rigidbody_to_transform_hpp
#define rigidbody_to_transform_hpp

#include "../../../math/algebra/vmath.hpp"
#include "../../../scene/ecs-system-factory.hpp"
#include "../simulation/physics-simulation.hpp"

namespace kege{

    class RigidbodyToTransform : public kege::ecs::System
    {
    public:

        RigidbodyToTransform( kege::ECS* ecs );
        void update( double dms );
    };
}


namespace kege{

    class TransformToRigidbody : public kege::ecs::System
    {
    public:

        TransformToRigidbody( kege::ECS* ecs );
        void update( double dms );
    };
}

#endif /* rigidbody_to_transform_hpp */
