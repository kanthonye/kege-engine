//
//  core-vfs.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef core_vfs_hpp
#define core_vfs_hpp

#include "../io/virtual-directory.hpp"
#include "../system/system.hpp"

namespace kege{

    /**
     * System use to initialize and shutdown the ECS (Entity Component System)
     */
    class CoreVFS : public CoreSystem< kege::VirtualDirectory >
    {
    public:

        CoreVFS( kege::Engine* engine );
        bool initialize();
        void shutdown();
    };

}
#endif /* core_vfs_hpp */
