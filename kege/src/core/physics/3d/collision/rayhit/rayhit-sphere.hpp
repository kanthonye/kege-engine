//
//  rayhit-sphere.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_sphere_hpp
#define rayhit_sphere_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a sphere
    bool rayhitSphere(const Ray& ray, const Sphere& sphere, RayHit* out_hit);

    bool rayhitCircle( const Ray& ray, const Circle& circle, RayHit* result );
}
#endif /* rayhit_sphere_hpp */
