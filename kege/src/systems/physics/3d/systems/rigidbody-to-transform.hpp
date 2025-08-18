//
//  rigidbody-to-transform.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 7/4/25.
//

#ifndef rigidbody_to_transform_hpp
#define rigidbody_to_transform_hpp

#include "../../../-/system-dependencies.hpp"
#include "../simulation/physics-simulation.hpp"

namespace kege{

    class RigidbodyToTransform : public kege::EntitySystem
    {
    public:

        RigidbodyToTransform( kege::Engine* engine );
        void update( double dms );
        bool initialize();
        void shutdown();
    };

}

#endif /* rigidbody_to_transform_hpp */
