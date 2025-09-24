//
//  transform.cpp
//  kege
//
//  Created by Kenneth Esdaile on 11/20/24.
//

#include "transform.hpp"

namespace kege{

    ModelMatrices buildModelMatrice( const kege::vec3& position, const kege::quat& orientation, const kege::vec3& scale )
    {
        ModelMatrices mm;
        mat44 rotation = kege::quatToM44( orientation );
        mm.transform[ 0 ] = kege::vec4( rotation[ 0 ].xyz * scale.x, 0.0 );
        mm.transform[ 1 ] = kege::vec4( rotation[ 1 ].xyz * scale.y, 0.0 );
        mm.transform[ 2 ] = kege::vec4( rotation[ 2 ].xyz * scale.z, 0.0 );
        mm.transform[ 3 ] = kege::vec4( position, 1.0 );
        return mm;
    }

}
