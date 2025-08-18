//
//  core-vfs.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef core_vfs_hpp
#define core_vfs_hpp

#include "../io/virtual-directory.hpp"
#include "module.hpp"

namespace kege{

    /**
     * System use to initialize and shutdown the ECS (Entity Component System)
     */
    class VirtualDirectoryModule : public Module
    {
    public:

        VirtualDirectoryModule( kege::Engine* engine );

        operator bool()const override{ return _module != nullptr; }
        const kege::VirtualDirectory* operator ->()const;
        kege::VirtualDirectory* operator ->();

        bool initialize()override;
        void shutdown()override;
        void add()override;

    private:

        kege::Ref< kege::VirtualDirectory > _module;
    };

}
#endif /* core_vfs_hpp */
