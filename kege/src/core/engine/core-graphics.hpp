//
//  graphics-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#ifndef graphics_system_hpp
#define graphics_system_hpp

#include "../../core/system/system.hpp"
#include "../../core/graphics/core/graphics.hpp"

namespace kege{

    class CoreGraphics : public CoreSystem< kege::Graphics >
    {
    public:

        CoreGraphics( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}
#endif /* graphics_system_hpp */
