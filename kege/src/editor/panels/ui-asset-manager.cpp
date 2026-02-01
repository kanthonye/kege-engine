//
//  ui-asset-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#include "ui-asset-manager.hpp"

namespace kege::ui{

    AssetManagerUI::AssetManagerUI(kege::ProjectManager *pm, kege::GUI *gui, kege::ECS *ecs)
    :   ui::Panel("AssetManager", pm, gui, ecs)
    ,   _string_categories({{"All"}, {"Textures"}, {"Models"}, {"Shaders"}, {"Sounds"}, {"Scripts"}})
    ,   _category_index(0)
    {
        _categories.resize(_string_categories.size());
        for (int i=0; i<_string_categories.size(); ++i)
        {
            _categories[i].text = ui::Text
            {
                .width = 100,
                .color = 0xFFFFFFFF,
                .ptr = _string_categories[i].c_str(),
                .font_size = 20,
                .align = AlignText::Left,
            };
        }
        // Generate persistent UIDs
//        _main_container_uid = _gui->layout()->generateUID();
//        _asset_grid_uid = _gui->layout()->generateUID();
//        _folder_tree_uid = _gui->layout()->generateUID();
//        _import_button_uid = _gui->layout()->generateUID();
//        _refresh_button_uid = _gui->layout()->generateUID();
//        _view_toggle_uid = _gui->layout()->generateUID();

        // Initialize view mode from saved preference
        _view_mode = ViewMode::GRID; // Default to grid

        // Setup asset type registry
        setupAssetTypeRegistry();

        // Load existing assets from AssetManager
        refreshAssetList();

        // Setup file watcher for auto-reimport
        setupFileWatcher();
    }

    AssetManagerUI::~AssetManagerUI()
    {
        // Cleanup
        _file_watcher_active = false;
        if (_file_watcher_thread.joinable())
        {
            _file_watcher_thread.join();
        }
    }

    void AssetManagerUI::update()
    {
        // Process any pending file changes
        processFileChanges();

        // Process any pending imports
        processImportQueue();

        // Update UI
        updateToolbar();
        updateFolderTree();
//        updateAssetView();
//        updateStatusBar();
//        updateContextMenu();
//
//        // Handle drag and drop
//        updateDragAndDrop();

        _frame_counter++;
    }

    // Public API for importing assets
    bool AssetManagerUI::importAsset(const fs::path& file_path, const std::string& target_category)
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

        updateFilteredAssets();
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

