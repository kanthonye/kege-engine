//
//  resource.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#include "asset-manager.hpp"

namespace kege
{
    uint32_t AssetTable::_type_enumerator = 0;
}

namespace kege{

    void AssetManager::shutdown()
    {
        for ( int i=0; i<_assets.size(); ++i)
        {
            if ( _assets[i] != nullptr )
            {
                delete _assets[i];
                _assets[i] = nullptr;
            }
        }
        _assets.clear();
    }

    AssetManager::~AssetManager()
    {
        shutdown();
    }
    
    AssetManager::AssetManager()
    {}
}
