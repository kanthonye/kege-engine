//
//  rayhit-obb.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_obb_hpp
#define rayhit_obb_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against an Oriented Bounding Box (OBB)
    bool rayhitOBB(const Ray& ray, const OBB& obb, RayHit* out_hit);

}
#endif /* rayhit_obb_hpp */
