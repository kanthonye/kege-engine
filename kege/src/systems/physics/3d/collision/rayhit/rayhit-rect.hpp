//
//  rayhit-rect.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_rect_hpp
#define rayhit_rect_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a rectangle on a plane (2D extent in 3D space)
    bool rayhitRect(const Ray& ray, const Rect& rect, RayHit* out_hit);

}
#endif /* rayhit_rect_hpp */
