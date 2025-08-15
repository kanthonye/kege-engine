//
//  entity-tag.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#ifndef entity_tag_hpp
#define entity_tag_hpp

#include <string>
#include "scene.hpp"
#include "log.hpp"
#include "kege-string.hpp"

namespace kege{

    typedef kege::string EntityTag;
    Entity getEntityChild( Entity entity, const kege::string& name );
}

#endif /* entity_tag_hpp */
