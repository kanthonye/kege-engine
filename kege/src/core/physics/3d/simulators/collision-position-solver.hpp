//
//  collision-position-resolver.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#ifndef collision_position_resolver_hpp
#define collision_position_resolver_hpp

#include "simulator.hpp"

namespace kege::physics{

    struct PositionCorrectionSolver : public Simulator
    {
    public:

        void rotationalCorrection( double dms, std::vector< CollisionManifold* >& sorted_collisions );
        void linearCorrection( double dms, std::vector< CollisionManifold* >& sorted_collisions );
        void simulate( double dms );

        PositionCorrectionSolver();

    private:

        int _iterations;
    };

}
#endif /* collision_position_resolver_hpp */
