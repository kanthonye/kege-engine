//
//  resource.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#include "asset-manager.hpp"

namespace kege{

    uint32_t ResourceManager::_enumerator = 0;

}

namespace kege{

    void AssetManager::initalize()
    {
        _managers.resize( ResourceManager::getMaxTypeCount() );
    }

    void AssetManager::shutdown()
    {
        for ( int i=0; i<_managers.size(); ++i)
        {
            if ( _managers[i] != nullptr )
            {
                delete _managers[i];
                _managers[i] = nullptr;
            }
        }
        _managers.clear();
    }
    AssetManager::~AssetManager()
    {
        shutdown();
    }
    AssetManager::AssetManager()
    {}
}
