//
//  load-input-context.cpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/18/24.
//

#include "input-actions.hpp"

namespace kege{

    #define KEGE_INPUT_ACTION(s) #s
    const char* enumActionToString( int type )
    {
        static const char *actions[] = { KEGE_INPUT_ACTION_ENUMS };
        return actions[ type ];
    };

};

