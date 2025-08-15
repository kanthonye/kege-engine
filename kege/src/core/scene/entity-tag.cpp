//
//  entity-tag.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#include "entity-tag.hpp"

namespace kege{

    Entity getEntityChild( Entity entity, const kege::string& name )
    {
        /*
         if this entity is not a parent it has no child.
         */
        if ( !entity.isParent() )
        {
            return {};
        }

        /*
         if this entity is a parent, find child. only entities with a comp::Tag can be retrieved.
         */
        EntityTag* tag;
        for (Entity child = entity.begin(); child != 0 ; child = child.next() )
        {
            tag = child.get< EntityTag >();
            if ( tag )
            {
                if ( *tag == name )
                {
                    return child;
                }
            }
        }

        /*
         if the entity is not in parent entity child, search child entities.
         */
        for (Entity child = entity.begin(); child != 0 ; child = child.next() )
        {
            entity = getEntityChild( child, name );
            if ( entity )
            {
                return entity;
            }
        }

        /* entity not found.
         */
        return {};
    }

}
