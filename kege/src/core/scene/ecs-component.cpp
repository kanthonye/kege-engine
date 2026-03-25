//
//  ecs-component.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/5/26.
//

#include "ecs-component.hpp"

namespace kege::ecs{

    std::atomic<uint32_t> Component::Info::type_count = 0;
    
}
