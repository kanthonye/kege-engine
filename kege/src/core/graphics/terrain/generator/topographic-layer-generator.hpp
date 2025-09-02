//
//  heightmap-layer-generator.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef heightmap_layer_generator_hpp
#define heightmap_layer_generator_hpp

#include <string>
#include "terrain-topography.hpp"

namespace kege{

    class TopographicOperation;

    // Abstract base class for all heightmap modules
    class TopographicLayerGenerator : public RefCounter
    {
    public:

        // Generate a result image based on input heightmap
        virtual void generate( double x, double y, Ref< TerrainTopography >& topography )const = 0;
        virtual ~ TopographicLayerGenerator() = default;
    };

}
#endif /* heightmap_layer_generator_hpp */
