//
//  input-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef input_system_hpp
#define input_system_hpp

#include "../../core/system/system.hpp"
#include "../../core/input/input-context.hpp"

namespace kege{

    class InputSystem : public System
    {
    public:

        InputSystem( kege::Engine* engine );
        bool initialize()override;
        void shutdown()override;
    };

}


#endif /* input_system_hpp */
