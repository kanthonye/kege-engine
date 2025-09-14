//
//  patch-indice-array.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/5/24.
//

#include "cube-face-mesh-data.hpp"

namespace kege{

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
    }
}
