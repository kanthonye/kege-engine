//
//  terrain-tile-chunk.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#include "../common/all.hpp"
#include "terrain-chunk.hpp"




struct NoiseLayer
{
    std::string name;
    int operation;
    int generator;
    float weight;
    kege::vec2 offset;
};

struct NoiseParameters
{
    int seed;
    int octaves;
    float frequency;
    float lacunarity;
    float persistence;
    float scale;
    float warp_strength;
    bool warp;
};

struct NoiseGenerator
{
    std::string name;
    std::string type;
    NoiseParameters parameters;
};

struct TerrainGeneration
{
    std::string name;

    std::vector< NoiseLayer > layers;
    std::vector< NoiseGenerator > generators;

    bool recompute;
};


std::vector< float > generate
(
    int width,
    const kege::dvec3& offset,
    const kege::mat33& axes,
    const TerrainGeneration* gen,
    const std::vector< int >* permutation_table = &kege::getPermutationTable3D()
)
{
    std::vector< float > map;
    map.resize( width * width );

    int index = 0;
    double amplitude;
    double frequency;
    double max_amplitude;
    double n, sum, scale;
    double net_sum = 0.0f;

    const NoiseLayer* layer = nullptr;
    const NoiseParameters* parameters = nullptr;
    const double half_size = double( width ) * 0.5;
    const double step_size = 1.0 / double( width );

    double a = 3, b = 1.3, nx,ny,nz;

    for (float y = -half_size; y <= half_size; y += step_size )
    {
        for (float x = -half_size; x <= half_size; x += step_size )
        {
            nx = offset.x + x * axes[ 0 ].x + y * axes[ 1 ].x + axes[2].x;
            ny = offset.y + x * axes[ 0 ].y + y * axes[ 1 ].y + axes[2].y;
            nz = offset.z + x * axes[ 0 ].z + y * axes[ 1 ].z + axes[2].z;

            for (int l = 0; l < gen->layers.size(); ++l)
            {
                layer = &gen->layers[l];
                parameters = &gen->generators[ layer->generator ].parameters;

                scale = 1.0  / parameters->scale;
                max_amplitude = 0.0f;
                amplitude = 1.0; // Reduces the impact of each octave as the detail increases.
                frequency = 1.0; // Scales the input for higher detail in subsequent octaves.
                sum = 0.0f;

                for (int k = 0; k < parameters->octaves; ++k)
                {
                    n = amplitude * kege::perlin3D
                    (
                        (nx * frequency) * scale,
                        (ny * frequency) * scale,
                        (nz * frequency) * scale,
                        *permutation_table
                    );

                    k = pow(n, a);
                    n = k / (k + pow(b - b * n, a));

                    sum += n;

                    max_amplitude += amplitude;
                    amplitude *= parameters->persistence;
                    frequency *= parameters->lacunarity;
                }

                sum /= max_amplitude;

                switch ( layer->operation )
                {
                    default:
                    case 0: net_sum += sum * layer->weight; break;
                    case 1: net_sum -= sum * layer->weight; break;
                    case 2: net_sum *= sum * layer->weight; break;
                }
            }

            map[ index ] = net_sum / double( gen->layers.size() );
            index += 1;
        }
    }
    return map;
}
