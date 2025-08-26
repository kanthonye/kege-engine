//
//  normal-map-generator.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/16/25.
//

#ifndef normal_map_generator_hpp
#define normal_map_generator_hpp

#include "../../-/system-dependencies.hpp"
#include "topographic-layer-generator.hpp"

namespace kege{

    // Example TopographicLayerGenerator: HeightmapGenerator
    class NormalmapGenerator : public TopographicLayerGenerator
    {
    public:

        void generate( double tx, double ty, Ref< TerrainTopography >& topography )const override;
        NormalmapGenerator(double strength, int width);

        double _strength;
        int _width;
    };

}
#endif /* normal_map_generator_hpp */
