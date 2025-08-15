//
//  quadtree.h
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/28/24.
//

#ifndef kege_quadtree_h
#define kege_quadtree_h

#include "vectors.hpp"

namespace kege{

    template< typename T > struct QuadtreeChildren;

    template< typename T > struct Quadtree
    {
        T& operator ->()
        { return data; }
        
        Quadtree()
        : children( nullptr )
        , center( 0,0,0 )
        , diameter( 0 )
        , depth( 0 )
        {}

        T data;

        QuadtreeChildren< T >* children;
        dvec3 center;
        uint1 diameter;
        uint1 depth;
    };

    template< typename T > struct QuadtreeChildren
    {
        Quadtree< T > nodes[4];
    };

}

#endif /* kege_quadtree_h */
