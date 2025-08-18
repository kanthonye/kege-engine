//
//  graphics-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#ifndef graphics_system_hpp
#define graphics_system_hpp

#include "module.hpp"
#include "../../core/graphics/core/graphics.hpp"

namespace kege{

    class GraphicsModule : public Module
    {
    public:

        kege::Ref< kege::Graphics > operator ->() { return _module; }
        const kege::Ref< kege::Graphics > operator ->() const { return _module; }
        operator bool()const{ return _module != nullptr; }

        kege::Graphics* get();
        bool initialize();
        void shutdown();
        void add();

        GraphicsModule( kege::Engine* engine );

    private:
        
        // This is the graphics system that will be used to create the swapchain and other graphics resources
        // It will also be used to create the render graph and other graphics related systems
        // The graphics system will be used to manage rendering operations across multiple frames and handle resources efficiently.
        // It will also be used to create the graphics window and handle input events.
        // The graphics system will be used to create the graphics API and handle graphics related operations.
        kege::Ref< kege::Graphics > _module;
    };

}
#endif /* graphics_system_hpp */
