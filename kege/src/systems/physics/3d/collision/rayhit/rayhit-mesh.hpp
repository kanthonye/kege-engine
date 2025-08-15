//
//  rayhit-mesh.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef rayhit_mesh_hpp
#define rayhit_mesh_hpp

#include "../../collision/algorithms/utils.hpp"

namespace kege::algo{

    // Tests ray against a polygon (flat and assumed convex)
    bool rayhitPolygon(const Ray& ray, const Polygon& polygon, RayHit* out_hit);

}
#endif /* rayhit_mesh_hpp */
