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
        std::vector< kege::Entity > dead_entities;
        for ( kege::Entity& entity : *_entities )
        {
            DecayOverTime* decay = entity.get< DecayOverTime >();
            decay->lifespand -= dms;
            if ( decay->lifespand <= 0 )
            {
                dead_entities.push_back( entity );
            }
        }
        for ( kege::Entity& entity : dead_entities )
        {
            _engine->getScene()->remove( entity );
        }
    }
    bool UpdateDecayOverTime::initialize()
    {
        _signature = createEntitySignature< DecayOverTime >();
        return EntitySystem::initialize();
    }

    UpdateDecayOverTime::UpdateDecayOverTime( kege::Engine* engine )
    :   kege::EntitySystem( engine, "update-decay-overtime", REQUIRE_UPDATE )
    {}
}
