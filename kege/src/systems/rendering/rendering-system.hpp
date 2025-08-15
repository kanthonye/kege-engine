//
//  rendering-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/27/25.
//

#ifndef kege_rendering_system_hpp
#define kege_rendering_system_hpp

#include "system.hpp"
#include "render-graph.hpp"

namespace kege{

    class RenderingSystem : public kege::System
    {
    public:

        void operator()( const MsgExecuteRenderProcess& pass );

        bool initialize();
        void shutdown();
        
        RenderingSystem( kege::Engine* engine );
    };

}
#endif /* kege_render_pipeline_system_hpp */
