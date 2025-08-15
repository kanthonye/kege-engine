//
//  input.hpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/17/24.
//

#ifndef kege_input_hpp
#define kege_input_hpp

#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include "keycodes.hpp"

namespace kege{

    struct Key
    {
        int code, state, mods, scancode;
    };

    struct Coord2d
    {
        double x, y;
    };

    struct Input
    {
        enum Type: char{ KEYBOARD, MOUSEKEY, POINTER, SCROLL, ASCII_CODE, TOTAL_TYPE };

        Input& operator=( const Input& other );
        Input( Type typ, uint16_t code, uint16_t state, int mods );
        Input( Type typ, double x, double y );
        Input( const Input& other );
        Input(){}

        union
        {
            Coord2d coord;
            Key     key;
        };

        Type type;
    };

    typedef std::vector< kege::Input > InputEvents;

}

#endif /* kege_input_hpp */
