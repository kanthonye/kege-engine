//
//  selection-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#ifndef selection_system_hpp
#define selection_system_hpp

#include "../../../math/algebra/vmath.hpp"
#include "../../../scene/ecs.hpp"
#include "../../../input/input-context-manager.hpp"
#include "../collision/rayhit/rayhit.hpp"

namespace kege{

    class EntitySelectionSystem : public kege::ecs::System
    {
    public:

        void operator()( const MappedInputs& inputs );
        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;
        
        EntitySelectionSystem( kege::ECS* ecs );
        bool _make_selection;
    };

    struct MsgEntitySelection
    {
        ecs::Entity entity;
    };

    struct MsgEntitySelectionDistance
    {
        double dist;
    };

};

#endif /* selection_system_hpp */
