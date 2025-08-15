//
//  gravity-behavior.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef gravity_behavior_hpp
#define gravity_behavior_hpp

#include "array.hpp"
#include "particle.hpp"
#include "behavior.hpp"

namespace kege{

    struct DirectionalGravity : public Behavior
    {
        void update( double dms, Particle& particle ) override;
        DirectionalGravity( const kege::vec3& force );
        kege::vec3 force;
    };

    struct CenterOfMassGravity : public Behavior
    {
        void update( double dms, Particle& particle ) override;

        CenterOfMassGravity
        (
            float strength = 9.8f, // Default Earth gravity (m/s²)
            bool  falloff = true // Enable inverse-square law (like real gravity)
        );

        float strength;
        bool  falloff;
    };

}

#endif /* gravity_behavior_hpp */
