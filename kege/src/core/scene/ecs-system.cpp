//
//  ecs-system.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/28/25.
//

#include "ecs.hpp"
#include "ecs-system.hpp"

namespace kege::ecs{

    kege::Scene* System::getScene()
    {
        return _ecs->getScene();
    }
    
    void System::onSceneChanged()
    {
    }

    bool System::initialize()
    {
        return true;
    }

    void System::shutdown()
    {
    }

}
