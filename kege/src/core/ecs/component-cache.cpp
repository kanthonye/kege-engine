//
//  component-cache.cpp
//  physics
//
//  Created by Kenneth Esdaile on 4/20/25.
//

#include "component-cache.hpp"

namespace kege{

    uint32_t ComponentCache::_type_counter = 0;

    Relationship compare(const kege::EntitySignature& a, const kege::EntitySignature& b)
    {
        auto intersection = a & b;
        if (intersection == b) return b == a ? EQUAL : SUBSET;
        if (intersection == a) return SUPERSET;
        return intersection.any() ? OVERLAP : DISJOINT;
    }

}
