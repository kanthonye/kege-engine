//
//  size-over-lifetime.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef size_over_lifetime_hpp
#define size_over_lifetime_hpp

#include "array.hpp"
#include "particle.hpp"
#include "behavior.hpp"

namespace kege{

    struct SizeOverLifetime : public Behavior
    {
        void update( double dms, Particle& particle );
        SizeOverLifetime( const array< float >& curve );
        array< float > curve;
    };

}

#endif /* size_over_lifetime_hpp */
