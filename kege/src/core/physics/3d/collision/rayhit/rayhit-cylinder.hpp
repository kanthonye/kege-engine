//
//  rayhit-cylinder.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_cylinder_hpp
#define rayhit_cylinder_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a cylinder
    bool rayhitCylinder(const Ray& ray, const Cylinder& cylinder, RayHit* out_hit);

}
#endif /* rayhit_cylinder_hpp */
