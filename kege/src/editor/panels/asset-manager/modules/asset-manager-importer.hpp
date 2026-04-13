//
//  asset-manager-importer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef asset_manager_importer_hpp
#define asset_manager_importer_hpp

#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <functional>
#include "asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerImport : public AssetManagerModule
    {
    public:

        enum class ImportStatus { PENDING, PROCESSING, COMPLETE, FAILED };

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

        bool importAsset(const fs::path& file_path, const std::string& target_category);
        void operator()(const std::string& type, void* data);
        void update();

        AssetManagerImport(AssetManagerUI* m,kege::UI* g);
        ~AssetManagerImport();

    private:

        void registerAssetType(AssetType type, const std::string& name, const std::vector<std::string>& extensions);
        void setupAssetTypeRegistry();
        void importTexture(ImportJob& job);
        void importModel(ImportJob& job);
        void importShader(ImportJob& job);
        void importSound(ImportJob& job);
        void processFileChanges();
        void processImportQueue();
        void setupFileWatcher();

    private:

        // Asset type registry
        std::unordered_map<std::string, AssetTypeInfo> _asset_type_registry;

        // Import system
        std::vector<ImportJob> _import_queue;
        std::mutex _import_mutex;

        // File watcher for auto-reimport
        std::thread _file_watcher_thread;
        std::atomic<bool> _file_watcher_active{true};
        std::unordered_map<std::string, fs::file_time_type> _file_timestamps;
        std::vector<fs::path> _changed_files;
        std::mutex _file_change_mutex;
    };
    
}
#endif /* asset_manager_importer_hpp */
