//
//  input.cpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/17/24.
//

#include "inputs.hpp"

namespace kege{

    Input& Input::operator=( const Input& other )
    {
        if( other.type == POINTER || other.type == SCROLL )
        {
            coord = other.coord;
            type  = other.type;
        }
        else
        {
            key  = other.key;
            type = other.type;
        }
        return *this;
    }

    Input::Input( Type typ, uint16_t code, uint16_t state, int mods )
    {
        key.code  = code;
        key.state = state;
        key.mods  = mods;
        type = typ;
    }

    Input::Input( Type typ, double x, double y )
    {
        coord.x = x;
        coord.y = y;
        type = typ;
    }

    Input::Input( const Input& other )
    {
        if( other.type == POINTER || other.type == SCROLL )
        {
            coord = other.coord;
            type  = other.type;
        }
        else
        {
            key  = other.key;
            type = other.type;
        }
    }
}
