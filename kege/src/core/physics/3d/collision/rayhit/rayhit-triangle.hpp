//
//  rayhit-triangle.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_triangle_hpp
#define rayhit_triangle_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a triangle
    bool rayhitTriangle(const Ray& ray, const Triangle& tri, RayHit* out_hit);

}
#endif /* rayhit_triangle_hpp */
