//
//  ui-asset-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#ifndef ui_asset_manager_hpp
#define ui_asset_manager_hpp

#include "../ui-panel.hpp"
#include "modules/asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerUI : public ui::Panel
    {
    public:

        void handle(const void* sender, const std::string& type, void* data);

        enum class ImportStatus { PENDING, PROCESSING, COMPLETE, FAILED };
        std::vector<std::string> _string_categories;
        std::vector<kege::ui::Text> _categories;

        // Folder/category structure
        struct FolderNode
        {
            std::string name;
            std::string full_path;
            std::vector<std::shared_ptr<FolderNode>> children;
            FolderNode* parent = nullptr;

            // Statistics
            size_t asset_count = 0;
            size_t subfolder_count = 0;
        };

        // Import system
        struct ImportJob
        {
            fs::path source_path;
            std::string target_category;
            ImportStatus status;
            std::string error_message;
            float progress = 0.0f;
        };

        // Asset type registry
        struct AssetTypeInfo
        {
            AssetType type;
            std::string name;
            std::vector<std::string> extensions;
            uint32_t icon_color;
            // TODO: Add icon texture/geometry
        };

    public:

        void addTagsToAssets(const std::vector<uint64_t>& asset_handles, const std::set<std::string>& tags);

        // Public API for importing assets
        bool importAsset(const fs::path& file_path, const std::string& target_category = "");

        // Public API for batch operations
        void deleteAssets(const std::vector<uint64_t>& asset_handles);
        void addAsset(const AssetMetadata& amd);

        void handle(const kege::ui::AssetMetadataDropOff& event)override;

        void updateLayout( int16_t layer )override;

        AssetManagerUI(kege::ui::DockManager* dm);

        ~AssetManagerUI() override;

    private:

        //void setupAssetTypeRegistry();

//        void registerAssetType(AssetType type, const std::string& name, const std::vector<std::string>& extensions);

//        void refreshAssetList();
//
//        void rebuildFolderStructure();

        FolderNode* ensureFolderExists(const std::string& path);

//        void updateFilteredAssets();

//        void setupFileWatcher();

        void processFileChanges();

//        void processImportQueue();
//
//        void importTexture(ImportJob& job);
//
//        void importModel(ImportJob& job);
//
//        void importShader(ImportJob& job);
//
//        void importSound(ImportJob& job);


        //void updateToolbar();

        //void updateFolderTree();

        //void updateAssetView();

        //void updateGridView();

        //void updateListView();

        //void updateStatusBar();

        //void updateContextMenu();

        //void updateDragAndDrop();

        void handleAssetClick(size_t asset_index, uint32_t modifiers);

        //void startAssetDrag(size_t initial_index);

        // TODO: Implement methods for:
        // - Asset property editing dialog
        // - Batch operations UI
        // - Import settings editor
        // - Tag management
        // - Plugin integration hooks

    private:

        std::vector< Ref<AssetManagerModule> > _modules;
        Communication _communication;

        // Asset storage
        std::vector<AssetMetadata> _assets;
        std::vector<size_t> _filtered_indices; // Indices of assets after filtering
        std::vector<size_t> _selected_indices; // Currently selected assets
        std::mutex _asset_mutex;

        // View state
//        ViewMode _view_mode;

        // Filter state
        std::string _search_text;
        std::set<AssetType> _type_filters;
        std::string _current_category = "All";
        bool _show_favorites_only = false;

        // Folder/category structure
        std::shared_ptr<FolderNode> _root_folder;
        std::unordered_map<std::string, FolderNode*> _folder_map;
//
//        // Import system
//        std::vector<ImportJob> _import_queue;
//        std::mutex _import_mutex;
//
        // File watcher for auto-reimport
        std::thread _file_watcher_thread;
        std::atomic<bool> _file_watcher_active{true};
        std::unordered_map<std::string, fs::file_time_type> _file_timestamps;
        std::vector<fs::path> _changed_files;
        std::mutex _file_change_mutex;
//
//        // Asset type registry
//        std::unordered_map<std::string, AssetTypeInfo> _asset_type_registry;

        // Drag and drop state
//        bool _is_dragging_assets = false;
//        std::vector<uint64_t> _dragged_asset_handles;
//        kege::dvec2 _drag_start;

        // UI state
//        ui::WidgetHandle _asset_grid_uid;
//        ui::WidgetHandle _folder_tree_uid;
//        ui::WidgetHandle _import_button_uid;
//        ui::WidgetHandle _refresh_button_uid;
//        ui::WidgetHandle _view_toggle_uid;
//
//        ui::WidgetHandle _favorites_uid;
        // Performance
        static constexpr size_t ASSETS_PER_PAGE = 50; // For virtual scrolling
        uint32_t _frame_counter = 0;

        int32_t _category_index;
        bool _show_generate_window;

        friend AssetManagerModule;
    };

}
#endif /* ui_asset_manager_hpp */
