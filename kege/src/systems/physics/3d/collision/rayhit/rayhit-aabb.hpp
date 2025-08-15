//
//  rayhit-aabb.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_aabb_hpp
#define rayhit_aabb_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against an Axis-Aligned Bounding Box (AABB)
    bool rayhitAABB(const Ray& ray, const AABB& aabb, RayHit* out_hit);

}
#endif /* rayhit_aabb_hpp */
