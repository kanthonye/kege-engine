//
//  heightmap-layer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#ifndef heightmap_layer_hpp
#define heightmap_layer_hpp

#include <string>
#include <vector>
#include "../../-/system-dependencies.hpp"

namespace kege{

    // Forward declare Image class (replace with your own image implementation)
    class TopographicLayer : public RefCounter
    {
    public:

        float get( int32_t x, int32_t y )const;
        float& get( int32_t x, int32_t y );
        void fill(float value);
        TopographicLayer(int w, int h);

        std::vector<float> data; // Example data structure for simplicity
        int width, height;
    };

    // Forward declare Image class (replace with your own image implementation)
    class Normalmap : public RefCounter
    {
    public:

        const ubyte4& get( int32_t x, int32_t y )const;
        ubyte4& get( int32_t x, int32_t y );
        void fill(const ubyte4& value);
        Normalmap(int w, int h);

        std::vector< ubyte4 > data; // Example data structure for simplicity
        int width, height;
    };

}

#endif /* heightmap_layer_hpp */
