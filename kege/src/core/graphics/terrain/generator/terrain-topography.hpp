//
//  topography-map.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef topography_map_hpp
#define topography_map_hpp

#include <string>
#include "topographic-layer.hpp"
#include "image-layer-manager.hpp"

namespace kege{


    // The output object class containing the collection of images
    struct TerrainTopography : public RefCounter
    {
    public:

        Ref< TopographicLayer > heightmap;
        Ref< Normalmap > normalmap;
        Ref< TopographicLayer > vegemap;
    };

}

#endif /* topography_map_hpp */
