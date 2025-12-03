//
//  update-decay-over-time.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#include "update-decay-over-time.hpp"

namespace kege{

    void UpdateDecayOverTime::update( double dms )
    {
        std::vector< ecs::Entity > dead_entities;
        for ( auto [entity, decay] : view< DecayOverTime >() )
        {
            decay->lifespand -= dms;
            if ( decay->lifespand <= 0 )
            {
                dead_entities.push_back( entity );
            }
        }
        for ( ecs::Entity& entity : dead_entities )
        {
            getScene()->remove( entity );
            _ecs->destroy( entity );
        }
    }
//    bool UpdateDecayOverTime::initialize()
//    {
//        _signature = createEntitySignature< DecayOverTime >();
//        return EntitySystem::initialize();
//    }

    UpdateDecayOverTime::UpdateDecayOverTime( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "update-decay-overtime", REQUIRE_UPDATE )
    {}
}
