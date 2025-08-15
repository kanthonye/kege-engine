//
//  3d-simplex-noise.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#include "perlin-noise.hpp"
#include "simplex-noise.hpp"

namespace kege{

    // Gradient directions for 2D Simplex Noise
    const int grad2[8][2] = {
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    // Dot product of gradient vector and distance vector
    inline double dot(const int* g, double x, double y) {
        return g[0] * x + g[1] * y;
    }

    // 2D Simplex noise function
    double simplex2D(double x, double y, const std::vector<int>& perm) 
    {
        const double F2 = 0.5f * (std::sqrt(3.0f) - 1.0f); // Skewing factor
        const double G2 = (3.0f - std::sqrt(3.0f)) / 6.0f; // Unskewing factor

        // Skew the input space to determine the simplex cell
        double s = (x + y) * F2;
        int i = (int)std::floor(x + s);
        int j = (int)std::floor(y + s);

        // Unskew the cell origin back to (x, y) space
        double t = (i + j) * G2;
        double X0 = i - t;
        double Y0 = j - t;

        // Distances from cell origin
        double x0 = x - X0;
        double y0 = y - Y0;

        // Determine which simplex triangle we are in
        int i1, j1;
        if (x0 > y0) {
            i1 = 1; j1 = 0; // Lower triangle
        } else {
            i1 = 0; j1 = 1; // Upper triangle
        }

        // Offsets for the remaining simplex corners
        double x1 = x0 - i1 + G2;
        double y1 = y0 - j1 + G2;
        double x2 = x0 - 1.0f + 2.0f * G2;
        double y2 = y0 - 1.0f + 2.0f * G2;

        // Hash the simplex corner coordinates
        int ii = i & 255;
        int jj = j & 255;
        int g0 = perm[ii + perm[jj]] % 8;
        int g1 = perm[ii + i1 + perm[jj + j1]] % 8;
        int g2 = perm[ii + 1 + perm[jj + 1]] % 8;

        // Calculate the contribution from the three corners
        double t0 = 0.5f - x0 * x0 - y0 * y0;
        double n0 = t0 < 0 ? 0.0f : (t0 * t0) * dot(grad2[g0], x0, y0);

        double t1 = 0.5f - x1 * x1 - y1 * y1;
        double n1 = t1 < 0 ? 0.0f : (t1 * t1) * dot(grad2[g1], x1, y1);

        double t2 = 0.5f - x2 * x2 - y2 * y2;
        double n2 = t2 < 0 ? 0.0f : (t2 * t2) * dot(grad2[g2], x2, y2);

        // Return the sum scaled to the range [-1, 1]
        //return 70.0f * (n0 + n1 + n2);
        return fmax( -1.0, fmin( 1.0, 70.0f * (n0 + n1 + n2) ) );
    }

}



namespace kege{

    // Helper function to lerp between two values smoothly
    double lerp(double a, double b, double t) {
        return a + t * (b - a);
    }

    // Helper function to calculate the dot product of two 3D vectors
    double dot3(const int *g, double x, double y, double z) {
        return g[0] * x + g[1] * y + g[2] * z;
    }

    // Gradient directions for 3D Simplex Noise
    const int grad3[12][3] =
    {
        {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
        {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
        {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
    };
    // Precomputed gradients for 1D Simplex noise
    const int G3[3] = {1, 1, 0};

    // Simplex noise 3D function
    double simplex3D( double x, double y, double z, const std::vector< int >& perm )
    {
        // Skew the input space to determine which simplex cell we're in
        float s = (x + y + z) / 3.0f;
        int i = floor(x + s);
        int j = floor(y + s);
        int k = floor(z + s);

        float t = (i + j + k) / 3.0f;
        float x0 = x - (i - t);
        float y0 = y - (j - t);
        float z0 = z - (k - t);

        // Determine the largest coordinate and its index
        int i1, j1, k1;
        int i2, j2, k2;
        if (x0 >= y0) {
            if (y0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0;
                i2 = 1; j2 = 1; k2 = 0;
            } else if (x0 >= z0) {
                i1 = 1; j1 = 0; k1 = 0;
                i2 = 1; j2 = 0; k2 = 1;
            } else {
                i1 = 0; j1 = 0; k1 = 1;
                i2 = 1; j2 = 0; k2 = 1;
            }
        } else {
            if (y0 >= z0) {
                i1 = 0; j1 = 1; k1 = 0;
                i2 = 1; j2 = 1; k2 = 0;
            } else if (x0 >= z0) {
                i1 = 0; j1 = 0; k1 = 1;
                i2 = 1; j2 = 0; k2 = 1;
            } else {
                i1 = 0; j1 = 0; k1 = 1;
                i2 = 0; j2 = 1; k2 = 1;
            }
        }

        // Calculate the unskewed coordinates
        float x1 = x0 - i1 + G3[i1];
        float y1 = y0 - j1 + G3[j1];
        float z1 = z0 - k1 + G3[k1];
        float x2 = x0 - i2 + G3[i2];
        float y2 = y0 - j2 + G3[j2];
        float z2 = z0 - k2 + G3[k2];
        float x3 = x0 - 1.0f + G3[0];
        float y3 = y0 - 1.0f + G3[1];
        float z3 = z0 - 1.0f + G3[2];

        // Calculate the contribution from each corner
        float n0, n1, n2, n3;
        n0 = dot3(grad3[i + j * 3 + k * 9], x0, y0, z0);
        n1 = dot3(grad3[i + i1 + (j + j1) * 3 + (k + k1) * 9], x1, y1, z1);
        n2 = dot3(grad3[i + i2 + (j + j2) * 3 + (k + k2) * 9], x2, y2, z2);
        n3 = dot3(grad3[0], x3, y3, z3);

        // Interpolate the contributions
        float t0 = fade(x0);
        float t1 = fade(y0);
        float t2 = fade(z0);
        float u = lerp(n0, n1, t0);
        float v = lerp(n2, n3, t0);
        return lerp(lerp(u, v, t1), lerp(lerp(n1, n2, t1), lerp(n0, n3, t1), t2), t2);
    }


    // Generates a random permutation table
    std::vector< int > generatePermutationTable( unsigned seed )
    {
        std::vector< int > perm( 512 );
        std::mt19937 generator( seed );
        std::iota( perm.begin(), perm.begin() + 256, 0 );
        std::shuffle( perm.begin(), perm.begin() + 256, generator );
        for (int i = 0; i < 256; ++i)
        {
            perm[256 + i] = perm[i];
        }
        return perm;
    }

//    // Generates a random permutation table for seamless tiling
//    std::vector<int> generatePermutationTable(unsigned seed) {
//        std::vector<int> perm(512);
//        std::mt19937 generator(seed);
//        std::iota(perm.begin(), perm.begin() + 256, 0);
//        std::shuffle(perm.begin(), perm.begin() + 256, generator);
//        for (int i = 0; i < 256; ++i) {
//            perm[256 + i] = perm[i];
//        }
//        return perm;
//    }

}
