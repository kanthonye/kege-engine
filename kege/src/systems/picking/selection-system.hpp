//
//  selection-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#ifndef selection_system_hpp
#define selection_system_hpp

#include "../-/system-dependencies.hpp"
#include "../physics/3d/collision/rayhit/rayhit.hpp"

namespace kege{

    class EntitySelectionSystem : public kege::EntitySystem
    {
    public:

        void operator()( const MappedInputs& inputs );
        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;
        
        EntitySelectionSystem( kege::Engine* engine );
        bool _make_selection;
    };

    struct MsgEntitySelection
    {
        kege::Entity entity;
    };

    struct MsgEntitySelectionDistance
    {
        double dist;
    };

};

#endif /* selection_system_hpp */
