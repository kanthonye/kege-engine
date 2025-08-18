//
//  core-esm.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#ifndef core_esm_hpp
#define core_esm_hpp

#include "module.hpp"
#include "../system/entity-system-manager.hpp"

namespace kege{

    class EntitySystemManagerModule : public Module
    {
    public:

        kege::Ref< kege::EntitySystemManager > operator ->() { return _module; }
        const kege::Ref< kege::EntitySystemManager > operator ->() const { return _module; }
        operator bool()const override{ return _module != nullptr; }

        bool initialize()override;
        void shutdown()override;
        void add()override;

        EntitySystemManagerModule( kege::Engine* engine );

    private:

        kege::Ref< kege::EntitySystemManager > _module;
        // This is the Entity System Manager that will be used to manage entity systems
        // It will be used to add, remove and update entity systems across multiple frames
        // It will also be used to handle entity creation and destruction.
        // The Entity System Manager will be used to manage entity systems efficiently and handle resources efficiently.            
    };

}

#endif /* core_esm_hpp */
