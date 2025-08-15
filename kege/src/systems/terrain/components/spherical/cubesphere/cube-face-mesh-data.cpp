//
//  patch-indice-array.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/5/24.
//

#include "cube-face-mesh-data.hpp"

namespace kege{

    uint32_t getCapSize( uint32_t count, uint32_t base, uint32_t exponent )
    {
        uint32_t exp = ceil( ::log( count ) / ::log( base ) );
        exp = kege::min( exp, exponent );
        uint32_t size = pow( base, exp );
        return ( size < base ) ? base : size;
    }

    QuadtreePatchNode::QuadtreePatchNode()
    :   children( nullptr )
    ,   depth( 0 )
    ,   visible( true )
    {}
    
    PatchData::PatchData()
    :   transform(0.f, 0.f, 0.f, 0.f)
    {
        patch_index_id = ( 0 );
        patch_vertex_id = ( 0 );
//        memset(elevations, 0x0, 36 );
    }
}
