//
//  noise.cpp
//  column-major
//
//  Created by Kenneth A Esdaile on 7/13/18.
//  Copyright © 2018 Kenneth A Esdaile. All rights reserved.
//

#include "perlin-noise.hpp"

namespace kege{

    const std::vector<int>& getPermutationTable3D()
    {
        static std::vector<int> perm;
        if ( perm.empty() )
        {
            int p[512] =
            {
                151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
                140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
                247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
                57 ,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
                74 ,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
                60 ,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
                65 , 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
                200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
                52 ,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
                207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
                119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
                129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
                218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
                81 , 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
                184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
                222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180,

                151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
                140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
                247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
                57 ,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
                74 ,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
                60 ,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
                65 , 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
                200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
                52 ,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
                207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
                119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
                129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
                218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
                81 , 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
                184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
                222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
            };
            perm.resize( 512 );
            memcpy( perm.data(), p, 512*sizeof(int) );
        }
        return perm;
    }

    double fade(double t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double grad(int hash, double x, double y)
    {
        int h = hash & 3; // Take the first 2 bits of the hash
        double u = h < 2 ? x : y;
        double v = h < 2 ? y : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }


    // Dot product for 3D gradients
    double grad(int hash, double x, double y, double z)
    {
        // Hash to 12 gradient directions
        int h = hash & 15;
        double u = (h < 8) ? x : y;
        double v = (h < 4) ? y : (h == 12 || h == 14) ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
//    double grad(int hash, double x, double y, double z)
//    {
//        int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
//        double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
//        v = h<4 ? y : h==12||h==14 ? x : z;
//        return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
//    }

    // Perlin noise function
    double perlin2D(double x, double y, const std::vector<int>& perm)
    {
        return perlin3D( x, 0, y, perm );
    }

    double perlin3D(double x, double y, double z, const std::vector<int>& perm) {
        // Unit cube coordinates
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        int Z = (int)std::floor(z) & 255;

        // Local coordinates inside the unit cube
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        // Fade curves
        float u = fade(x);
        float v = fade(y);
        float w = fade(z);

        // Hash coordinates of cube corners
        int A  = (perm[  X  ] + Y) & 255;
        int AA = (perm[  A  ] + Z) & 255;
        int AB = (perm[A + 1] + Z) & 255;
        int B  = (perm[X + 1] + Y) & 255;
        int BA = (perm[  B  ] + Z) & 255;
        int BB = (perm[B + 1] + Z) & 255;

        // Add blended results from all corners
        float res = lerp(
                         lerp(
                              lerp(grad(perm[AA], x, y, z),
                                   grad(perm[BA], x - 1, y, z),
                                   u),
                              lerp(grad(perm[AB], x, y - 1, z),
                                   grad(perm[BB], x - 1, y - 1, z),
                                   u),
                              v),
                         lerp(
                              lerp(grad(perm[AA + 1], x, y, z - 1),
                                   grad(perm[BA + 1], x - 1, y, z - 1),
                                   u),
                              lerp(grad(perm[AB + 1], x, y - 1, z - 1),
                                   grad(perm[BB + 1], x - 1, y - 1, z - 1),
                                   u),
                              v),
                         w);

        return res;
    }

//    double perlin3D(double x, double y, double z, const std::vector<int>& perm)
//    {
//        int X = (int)floor(x) & 255; // FIND UNIT CUBE THAT
//        int Y = (int)floor(y) & 255; // CONTAINS POINT.
//        int Z = (int)floor(z) & 255;
//
//        x -= floor(x); // FIND RELATIVE X,Y,Z
//        y -= floor(y); // OF POINT IN CUBE.
//        z -= floor(z);
//
//        double u = fade(x); // COMPUTE FADE CURVES
//        double v = fade(y); // FOR EACH OF X,Y,Z.
//        double w = fade(z);
//
//        int A  = perm[ X ] + Y;
//        int AA = perm[ A ] + Z;
//        int AB = perm[ A+1 ] + Z;
//        int B  = perm[ X+1 ] + Y;
//        int BA = perm[ B ] + Z;
//        int BB = perm[ B+1 ] + Z;
//
//        double l0 = kege::lerp( grad( perm[AB+1], x  , y-1, z-1 ), grad( perm[BB+1], x-1, y-1, z-1 ), u );
//        double l1 = kege::lerp( grad( perm[AA+1], x  , y  , z-1 ), grad( perm[BA+1], x-1, y  , z-1 ), u );
//        double l3 = kege::lerp( grad( perm[AB  ], x  , y-1, z   ), grad( perm[BB  ], x-1, y-1, z   ), u );
//        double l4 = kege::lerp( grad( perm[AA  ], x  , y  , z   ), grad( perm[BA  ], x-1, y  , z   ), u );
//        return kege::lerp( kege::lerp( l4, l3, v ), kege::lerp( l1, l0, v ), w );
//    }

    std::vector<int> generatePermutationTable3D(int seed)
    {
        srand(seed);
        std::vector<int> perm(512);
        for (int i=0; i<256; i++)
        {
            perm[ i ] = rand() % 255;
            perm[ i + 256 ] = perm[ i ];
        }
        return perm;
    }
    
}
