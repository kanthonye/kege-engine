//
//  asset-system.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#include "asset-system.hpp"

namespace kege{

    uint32_t ResrcMngr::_enumerator = 1;

    void AssetSystem::shutdown()
    {
        for (ResrcMngr* mgr : _managers )
        {
            delete mgr;
        }
        _managers.clear();
    }

    AssetSystem::~AssetSystem()
    {
        shutdown();
    }

    AssetSystem::AssetSystem()
    {
        _managers.resize( ResrcMngr::getMaxResMgrCount() );
    }

}
