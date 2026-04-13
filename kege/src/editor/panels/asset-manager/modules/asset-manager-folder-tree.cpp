//
//  asset-manager-folder-tree.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-folder-tree.hpp"

namespace kege::ui{

    void AssetManagerFolderTree::operator()(const std::string& type, void* data)
    {}
    void AssetManagerFolderTree::updateFilteredAssets()
    {
        _filtered_indices.clear();

//        for (size_t i = 0; i < getAssets()->size(); i++)
//        {
//            const auto& asset = getAssets()->at(i);
//
//            // Apply category filter
//            if (_current_category.ptr != "All" && asset.category != _current_category)
//            {
//                continue;
//            }
//
//            // Apply type filter
//            if (!_type_filters.empty() && !_type_filters.count(asset.type))
//            {
//                continue;
//            }
//
//            // Apply favorites filter
//            if (_show_favorites_only && !asset.is_favorite)
//            {
//                continue;
//            }
//
//            // Apply search filter
//            if (!_search_text.empty())
//            {
//                std::string name_lower = asset.name;
//                std::string search_lower = _search_text;
//                std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
//                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
//
//                if (name_lower.find(search_lower) == std::string::npos) {
//                    // Also search in tags
//                    bool tag_match = false;
//                    for (const auto& tag : asset.tags) {
//                        std::string tag_lower = tag;
//                        std::transform(tag_lower.begin(), tag_lower.end(), tag_lower.begin(), ::tolower);
//                        if (tag_lower.find(search_lower) != std::string::npos) {
//                            tag_match = true;
//                            break;
//                        }
//                    }
//
//                    if (!tag_match) {
//                        continue;
//                    }
//                }
//            }
//
//            _filtered_indices.push_back(i);
//        }
    }
    void AssetManagerFolderTree::refreshAssetList()
    {
        std::lock_guard<std::mutex> lock(getAssetMutex());
        getAssets()->clear();

        // TODO: Query assets from AssetManager
        // This needs to iterate through all AssetCacheTables and get their contents
        // For now, we'll create placeholder assets

        // Example: Get texture assets
        /*
        auto texture_table = _asset_manager->getAssetCacheTable<Texture>();
        if (texture_table) {
            auto textures = texture_table->getAllAssets();
            for (const auto& [name, texture] : textures) {
                AssetMetadata metadata;
                metadata.name = name;
                metadata.type = AssetType::TEXTURE;
                metadata.loaded = texture != nullptr;
                // ... set other metadata
                _assets.push_back(metadata);
            }
        }
        */

        // Update folder structure
        rebuildFolderStructure();

        // Update filtered list
        updateFilteredAssets();
    }
    void AssetManagerFolderTree::rebuildFolderStructure()
    {
        _root_folder = std::make_shared<FolderNode>();
        _root_folder->name = "Assets";
        _root_folder->full_path = "/";
        _folder_map["/"] = _root_folder.get();

        std::vector<AssetMetadata>* assets = getAssets();

        // Organize assets into categories/folders
        for (const auto& asset : *assets) {
            std::string category_path = "/" + asset.category;

            // Ensure folder exists
            FolderNode* folder = ensureFolderExists(category_path);
            folder->asset_count++;

            // Update parent folder counts
            FolderNode* parent = folder->parent;
            while (parent) {
                parent->asset_count++;
                parent = parent->parent;
            }
        }
    }
    AssetManagerFolderTree::FolderNode* AssetManagerFolderTree::ensureFolderExists(const std::string& path)
    {
        if (_folder_map.count(path)) {
            return _folder_map[path];
        }

        // Split path and create folders
        std::vector<std::string> parts;
        std::string part;
        std::istringstream stream(path);

        while (std::getline(stream, part, '/')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }

        FolderNode* current = _root_folder.get();
        std::string current_path = "/";

        for (const auto& part : parts) {
            current_path += part + "/";

            if (!_folder_map.count(current_path)) {
                auto new_folder = std::make_shared<FolderNode>();
                new_folder->name = part;
                new_folder->full_path = current_path;
                new_folder->parent = current;

                current->children.push_back(new_folder);
                _folder_map[current_path] = new_folder.get();
                current->subfolder_count++;
            }

            current = _folder_map[current_path];
        }

        return current;
    }
    void AssetManagerFolderTree::update()
    {
        // Create a split view with folder tree on left
        _ui->beginRow(0);

        // Folder tree panel (20% width)
        //_ui->beginColumn(0);
        //_ui->putSpacer(0);

        // TODO: Implement collapsible tree view for folders
        // For now, simple category list

//        updateFilteredAssets();
//        for (auto& category : _categories)
//        {
//            ui::WidgetDesc category_desc;
//            category_desc.user_id = &category.uid;
//            category_desc.text = category.text;
//            category_desc.style = (_current_category == category.text.ptr) ? &_ui->theme().selected_item : &_ui->theme().button;
//            category_desc.single_click = ui::ClickTrigger::OnRelease;
//
//            _ui->put(category_desc);
//
//            if (_ui->click(category.uid))
//            {
//                _current_category = category.uid;
//                updateFilteredAssets();
//            }
//        }

        //_ui->endColumn(0);

        // Main asset view area (80% width)
        //_ui->beginColumn(0);
        //_ui->endColumn(0);

        _ui->endRow();
    }

}


