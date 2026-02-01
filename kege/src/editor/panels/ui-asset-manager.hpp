//
//  ui-asset-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#ifndef ui_asset_manager_hpp
#define ui_asset_manager_hpp

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <functional>

#include "../../core/scene/ecs.hpp"
#include "../../core/scene/scene.hpp"
#include "../../core/app/project-manager.hpp"
#include "../../core/scene/ecs.hpp"
#include "../../core/graphics/gui/ui-layout.hpp"
#include "../../core/graphics/gui/ui-viewer.hpp"
#include "../gui.hpp"
#include "ui-panel.hpp"

namespace kege::ui{

    namespace fs = std::filesystem;
    enum class AssetType
    {
        UNKNOWN,
        TEXTURE,
        MODEL,
        SHADER,
        SOUND,
        SCRIPT,
        MATERIAL,
        FONT,
        // Add more as needed
    };

    struct AssetMetadata
    {
        std::string name;          // Asset name (stem)
        std::string display_name;  // User-friendly display name
        std::string original_path; // Full original file path
        AssetType type;
        uint64_t handle;           // AssetManager handle
        std::string loader_id;     // Loader extension/type
        std::chrono::system_clock::time_point import_time;
        fs::file_time_type last_modified;
        size_t file_size;

        // User-defined metadata
        std::set<std::string> tags;
        std::string category;
        bool is_favorite = false;

        ui::UID asset_uid;
        ui::UID thumbnail_uid;
        ui::UID name_uid;


        // Import settings (type-specific)
        struct ImportSettings
        {
            // Texture settings
            struct TextureSettings
            {
                bool generate_mipmaps = true;
                bool srgb = false;
                bool compress = true;
            }
            texture;

            // Model settings
            struct ModelSettings
            {
                float scale = 1.0f;
                bool flip_uvs = true;
                bool generate_normals = false;
            }
            model;

            // Shader settings
            struct ShaderSettings
            {
                bool optimize = true;
                bool debug_info = false;
            }
            shader;

            // Sound settings
            struct SoundSettings
            {
                bool stream = false;
                bool loop = false;
                float volume = 1.0f;
            }
            sound;
        }
        import_settings;

        // Status
        bool loaded = false;
        bool missing_source = false;
        std::string last_error;

        // Thumbnail placeholder (you'll replace with actual thumbnails)
        uint32_t placeholder_color = 0x808080FF; // Gray

        // Get color based on asset type
        uint32_t type_color() const
        {
            switch (type)
            {
                case AssetType::TEXTURE: return 0xFF6B6BFF; // Red
                case AssetType::MODEL:   return 0x4ECDC4FF; // Teal
                case AssetType::SHADER:  return 0x45B7D1FF; // Blue
                case AssetType::SOUND:   return 0x96CEB4FF; // Green
                case AssetType::SCRIPT:  return 0xFFEAA7FF; // Yellow
                case AssetType::MATERIAL:return 0xDDA0DDFF; // Purple
                case AssetType::FONT:    return 0xFFA07AFF; // Orange
                default:                 return 0x808080FF; // Gray
            }
        }

        std::string type_string() const
        {
            switch (type)
            {
                case AssetType::TEXTURE: return "Texture";
                case AssetType::MODEL:   return "Model";
                case AssetType::SHADER:  return "Shader";
                case AssetType::SOUND:   return "Sound";
                case AssetType::SCRIPT:  return "Script";
                case AssetType::MATERIAL:return "Material";
                case AssetType::FONT:    return "Font";
                default:                 return "Unknown";
            }
        }
    };

}



namespace kege::ui{

    class AssetManagerUI : public ui::Panel
    {
    public:

        enum class ImportStatus { PENDING, PROCESSING, COMPLETE, FAILED };
        std::vector<std::string> _string_categories;
        std::vector<kege::ListElem> _categories;

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

        void update() override;

        AssetManagerUI(kege::ProjectManager *pm, kege::GUI *gui, kege::ECS *ecs);

        ~AssetManagerUI() override;

    private:

        void setupAssetTypeRegistry();

        void registerAssetType(AssetType type, const std::string& name, const std::vector<std::string>& extensions);

        void refreshAssetList();

        void rebuildFolderStructure();

        FolderNode* ensureFolderExists(const std::string& path);

        void updateFilteredAssets();

        void setupFileWatcher();

        void processFileChanges();

        void processImportQueue();

        void importTexture(ImportJob& job);

        void importModel(ImportJob& job);

        void importShader(ImportJob& job);

        void importSound(ImportJob& job);

        void updateToolbar();

        void updateFolderTree();

        void updateAssetView();

        void updateGridView();

        void updateListView();

        void updateStatusBar();

        void updateContextMenu();

        void updateDragAndDrop();

        void handleAssetClick(size_t asset_index, uint32_t modifiers);

        void startAssetDrag(size_t initial_index);

        // TODO: Implement methods for:
        // - Asset property editing dialog
        // - Batch operations UI
        // - Import settings editor
        // - Tag management
        // - Plugin integration hooks

    private:

        // Asset storage
        std::vector<AssetMetadata> _assets;
        std::vector<size_t> _filtered_indices; // Indices of assets after filtering
        std::vector<size_t> _selected_indices; // Currently selected assets
        std::mutex _asset_mutex;

        // View state
        enum class ViewMode { GRID, LIST };
        ViewMode _view_mode;

        // Filter state
        std::string _search_text;
        std::set<AssetType> _type_filters;
        std::string _current_category = "All";
        bool _show_favorites_only = false;

        // Folder/category structure
        std::shared_ptr<FolderNode> _root_folder;
        std::unordered_map<std::string, FolderNode*> _folder_map;

        // Import system
        std::vector<ImportJob> _import_queue;
        std::mutex _import_mutex;

        // File watcher for auto-reimport
        std::thread _file_watcher_thread;
        std::atomic<bool> _file_watcher_active{true};
        std::unordered_map<std::string, fs::file_time_type> _file_timestamps;
        std::vector<fs::path> _changed_files;
        std::mutex _file_change_mutex;

        // Asset type registry
        std::unordered_map<std::string, AssetTypeInfo> _asset_type_registry;

        // Drag and drop state
        bool _is_dragging_assets = false;
        std::vector<uint64_t> _dragged_asset_handles;
        kege::dvec2 _drag_start;

        // UI state
        ui::UID _main_container_uid;
        ui::UID _asset_grid_uid;
        ui::UID _folder_tree_uid;
        ui::UID _import_button_uid;
        ui::UID _refresh_button_uid;
        ui::UID _view_toggle_uid;

        ui::UID _favorites_uid;
        // Performance
        static constexpr size_t ASSETS_PER_PAGE = 50; // For virtual scrolling
        uint32_t _frame_counter = 0;

        int32_t _category_index;
    };

}
#endif /* ui_asset_manager_hpp */
