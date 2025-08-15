//
//  rayhit-pyramid.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_pyramid_hpp
#define rayhit_pyramid_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a pyramid shape
    bool rayhitPyramid(const Ray& ray, const Pyramid& pyramid, RayHit* out_hit);

}
#endif /* rayhit_pyramid_hpp */
