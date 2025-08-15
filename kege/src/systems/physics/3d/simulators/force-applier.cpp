//
//  force-applier.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "physics-simulation.hpp"
#include "force-applier.hpp"

namespace kege::physics{

    void ForceApplier::addForce( const kege::Ref< ForceDriver >& force_driver )
    {
        _force_drivers.push_back( force_driver );
        //kege::Ref< ForceDriver >& force = _force_drivers[_force_drivers.size()-1];
        //Setting::singleton().add< bool >( force->name, &force->enabled );
    }

    void ForceApplier::simulate( double dms )
    {
        kege::Rigidbody* body;

        for (kege::Ref< ForceDriver >& force : _force_drivers )
        {
            if ( !force->enabled )
                continue;

            for (ComponentCacheT< Rigidbody >::Iterator body = _simulator->rigidbodies().begin(); body != _simulator->rigidbodies().end(); body++ )
            {
                if ( !body->immovable )
                {
                    force->apply( dms, *body );
                }
            }
        }
    }
}
