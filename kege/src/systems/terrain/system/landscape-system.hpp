//
//  landscape-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/28/24.
//

#ifndef landscape_system_hpp
#define landscape_system_hpp

#include "landscape.hpp"
#include "entity-components.hpp"

namespace kege{

    class LandscapeSystem : public kege::System
    {
    public:

        LandscapeSystem( kege::Engine* engine );
        //void render( core::HierarchicalRenderList& render_objects );
        void execute( kege::CommandBuffer& command_buffer );
        void update( double dms );
        bool initialize();
    };

}

#endif /* landscape_system_hpp */
