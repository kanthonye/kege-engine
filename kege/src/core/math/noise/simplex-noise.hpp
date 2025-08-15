//
//  3d-simplex-noise.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef _d_simplex_noise_hpp
#define _d_simplex_noise_hpp

#include <vector>
#include <cmath>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <random>

namespace kege{

    std::vector< int > generatePermutationTable( unsigned seed );
    
    // Simplex noise 2D function
    double simplex2D( double x, double y, const std::vector<int>& perm);

    // Simplex noise 3D function
    double simplex3D( double x, double y, double z, const std::vector< int >& perm );

}
#endif /* _d_simplex_noise_hpp */
