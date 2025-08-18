//
//  emitter.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "emitter.hpp"
namespace kege{

    uint32_t Emitter::update( double dms )
    {
        uint32_t count = 0;
        _time_accumulator += dms;
        double emission_rate = 1.0 / _emission_count;
        if ( emission_rate <= _time_accumulator )
        {
            count = ( _burst ) ? _emission_count : _time_accumulator / emission_rate;
            _time_accumulator -= emission_rate * count;
        }
        return count;
    }

    Emitter::Emitter
    (
        float emissions_per_second, // particles to emit every second
        bool burst
    )
    :   _emission_count( emissions_per_second )
    ,   _time_accumulator( 0.0 )
    ,   _burst( burst )
    {
    }

}
