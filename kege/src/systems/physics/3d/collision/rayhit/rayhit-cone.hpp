//
//  rayhit-cone.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_cone_hpp
#define rayhit_cone_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a finite cone
    bool rayhitCone(const Ray& ray, const Cone& cone, RayHit* out_hit);

}
#endif /* rayhit_cone_hpp */