    void AssetManagerUI::setupAssetTypeRegistry() {
        // Register known asset types
        registerAssetType(AssetType::TEXTURE, "Texture", {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr"});
        registerAssetType(AssetType::MODEL, "Model", {".gltf", ".glb", ".fbx", ".obj", ".blend"});
        registerAssetType(AssetType::SHADER, "Shader", {".vert", ".frag", ".comp", ".glsl", ".hlsl"});
        registerAssetType(AssetType::SOUND, "Sound", {".wav", ".mp3", ".ogg", ".flac"});
        registerAssetType(AssetType::SCRIPT, "Script", {".lua", ".py", ".js"});
        registerAssetType(AssetType::MATERIAL, "Material", {".mat", ".material"});
        registerAssetType(AssetType::FONT, "Font", {".ttf", ".otf", ".woff", ".woff2"});
    }

    void AssetManagerUI::registerAssetType(AssetType type, const std::string& name, const std::vector<std::string>& extensions) {
        AssetTypeInfo info;
        info.type = type;
        info.name = name;
        info.extensions = extensions;
        info.icon_color = 0xFFFFFFFF; // Default white

        for (const auto& ext : extensions) {
            _asset_type_registry[ext] = info;
        }
    }

    void AssetManagerUI::refreshAssetList() {
        std::lock_guard<std::mutex> lock(_asset_mutex);
        _assets.clear();

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

    void AssetManagerUI::rebuildFolderStructure()
    {
        _root_folder = std::make_shared<FolderNode>();
        _root_folder->name = "Assets";
        _root_folder->full_path = "/";
        _folder_map["/"] = _root_folder.get();

        // Organize assets into categories/folders
        for (const auto& asset : _assets) {
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

    AssetManagerUI::FolderNode* AssetManagerUI::ensureFolderExists(const std::string& path)
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

    void AssetManagerUI::updateFilteredAssets()
    {
        _filtered_indices.clear();

        for (size_t i = 0; i < _assets.size(); i++) {
            const auto& asset = _assets[i];

            // Apply category filter
            if (_current_category != "All" && asset.category != _current_category) {
                continue;
            }

            // Apply type filter
            if (!_type_filters.empty() && !_type_filters.count(asset.type)) {
                continue;
            }

            // Apply favorites filter
            if (_show_favorites_only && !asset.is_favorite) {
                continue;
            }

            // Apply search filter
            if (!_search_text.empty()) {
                std::string name_lower = asset.name;
                std::string search_lower = _search_text;
                std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);

                if (name_lower.find(search_lower) == std::string::npos) {
                    // Also search in tags
                    bool tag_match = false;
                    for (const auto& tag : asset.tags) {
                        std::string tag_lower = tag;
                        std::transform(tag_lower.begin(), tag_lower.end(), tag_lower.begin(), ::tolower);
                        if (tag_lower.find(search_lower) != std::string::npos) {
                            tag_match = true;
                            break;
                        }
                    }

                    if (!tag_match) {
                        continue;
                    }
                }
            }

            _filtered_indices.push_back(i);
        }
    }

    void AssetManagerUI::setupFileWatcher()
    {
        _file_watcher_thread = std::thread([this]() {
            while (_file_watcher_active) {
                std::this_thread::sleep_for(std::chrono::seconds(1));

                std::lock_guard<std::mutex> lock(_asset_mutex);
                for (auto& asset : _assets) {
                    if (asset.original_path.empty()) continue;

                    try {
                        auto current_time = fs::last_write_time(asset.original_path);
                        auto last_time = _file_timestamps[asset.original_path];

                        if (current_time != last_time) {
                            _file_timestamps[asset.original_path] = current_time;

                            std::lock_guard<std::mutex> change_lock(_file_change_mutex);
                            _changed_files.push_back(asset.original_path);
                        }
                    } catch (...) {
                        // File may not exist anymore
                        if (!asset.missing_source) {
                            asset.missing_source = true;
                            // TODO: Mark asset as needing update
                        }
                    }
                }
            }
        });
    }

    void AssetManagerUI::processFileChanges()
    {
        std::vector<fs::path> changed_files;
        {
            std::lock_guard<std::mutex> lock(_file_change_mutex);
            changed_files.swap(_changed_files);
        }

        for (const auto& file_path : changed_files)
        {
            // Find asset by original path
            auto it = std::find_if(_assets.begin(), _assets.end(), [&file_path](const AssetMetadata& asset) {
                return asset.original_path == file_path.string();
            });

            if (it != _assets.end())
            {
                // TODO: Show notification to user
                // Auto-reimport or mark for reimport
                it->last_modified = fs::last_write_time(file_path);

                // For now, auto-reimport
                importAsset(file_path, it->category);
            }
        }
    }

    void AssetManagerUI::processImportQueue()
    {
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

                    if (_asset_type_registry.count(ext)) {
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

                        // Refresh asset list
                        refreshAssetList();
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

    void AssetManagerUI::importTexture(ImportJob& job)
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

        std::lock_guard<std::mutex> lock(_asset_mutex);
        _assets.push_back(metadata);
    }

    void AssetManagerUI::importModel(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement model import
    }

    void AssetManagerUI::importShader(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement shader import
    }

    void AssetManagerUI::importSound(ImportJob& job)
    {
        // Similar to importTexture
        // TODO: Implement sound import
    }

    void AssetManagerUI::updateToolbar()
    {
        _gui->beginRow(0);

        // Import button
        ui::Text import_text;// = _gui->layout()->text("", 20);
        import_text.width = 100;
        import_text.ptr = "Import";
        import_text.font_size = 20;
        import_text.color = 0xFFFFFFFF;
        import_text.align = AlignText::Left;
        if (_gui->button(0, _import_button_uid, import_text, &_gui->_theme.button))
        {
            // TODO: Open file dialog for import
            // This would trigger importAsset() for selected files
        }

        // Refresh button
        ui::Text refresh_text;
        refresh_text.width = 100;
        refresh_text.color = 0xFFFFFFFF;
        refresh_text.ptr = "Refresh";
        refresh_text.font_size = 20;
        refresh_text.align = AlignText::Left;
        if (_gui->button(0, _refresh_button_uid, refresh_text, &_gui->_theme.button)) {
            refreshAssetList();
        }

        // View mode toggle
        ui::Text view_text;
        view_text.width = 100;
        view_text.color = 0xFFFFFFFF;
        view_text.ptr = (_view_mode == ViewMode::GRID) ? "Grid" : "List";
        view_text.font_size = 20;
        view_text.align = AlignText::Left;
        if (_gui->button(0, _view_toggle_uid, view_text, &_gui->_theme.button))
        {
            _view_mode = (_view_mode == ViewMode::GRID) ? ViewMode::LIST : ViewMode::GRID;
        }

        // Search field
//        char search_buffer[256] = "";
//        strncpy(search_buffer, _search_text.c_str(), sizeof(search_buffer));
//        size_t search_size = _search_text.size() + 1;
//        uint64_t search_id = _gui->getAddressAsInt(&_search_text) + _frame_counter;
//        if (_gui->textField(0, search_id, search_buffer, search_size))
//        {
//            _search_text = search_buffer;
//            updateFilteredAssets();
//        }

        // Type filter dropdown
        // TODO: Implement multi-select dropdown for asset types

        // Favorites filter
        //bool favorites_checked = _show_favorites_only;
        //if (_gui->radio(0, _favorites_uid, favorites_checked)) {
        //    _show_favorites_only = favorites_checked;
        //    updateFilteredAssets();
        //}
        //_gui->label(0, _gui->layout()->text("Favorites", 12));

        _gui->options(0, _gui->getAddressAsInt(this), _categories, _category_index);
        _current_category = _string_categories[_category_index];
        
        _gui->endRow();
    }

    void AssetManagerUI::updateFolderTree()
    {
        // Create a split view with folder tree on left
        _gui->beginRow(0);

        // Folder tree panel (20% width)
        //_gui->beginColumn(0);
        //_gui->putSpacer(0);

        // TODO: Implement collapsible tree view for folders
        // For now, simple category list

        updateFilteredAssets();
//        for (auto& category : _categories)
//        {
//            ui::Text category_text;// = _gui->layout()->text(category.first.c_str(), 16);
//            category_text.width = 100;
//            category_text.color = 0xFFFFFFFF;
//            category_text.ptr = category.first.c_str();
//            category_text.font_size = 20;
//            category_text.align = AlignText::Left;
//
//            ui::WidgetDesc category_desc;
//            category_desc.uid = &category.second;
//            category_desc.text = category_text;
//            category_desc.style = (_current_category == category.first) ? &_gui->_theme.selected_item : &_gui->_theme.button;
//            category_desc.single_click = ui::ClickTrigger::OnRelease;
//
//            _gui->put(category_desc);
//
//            if (_gui->click(category.second))
//            {
//                _current_category = category.first;
//                updateFilteredAssets();
//            }
//        }

        //_gui->endColumn(0);

        // Main asset view area (80% width)
        //_gui->beginColumn(0);
        //_gui->endColumn(0);

        _gui->endRow();
    }

    void AssetManagerUI::updateAssetView()
    {
        if (_view_mode == ViewMode::GRID) {
            updateGridView();
        } else {
            updateListView();
        }
    }

    void AssetManagerUI::updateGridView()
    {
        // Begin scroll container for virtual scrolling
        _gui->beginScrollContainer(0, _gui->getAddressAsInt(this));

        // Calculate grid layout
        float available_width = _gui->layout()->getWidth();
        const float ITEM_WIDTH = 100.0f;
        const float ITEM_HEIGHT = 120.0f;
        const float ITEM_SPACING = 10.0f;

        int columns = std::max(1, static_cast<int>(available_width / (ITEM_WIDTH + ITEM_SPACING)));

        _gui->beginRow(0);
        //_gui->putSpacer(0);

        int column_count = 0;

        // Virtual scrolling: only render visible items
        // TODO: Calculate visible range based on scroll position

        std::lock_guard<std::mutex> lock(_asset_mutex);
        for (size_t visible_idx : _filtered_indices)
        {
            if (visible_idx >= _assets.size()) continue;

            auto& asset = _assets[visible_idx];

            // Begin column for asset item
            _gui->beginColumn(0);

            // Thumbnail/icon
            ui::WidgetDesc thumbnail_desc;
            thumbnail_desc.uid = &asset.thumbnail_uid;
            thumbnail_desc.rect = {0, 0, ITEM_WIDTH, ITEM_WIDTH};
            thumbnail_desc.color = asset.type_color();

            // Highlight if selected
            bool is_selected = std::find(_selected_indices.begin(), _selected_indices.end(), visible_idx) != _selected_indices.end();
            if (is_selected) {
                thumbnail_desc.style = &_gui->_theme.selected_item;
            } else {
                thumbnail_desc.style = &_gui->_theme.panel;
            }

            thumbnail_desc.single_click = ui::ClickTrigger::OnRelease;
            thumbnail_desc.double_click = ui::ClickTrigger::Immediate;

            _gui->put(thumbnail_desc);

            // Handle interactions
            if (_gui->click(asset.thumbnail_uid))
            {
                handleAssetClick(visible_idx, 0/*_gui->layout()->input()->modifierKeys()*/);
            }

            if (_gui->layout()->doubleClick(asset.thumbnail_uid))
            {
                // TODO: Open asset in appropriate editor
            }

            // Check for drag start
            if (_gui->pointerDragging() && _gui->mouseover(asset.thumbnail_uid))
            {
                if (!_is_dragging_assets)
                {
                    startAssetDrag(visible_idx);
                }
            }

            // Asset name
            std::string display_name = asset.display_name.empty() ? asset.name : asset.display_name;
            if (display_name.length() > 15)
            {
                display_name = display_name.substr(0, 12) + "...";
            }

            ui::Text name_text = _gui->layout()->text(display_name.c_str(), 12);
            _gui->label(0, name_text);

            // Asset type badge
            //ui::UID type_uid = _gui->layout()->generateUID();
            ui::Text type_text = _gui->layout()->text(asset.type_string().substr(0, 3).c_str(), 10);

            ui::WidgetDesc type_desc;
            //type_desc.uid = &type_uid;
            type_desc.text = type_text;
            type_desc.color = asset.type_color();
            type_desc.style = &_gui->_theme.label;

            _gui->put(type_desc);

            _gui->endColumn();

            // Move to next column or next row
            column_count++;
            if (column_count >= columns)
            {
                column_count = 0;
                _gui->endRow();
                _gui->beginRow(0);
                //_gui->putSpacer(0);
            }
        }

        _gui->endRow();
        _gui->endScrollContainer();
    }

    void AssetManagerUI::updateListView()
    {
        // TODO: Implement list view with columns
        // Similar to grid view but with different layout
    }

    void AssetManagerUI::updateStatusBar()
    {
        _gui->beginRow(0);

        // Selected count
        std::string selected_text = std::to_string(_selected_indices.size()) + " selected";
        _gui->label(0, _gui->layout()->text(selected_text.c_str(), 12));

        // Total count
        std::string total_text = std::to_string(_filtered_indices.size()) + " assets";
        _gui->label(0, _gui->layout()->text(total_text.c_str(), 12));

        // Import queue status
        if (!_import_queue.empty())
        {
            size_t pending = std::count_if(_import_queue.begin(), _import_queue.end(),
                [](const ImportJob& job) { return job.status == ImportStatus::PENDING; });
            size_t processing = std::count_if(_import_queue.begin(), _import_queue.end(),
                [](const ImportJob& job) { return job.status == ImportStatus::PROCESSING; });

            if (pending > 0 || processing > 0) {
                std::string import_text = "Importing: " + std::to_string(processing) +
                                         " / " + std::to_string(_import_queue.size());
                _gui->label(0, _gui->layout()->text(import_text.c_str(), 12));
            }
        }

        _gui->endRow();
    }

    void AssetManagerUI::updateContextMenu()
    {
        // TODO: Implement context menu for right-click on assets
        // Would show options like: Rename, Delete, Reimport, Edit Properties, Add to Favorites, etc.
    }

    void AssetManagerUI::updateDragAndDrop()
    {
        if (!_is_dragging_assets) return;

        // Draw drag preview
        for (uint64_t handle : _dragged_asset_handles)
        {
            // Find asset
            auto it = std::find_if(_assets.begin(), _assets.end(),
                [handle](const AssetMetadata& asset) { return asset.handle == handle; });

            if (it != _assets.end())
            {
//                    // Draw drag preview at pointer position
//                    ui::UID drag_uid = _gui->layout()->generateUID();
//
//                    ui::WidgetDesc drag_desc;
//                    drag_desc.uid = &drag_uid;
//                    drag_desc.rect = {
//                        _gui->pointer().x - 40,
//                        _gui->pointer().y - 40,
//                        80, 80
//                    };
//                    drag_desc.color = it->type_color();
//                    drag_desc.layer = 10; // Top layer
//                    drag_desc.color = 0x80FFFFFF; // Semi-transparent
//
//                    _gui->put(drag_desc);
            }
        }

        // Check for drop
        if (!_gui->buttonDown()) {
            _is_dragging_assets = false;

            // TODO: Check drop target
            // If dropped on SceneEditor, create object with asset
            // If dropped on folder, move asset to that folder
        }
    }

    void AssetManagerUI::handleAssetClick(size_t asset_index, uint32_t modifiers)
    {
        // Define modifier constants
        const uint32_t MODIFIER_CTRL = 0x01;
        const uint32_t MODIFIER_SHIFT = 0x02;

        if (modifiers & MODIFIER_CTRL) {
            // Toggle selection
            auto it = std::find(_selected_indices.begin(), _selected_indices.end(), asset_index);
            if (it != _selected_indices.end()) {
                _selected_indices.erase(it);
            } else {
                _selected_indices.push_back(asset_index);
            }
        } else if (modifiers & MODIFIER_SHIFT && !_selected_indices.empty()) {
            // Range selection
            size_t start = _selected_indices.back();
            size_t end = asset_index;
            if (start > end) std::swap(start, end);

            for (size_t i = start; i <= end; i++) {
                if (std::find(_selected_indices.begin(), _selected_indices.end(), i) == _selected_indices.end()) {
                    _selected_indices.push_back(i);
                }
            }
        } else {
            // Single selection
            _selected_indices.clear();
            _selected_indices.push_back(asset_index);
        }
    }

    void AssetManagerUI::startAssetDrag(size_t initial_index)
    {
        _is_dragging_assets = true;
        _drag_start = _gui->pointer();
        _dragged_asset_handles.clear();

        // If initial asset is selected, drag all selected assets
        if (std::find(_selected_indices.begin(), _selected_indices.end(), initial_index) != _selected_indices.end()) {
            for (size_t idx : _selected_indices) {
                if (idx < _assets.size()) {
                    _dragged_asset_handles.push_back(_assets[idx].handle);
                }
            }
        } else {
            if (initial_index < _assets.size()) {
                _dragged_asset_handles.push_back(_assets[initial_index].handle);
            }
        }
    }
} // namespace kege
