//
//  rayhit-capsule.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_capsule_hpp
#define rayhit_capsule_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a capsule (cylinder with hemispheres at ends)
    bool rayhitCapsule(const Ray& ray, const Capsule& capsule, RayHit* out_hit);

}
#endif /* rayhit_capsule_hpp */
