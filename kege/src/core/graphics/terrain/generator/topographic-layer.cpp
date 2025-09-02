//
//  heightmap-layer.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/15/24.
//

#include "topographic-layer.hpp"

namespace kege{

    float TopographicLayer::get( int32_t x, int32_t y )const
    {
        return data[ x + y * width ];
    }
    float& TopographicLayer::get( int32_t x, int32_t y )
    {
        return data[ x + y * width ];
    }
    void TopographicLayer::fill(float value)
    {
        std::fill(data.begin(), data.end(), value);
    }
    TopographicLayer::TopographicLayer(int w, int h)
    :   width(w)
    ,   height(h)
    ,   data(w * h, 0.0f)
    {}



    const ubyte4& Normalmap::get( int32_t x, int32_t y )const
    {
        return data[ x + y * width ];
    }
    ubyte4& Normalmap::get( int32_t x, int32_t y )
    {
        return data[ x + y * width ];
    }
    void Normalmap::fill(const ubyte4& value)
    {
        std::fill(data.begin(), data.end(), value);
    }
    Normalmap::Normalmap(int w, int h)
    :   width(w)
    ,   height(h)
    ,   data(w * h)
    {}

}
