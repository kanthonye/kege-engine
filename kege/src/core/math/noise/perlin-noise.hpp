//
//  noise.hpp
//  column-major
//
//  Created by Kenneth A Esdaile on 7/13/18.
//  Copyright © 2018 Kenneth A Esdaile. All rights reserved.
//

#ifndef KEGE_PERLIN_NOISE_FUNCTION
#define KEGE_PERLIN_NOISE_FUNCTION

#include <vector>
#include "../../math/algebra/vmath.hpp"

namespace kege{


    double fade(double t);
    const std::vector<int>& getPermutationTable3D();
    std::vector<int> generatePermutationTable3D(int seed);
    double perlin2D(double x, double y, const std::vector<int>& perm);
    double perlin3D(double x, double y, double z, const std::vector<int>& perm);

}
#endif /* KEGE_PERLIN_NOISE_FUNCTION */
