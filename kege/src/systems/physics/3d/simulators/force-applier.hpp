//
//  force-applier.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef force_applier_hpp
#define force_applier_hpp

#include "../dynamics/rigidbody.hpp"
#include "../simulators/simulator.hpp"

namespace kege::physics{

    struct ForceDriver : public RefCounter
    {
        virtual void apply( double dms, Rigidbody* body );
        bool enabled;
    };

    struct ForceApplier : public Simulator
    {
    public:

        void addForce( const kege::Ref< ForceDriver >& force_driver );
        void simulate( double time_step );

        std::vector< kege::Ref< ForceDriver > > _force_drivers;
    };

    struct GravityForceDriver : public ForceDriver
    {
        void update( double dms, kege::Rigidbody* body )
        {
            if ( !body->anti_gravity && body->is_awake )
            {
                body->linear.forces -= 9.8 * (1.0 / body->linear.invmass) * body->up;
            }
        }
        //GravityForceDriver(std::string name): ForceDriver(name) {}
    };
}
#endif /* force_applier_hpp */
