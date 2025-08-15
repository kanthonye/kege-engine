//
//  heightmap-generator.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef terrain_map_generator_hpp
#define terrain_map_generator_hpp

#include "topographic-layer-generator.hpp"

namespace kege{

    // Manages all heightmap layers
    class TerrainTopographyGenerator : public RefCounter
    {
    public:

        Ref< TerrainTopography > generate( double x, double y )const;
        void addSurfaceGenerator( Ref< TopographicLayerGenerator > generator );
        virtual ~TerrainTopographyGenerator();
        
    private:

        std::vector< Ref< TopographicLayerGenerator > > _topographic_layer_generators;
    };

}

#endif /* terrain_map_generator_hpp */
