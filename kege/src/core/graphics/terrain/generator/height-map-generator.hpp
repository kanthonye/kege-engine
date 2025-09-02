//
//  height-map-generator.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef height_map_generator_hpp
#define height_map_generator_hpp

#include "ref.hpp"
#include "vectors.hpp"
#include "perlin-noise.hpp"
#include "simplex-noise.hpp"
#include "topographic-layer-generator.hpp"

namespace kege{

    struct FractalNoiseParams;
    struct PermutationTable3D : public RefCounter
    {
        PermutationTable3D( const std::vector< int >& t ): table(t) {}
        PermutationTable3D(){}
        std::vector< int > table;
    };

    struct FractalNoiseParams
    {
        Ref< PermutationTable3D >   permutation;

        vec3                        offset;
        uint32_t                    octaves;       // Repeated layers of simplex noise.
        double                      persistance;   // Determines how amplitude decreases per octave.
        double                      lacunarity;    // Determines how frequency increases per octave.
        double                      scale;

        double                      strength;
        double                      steepness;
        double                      smax;
        double                      smin;
    };

    typedef double (*NoiseFunct2D)( double x, double y, const std::vector<int>& perm );



    struct HeightmapSetting
    {
        vec3        offset;

        double      smax;
        double      smin;
        double      strength;
        double      steepness;

        double      persistance;   // Determines how amplitude decreases per octave.
        double      lacunarity;    // Determines how frequency increases per octave.
        double      scale;

        uint32_t    octaves;       // Repeated layers of simplex noise.
    };


    struct HeightmapLayerSetting
    {
        Ref< PermutationTable3D > permutation;
        HeightmapSetting heightmap;
        NoiseFunct2D noiseFunct;
    };

    typedef double (*HeightmapOpFunct)(double,double);


    // Example TopographicLayerGenerator: HeightmapGenerator
    class HeightmapGenerator : public TopographicLayerGenerator
    {
    public:

        void generate( double tx, double ty, Ref< TerrainTopography >& topography )const override;

        double noise( double x, double y, const HeightmapLayerSetting* layer_settings )const;

        static double rigidNoise(double x, double y, const std::vector<int>& perm)
        {
            return 1.0 - abs( perlin2D( x, y, perm ) );
        }

        static double fractalNoise(double x, double y, const std::vector<int>& perm)
        {
            return 0.5 * (1.0 + perlin2D( x, y, perm ));
        }

        static double opAdd(double x, double y)
        {
            return x + y;
        }

        static double opSub(double x, double y)
        {
            return x - y;
        }

        static double opMul(double x, double y)
        {
            return x * y;
        }


        HeightmapGenerator
        (
            int width, int terrain_width, 
            const std::vector< HeightmapLayerSetting >& settings,
            HeightmapOpFunct heightmapOpFunct = HeightmapGenerator::opAdd
        );
        
        ~HeightmapGenerator(){}
        HeightmapGenerator();

        std::vector< HeightmapLayerSetting > _settings;
        HeightmapOpFunct _heightmapOpFunct;
        int _terrain_width;
        int _width;
    };

}
#endif /* height_map_generator_hpp */
