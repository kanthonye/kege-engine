//
//  asset-manager-importer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-importer.hpp"

namespace kege::ui{
    
    AssetManagerImport::AssetManagerImport(AssetManagerUI* m,kege::GUI* g)
    :   AssetManagerModule(m,g)
    {
        setupAssetTypeRegistry();
        setupFileWatcher();
    }

    void AssetManagerImport::operator()(const std::string& type, void* data)
    {}
    
    bool AssetManagerImport::importAsset(const fs::path& file_path, const std::string& target_category)
    {
        // Add to import queue
        ImportJob job;
        job.source_path = file_path;
        job.target_category = target_category;
        job.status = ImportStatus::PENDING;

        std::lock_guard<std::mutex> lock(_import_mutex);
        _import_queue.push_back(job);

        return true;
    }
    void AssetManagerImport::setupAssetTypeRegistry()
    {
        // Register known asset types
        registerAssetType(AssetType::TEXTURE,   "Texture",  {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr"});
        registerAssetType(AssetType::MODEL,     "Model",    {".gltf", ".glb", ".fbx", ".obj", ".blend"});
        registerAssetType(AssetType::SHADER,    "Shader",   {".vert", ".frag", ".comp", ".glsl", ".hlsl"});
        registerAssetType(AssetType::SOUND,     "Sound",    {".wav", ".mp3", ".ogg", ".flac"});
        registerAssetType(AssetType::SCRIPT,    "Script",   {".lua", ".py", ".js"});
        registerAssetType(AssetType::MATERIAL,  "Material", {".mat", ".material"});
        registerAssetType(AssetType::FONT,      "Font",     {".ttf", ".otf", ".woff", ".woff2"});
    }
    void AssetManagerImport::registerAssetType(AssetType type, const std::string& name, const std::vector<std::string>& extensions)
    {
        AssetTypeInfo info;
        info.type = type;
        info.name = name;
        info.extensions = extensions;
        info.icon_color = 0xFFFFFFFF; // Default white

        for (const auto& ext : extensions) {
            _asset_type_registry[ext] = info;
        }
    }
    void AssetManagerImport::update()
    {
        // Process any pending file changes
        processFileChanges();

        // Process any pending imports
        processImportQueue();
    }
    void AssetManagerImport::setupFileWatcher()
    {
        //std::vector<size_t>* selected_indices = getSelectedIndices();
        std::vector<AssetMetadata>* assets = getAssets();

        _file_watcher_thread = std::thread([this, assets]() {
            while (_file_watcher_active) {
                std::this_thread::sleep_for(std::chrono::seconds(1));

                std::lock_guard<std::mutex> lock(getAssetMutex());
                for (auto& asset : *assets)
                {
                    if (asset.original_path.empty()) continue;

                    try
                    {
                        auto current_time = fs::last_write_time(asset.original_path);
                        auto last_time = _file_timestamps[asset.original_path];

                        if (current_time != last_time)
                        {
                            _file_timestamps[asset.original_path] = current_time;

                            std::lock_guard<std::mutex> change_lock(_file_change_mutex);
                            _changed_files.push_back(asset.original_path);
                        }
                    }
                    catch (...)
                    {
                        // File may not exist anymore
                        if (!asset.missing_source)
                        {
                            asset.missing_source = true;
                            // TODO: Mark asset as needing update
                        }
                    }
                }
            }
        });
    }
    void AssetManagerImport::processFileChanges()
    {
        std::vector<fs::path> changed_files;
        {
            std::lock_guard<std::mutex> lock(_file_change_mutex);
            changed_files.swap(_changed_files);
        }

        std::vector<AssetMetadata>* assets = getAssets();
        for (const auto& file_path : changed_files)
        {
            // Find asset by original path
            auto it = std::find_if(assets->begin(), assets->end(), [&file_path](const AssetMetadata& asset) {
                return asset.original_path == file_path.string();
            });

            if (it != assets->end())
            {
                // TODO: Show notification to user
                // Auto-reimport or mark for reimport
                it->last_modified = fs::last_write_time(file_path);

                // For now, auto-reimport
                importAsset(file_path, it->category);
            }
        }
    }
    void AssetManagerImport::processImportQueue()
    {
        std::unordered_map<std::string, AssetTypeInfo> _asset_type_registry;
        std::lock_guard<std::mutex> lock(_import_mutex);

        for (auto& job : _import_queue)
        {
            if (job.status == ImportStatus::PENDING)
            {
                job.status = ImportStatus::PROCESSING;

                try
                {
                    // Determine asset type from extension
                    std::string ext = job.source_path.extension().string();
                    AssetType asset_type = AssetType::UNKNOWN;

                    if (_asset_type_registry.count(ext))
                    {
                        asset_type = _asset_type_registry[ext].type;
                    }

                    // Import based on asset type
                    switch (asset_type)
                    {
                        case AssetType::TEXTURE:
                            importTexture(job);
                            break;
                        case AssetType::MODEL:
                            importModel(job);
                            break;
                        case AssetType::SHADER:
                            importShader(job);
                            break;
                        case AssetType::SOUND:
                            importSound(job);
                            break;
                        default:
                            job.status = ImportStatus::FAILED;
                            job.error_message = "Unsupported file type: " + ext;
                            break;
                    }

                    if (job.status == ImportStatus::PROCESSING)
                    {
                        job.status = ImportStatus::COMPLETE;
                        job.progress = 1.0f;

                        _manager->handle(this, "RefreshAssetList", nullptr);
                        // Refresh asset list
                        //refreshAssetList();
                    }
                }
                catch (const std::exception& e)
                {
                    job.status = ImportStatus::FAILED;
                    job.error_message = e.what();
                }
            }
        }

        // Remove completed jobs
        _import_queue.erase(
            std::remove_if(_import_queue.begin(), _import_queue.end(),
                [](const ImportJob& job) {
                    return job.status == ImportStatus::COMPLETE ||
                           job.status == ImportStatus::FAILED;
                }),
            _import_queue.end());
    }
    void AssetManagerImport::importTexture(ImportJob& job)
    {
        // TODO: Use AssetManager to load texture
        // uint64_t handle = _asset_manager->load<Texture>(job.source_path.string());

        // Create metadata
        AssetMetadata metadata;
        metadata.name = job.source_path.stem().string();
        metadata.display_name = metadata.name;
        metadata.original_path = job.source_path.string();
        metadata.type = AssetType::TEXTURE;
        metadata.handle = 0; // TODO: Get actual handle from AssetManager
        metadata.loader_id = job.source_path.extension().string();
        metadata.import_time = std::chrono::system_clock::now();
        metadata.last_modified = fs::last_write_time(job.source_path);
        metadata.file_size = fs::file_size(job.source_path);
        metadata.category = job.target_category.empty() ? "Textures" : job.target_category;
        metadata.loaded = true;

        std::lock_guard<std::mutex> lock(getAssetMutex());
        getAssets()->push_back(metadata);
    }
    void AssetManagerImport::importModel(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement model import
    }
    void AssetManagerImport::importShader(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement shader import
    }
    void AssetManagerImport::importSound(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement sound import
    }
    AssetManagerImport::~AssetManagerImport()
    {
        // Cleanup
        _file_watcher_active = false;
        if (_file_watcher_thread.joinable())
        {
            _file_watcher_thread.join();
        }
    }
}
