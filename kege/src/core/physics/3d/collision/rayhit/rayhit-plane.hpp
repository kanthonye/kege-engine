//
//  rayhit-plane.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_plane_hpp
#define rayhit_plane_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a plane (can be one-sided)
    bool rayhitPlane(const Ray& ray, const Plane& plane, RayHit* out_hit);

}
#endif /* rayhit_plane_hpp */
