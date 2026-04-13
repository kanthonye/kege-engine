//
//  ui-asset-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#include "modules/asset-manager-importer.hpp"
#include "modules/asset-manager-folder-tree.hpp"
#include "modules/asset-manager-asset-view.hpp"
#include "modules/asset-manager-tool-bar.hpp"
#include "modules/asset-manager-create-asset.hpp"
#include "modules/create-asset-mesh-ui.hpp"
#include "modules/ui-create-material.hpp"
#include "modules/ui-create-pipeline.hpp"
#include "ui-asset-manager.hpp"





namespace kege::ui{

    class AssetManagerContextMenu : public AssetManagerModule
    {
    public:

        AssetManagerContextMenu(AssetManagerUI* m,kege::UI* g): AssetManagerModule(m,g){}
        void operator()(const std::string& type, void* data);
        void update();

    private:
    };
    void AssetManagerContextMenu::operator()(const std::string& type, void* data)
    {}
    void AssetManagerContextMenu::update()
    {}

}



namespace kege::ui{

    class AssetManagerStatusBar : public AssetManagerModule
    {
    public:

        AssetManagerStatusBar(AssetManagerUI* m,kege::UI* g): AssetManagerModule(m,g){}
        void operator()(const std::string& type, void* data);
        void update();

    private:
    };
    void AssetManagerStatusBar::operator()(const std::string& type, void* data)
    {}
    void AssetManagerStatusBar::update()
    {
    }
}


namespace kege::ui{

    AssetManagerUI::AssetManagerUI(kege::ui::DockManager* dm)
    :   ui::Panel("AssetManager", dm)
    ,   _string_categories({{"All"}, {"Textures"}, {"Models"}, {"Material"}, {"Shaders"}, {"Sounds"}, {"Scripts"}})
    ,   _category_index(0)
    ,   _show_generate_window(false)
    {
        _categories.resize(_string_categories.size());
        for (int i=0; i<_string_categories.size(); ++i)
        {
            _categories[i] = ui::Text
            {
                .width = 100,
                .color = 0xFFFFFFFF,
                .ptr = _string_categories[i].c_str(),
                .font_size = 20,
                .align = AlignText::Left,
            };
        }

        _modules.push_back(new AssetManagerToolBar(this, _ui));
        _modules.push_back(new AssetManagerFolderTree(this, _ui));
        _modules.push_back(new AssetManagerAssetView(this, _ui));
        _modules.push_back(new AssetManagerImport(this, _ui));
        _modules.push_back(new AssetManagerContextMenu(this, _ui));
        _modules.push_back(new AssetManagerCreateAsset(this, _ui));

        _modules.push_back(new CreateAssetMeshUI(this, _ui));
        _modules.push_back(new kege::ui::CreateMaterial(this, _ui));
        _modules.push_back(new kege::ui::CreatePipeline(this, _ui));
    }

    AssetManagerUI::~AssetManagerUI()
    {
        _modules.clear();
    }

    void AssetManagerUI::handle(const void* sender, const std::string &type, void *data)
    {
        for (auto& modul : _modules)
        {
            if(sender == modul.ref()) continue;
            modul->operator()(type, data);
        }
    }

    void AssetManagerUI::updateLayout()
    {
//        // Process any pending file changes
//        processFileChanges();
//
//        // Process any pending imports
//        processImportQueue();

        // Process any pending imports

        for (auto& modul : _modules)
        {
            modul->update();
        }
        _frame_counter++;
    }

    // Public API for importing assets
    bool AssetManagerUI::importAsset(const fs::path& file_path, const std::string& target_category)
    {
        // Add to import queue
//        ImportJob job;
//        job.source_path = file_path;
//        job.target_category = target_category;
//        job.status = ImportStatus::PENDING;
//
//        std::lock_guard<std::mutex> lock(_import_mutex);
//        _import_queue.push_back(job);

        return true;
    }

    // Public API for batch operations
    void AssetManagerUI::deleteAssets(const std::vector<uint64_t>& asset_handles)
    {
        std::lock_guard<std::mutex> lock(_asset_mutex);
        for (uint64_t handle : asset_handles) {
            auto it = std::find_if(_assets.begin(), _assets.end(),
                [handle](const AssetMetadata& asset) { return asset.handle == handle; });

            if (it != _assets.end()) {
                // Remove from AssetManager
                // TODO: Need type information to call correct erase method
                // For now, mark for deletion
                it->missing_source = true;

                // Remove from our list
                _assets.erase(it);
            }
        }

        //updateFilteredAssets();
    }

    void AssetManagerUI::addTagsToAssets(const std::vector<uint64_t>& asset_handles, const std::set<std::string>& tags)
    {
        std::lock_guard<std::mutex> lock(_asset_mutex);
        for (auto& asset : _assets) {
            if (std::find(asset_handles.begin(), asset_handles.end(), asset.handle) != asset_handles.end()) {
                asset.tags.insert(tags.begin(), tags.end());
            }
        }
    }

    void AssetManagerUI::addAsset(const AssetMetadata& amd)
    {
        std::lock_guard<std::mutex> lock(_asset_mutex);
        _filtered_indices.push_back(_assets.size());
        _assets.push_back(amd);
    }

    void AssetManagerUI::handle(const kege::ui::AssetMetadataDropOff& event)
    {
        std::cout << event.handle[0]->name << " DROPOFF\n";
        return;
    }
}
