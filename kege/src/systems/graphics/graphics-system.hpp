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

    class GraphicsSystem : public System
    {
    public:

        GraphicsSystem( kege::Engine* engine );
        bool initialize()override;
        void shutdown()override;

    private:

        kege::Ref< kege::GraphicsWindow > _window;
        kege::Ref< kege::Graphics > _graphics;

    };

}
#endif /* graphics_system_hpp */
