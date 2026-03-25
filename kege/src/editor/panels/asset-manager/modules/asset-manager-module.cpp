//
//  asset-manager-module.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-module.hpp"

namespace kege::ui{

    const std::vector<AssetMetadata>* AssetManagerModule::getAssets()const
    {
        return &_manager->_assets;
    }
    std::vector<AssetMetadata>* AssetManagerModule::getAssets()
    {
        return &_manager->_assets;
    }

    const std::vector<size_t>* AssetManagerModule::getSelectedIndices()const
    {
        return &_manager->_selected_indices;
    }
    std::vector<size_t>* AssetManagerModule::getSelectedIndices()
    {
        return &_manager->_selected_indices;
    }
    const std::vector<size_t>* AssetManagerModule::getFilteredIndices()const
    {
        return &_manager->_filtered_indices;
    }
    std::vector<size_t>* AssetManagerModule::getFilteredIndices()
    {
        return &_manager->_filtered_indices;
    }
    std::mutex& AssetManagerModule::getAssetMutex()
    {
        return _manager->_asset_mutex;
    }
}

