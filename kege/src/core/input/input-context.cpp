//
//  input-context.cpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/17/24.
//

#include "hashmap.hpp"
#include "json-parser.hpp"
#include "input-actions.hpp"
#include "input-context.hpp"
namespace kege{

    void InputContext::map( uint16_t keycode, const kege::InputCommand& info )
    {
        _inputs[ keycode ] = info;
    }

    const kege::InputCommand* InputContext::get( uint16_t keycode )const
    {
        InputMap::const_iterator i = _inputs.find( keycode );
        if( i == _inputs.end() )
        {
            return nullptr;
        }
        return &i->second;
    }

    kege::InputCommand* InputContext::get( uint16_t keycode )
    {
        InputMap::iterator i = _inputs.find( keycode );
        if( i == _inputs.end() )
        {
            return nullptr;
        }
        return &i->second;
    }

    InputContext::~InputContext()
    {
        _inputs.clear();
    }
}


