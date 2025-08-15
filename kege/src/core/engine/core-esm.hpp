//
//  core-esm.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef core_esm_hpp
#define core_esm_hpp

#include "../../core/system/entity-system-manager.hpp"

namespace kege{

    class CoreESM : public CoreSystem< kege::EntitySystemManager >
    {
    public:

        CoreESM( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}

#endif /* core_esm_hpp */
