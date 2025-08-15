//
//  dragging-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/11/25.
//

#ifndef dragging_system_hpp
#define dragging_system_hpp

#include "rigidbody.hpp"
#include "collider.hpp"
#include "entity-system.hpp"
#include "selection-system.hpp"

namespace kege{

    class EntityDraggingSystem : public kege::EntitySystem
    {
    public:

        void operator()( const MappedInputs& inputs );
        void operator()( const MsgEntitySelection& msg );
        void update( double dms )override;
        bool initialize()override;
        void shutdown()override;

        EntityDraggingSystem( kege::Engine* engine );

        kege::Entity _selected_entity;
        double _distance;
        bool _drag_entity;
    };
}
#endif /* dragging_system_hpp */
