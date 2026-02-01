//
//  file-browser.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/11/25.
//

#include "ui-file-browser.hpp"

namespace kege::ui{

    FileBrowser::FileBrowser(kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs, const std::string& path)
    :   kege::ui::Panel( "FileBrowser", pm, gui, ecs )
    ,   _current_path(path)
    ,   _show_tool_tip(false)
    ,   _curr_selection(0)
    {
        _path_field_id = _gui->getAddressAsInt(this);
        _scroll_container_id = _path_field_id + 4;

        // Initialize navigation history
        _history.push(_current_path);

        // Load initial directory contents
        refreshDirectory();
    }

    void FileBrowser::update()
    {
        // Main update function called every frame
        _gui->push({ .layer = 0, .uid = &_uid_main, .style = &_gui->_theme.panel, .clip_overflow = true });
        updateNavigationBar();
        updateFileGrid();
        updateSelectionBox();
        updateDragAndDrop();
        //updateContextMenu();
        updateTooltips();

        // Clear any completed operations
        //processPendingOperations();
        _gui->pop();

        for (const auto& entry : _selected_indices)
        {
            _gui->get(_filtered_files[entry].uid)->color = 0xFFFFFF20;
        }
    }

    void FileBrowser::refreshDirectory()
    {
        _files.clear();
        _selected_indices.clear();
        _hovered_file_index.reset();

        try
        {
            if (fs::exists(_current_path) && fs::is_directory(_current_path))
            {
                // Check cache first
                std::string path_str = _current_path.string();
                auto now = std::chrono::system_clock::now();
                auto cache_age = now - _last_cache_time;

                // Use cache if it's less than 5 seconds old
                if (_directory_cache.count(path_str) && cache_age < std::chrono::seconds(5))
                {
                    _files = _directory_cache[path_str];
                }
                else
                {
                    // Read directory
                    for (const auto& entry : fs::directory_iterator(_current_path))
                    {
                        try
                        {
                            FileEntry file;
                            file.path = entry.path();
                            file.name = entry.path().filename().string();
                            file.is_directory = entry.is_directory();

                            if (!_show_hidden && file.name[0] == '.')
                                continue;

                            file.display_name = file.name;
                            if (file.display_name.length() > 15)
                            {
                                file.display_name = file.display_name.substr(0, 12) + "...";
                            }
                            file.name_text = _gui->layout()->text(file.display_name.c_str(), 20);
                            file.name_text.align = ui::AlignText::Center;
                            file.name_text.color = 0xFFFFFFFF;

                            if (!file.is_directory)
                            {
                                file.size = entry.is_regular_file() ? entry.file_size() : 0;
                                file.extension = entry.path().extension().string();
                            }
                            else
                            {
                                file.size = 0;
                                file.extension = "";
                            }

                            //file.modified_time = fs::last_write_time(entry);

                            _files.push_back(file);
                        }
                        catch (...)
                        {
                            // Skip files we can't access
                            continue;
                        }
                    }

                    // Sort: directories first, then files
                    std::sort(_files.begin(), _files.end(), [](const FileEntry& a, const FileEntry& b) {
                        if (a.is_directory && !b.is_directory) return true;
                        if (!a.is_directory && b.is_directory) return false;
                        return a.name < b.name;
                    });

                    // Cache the results
                    _directory_cache[path_str] = _files;
                    _last_cache_time = now;
                }

                // Apply filter
                applyFilter();
            }
        }
        catch (const fs::filesystem_error& e)
        {
            // TODO: Show error message
            std::cerr << "Error accessing directory: " << e.what() << std::endl;
        }
    }

    void FileBrowser::applyFilter()
    {
        _filtered_files.clear();

        if (_filter_text.empty())
        {
            _filtered_files = _files;
            return;
        }

        std::string filter_lower = _filter_text;
        std::transform(filter_lower.begin(), filter_lower.end(), filter_lower.begin(), ::tolower);

        for (const auto& file : _files)
        {
            std::string name_lower = file.name;
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);

            if (name_lower.find(filter_lower) != std::string::npos)
            {
                _filtered_files.push_back(file);
            }
        }
    }

    bool navButn(kege::GUI* gui, UID& uid, const ui::Text& label)
    {
        gui->put
        ({
            .uid = &uid,
            .rect = ui::Rect{.width = 20.f, .height = 20.f, .x = 0.f, .y = 0.f },
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {10,10,10,10},
            .color = 0xFFFFFF0F,
            .text = label,
        });
        return gui->click(uid);
    }

    bool navToggle(kege::GUI* gui, UID& uid, bool state, const ui::Text& label)
    {
        gui->put
        ({
            .uid = &uid,
            .rect = ui::Rect{.width = 20.f, .height = 20.f, .x = 0.f, .y = 0.f },
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
            .border.corner_curves = {10,10,10,10},
            .color = (state) ? 0xCC7F41FF: 0xFFFFFF0F,
            .text = label,
        });
        return gui->click(uid);
    }

    void FileBrowser::updateNavigationBar()
    {
        // Start a row for navigation controls
        _gui->push({.layer = 0, .style = &_gui->_theme.navbar});

        // Back button
        ui::Text back_text;
        back_text.width = 10;
        back_text.height = 20;
        back_text.color = 0xFFFFFFFF;
        back_text.font_size = 20;
        back_text.ptr = "<";
        back_text.x = 7;
        back_text.y = 0;
        if ( navButn(_gui, _back_button_uid, back_text) )
        {
            if (_history.can_go_back())
            {
                _current_path = _history.go_back();
                refreshDirectory();
            }
        }

        // Forward button
        ui::Text forward_text;// = _gui->layout()->text(">", 20);
        forward_text.width = 10;
        forward_text.height = 20;
        forward_text.color = 0xFFFFFFFF;
        forward_text.font_size = 20;
        forward_text.ptr = ">";
        forward_text.x = 7;
        forward_text.y = 0;
        if ( navButn(_gui, _forward_button_uid, forward_text) )
        //if (_gui->button(0, _forward_button_id, forward_text, &_gui->_theme.button))
        {
            if (_history.can_go_forward())
            {
                _current_path = _history.go_forward();
                refreshDirectory();
            }
        }

        // Up button (parent directory)
        ui::Text up_text;// = _gui->layout()->text("↑", 20);
        up_text.width = 10;
        up_text.height = 20;
        up_text.color = 0xFFFFFFFF;
        up_text.font_size = 20;
        up_text.ptr = "^";
        up_text.x = 6;
        up_text.y = 2;

        //up_text.align = ui::AlignText::Center;
        if ( navButn(_gui, _up_button_uid, up_text) )
        //if (_gui->button(0, _up_button_id, up_text, &_gui->_theme.button))
        {
            if (_current_path.has_parent_path())
            {
                navigateTo(_current_path.parent_path());
            }
        }

        // Current path display/input
        _current_path_str = _current_path.string();

        ui::Text current_path_text;// = _gui->layout()->text("↑", 20);
        current_path_text.width = 10;
        current_path_text.height = 20;
        current_path_text.color = 0xFFFFFFFF;
        current_path_text.font_size = 20;
        current_path_text.ptr = _current_path_str.data();
        _gui->put({.layer = 0, .style = &_gui->_theme.broswer_file_path, .text = current_path_text});

        // Use textField for path input
        /*
         size_t size = path_str.size() + 1;
         char path_buffer[256];
         strncpy(path_buffer, path_str.c_str(), std::min(size, sizeof(path_buffer)));
        if (_gui->textField(0, _path_field_id, path_buffer, size))
        {
            fs::path new_path(path_buffer);
            if (fs::exists(new_path) && fs::is_directory(new_path))
            {
                navigateTo(new_path);
            }
        }
         */

        // Refresh button
        ui::Text refresh_text;// = _gui->layout()->text("↻", 20);
        refresh_text.width = 10;
        refresh_text.color = 0xFFFFFFFF;
        refresh_text.font_size = 20;
        refresh_text.ptr = "@";
        refresh_text.x = 7;
        refresh_text.y = 2;
        if ( navButn(_gui, _refresh_button_uid, refresh_text) )
        //if (_gui->button(0, _refresh_button_id, refresh_text, &_gui->_theme.button))
        {
            refreshDirectory();
        }

        // New folder button
        ui::Text new_folder_text;// = _gui->layout()->text("+", 20);
        new_folder_text.width = 10;
        new_folder_text.color = 0xFFFFFFFF;
        new_folder_text.font_size = 20;
        new_folder_text.ptr = "+";
        new_folder_text.x = 7;
        new_folder_text.y = 2;
        if ( navButn(_gui, _new_folder_button_uid, new_folder_text) )
        {
            createNewFolder();
        }

        // New folder button
        new_folder_text.width = 10;
        new_folder_text.color = 0xFFFFFFFF;
        new_folder_text.font_size = 20;
        new_folder_text.ptr = "T";
        new_folder_text.x = 7;
        new_folder_text.y = 2;
        if ( navToggle(_gui, _tool_tip_button_uid, _show_tool_tip, new_folder_text) )
        {
            _show_tool_tip = !_show_tool_tip;
        }

        _gui->put({.layer = 0, .rect.width = 20});


        // Filter input
//        _gui->push({.layer = 0, .style = &_gui->_theme.filter_container});
//        {
//            // Filter input label
//            ui::Text filter_text = {};
//            filter_text.ptr = "filter:";
//            filter_text.width = 40;
//            _gui->fittedLabel(0, filter_text);
//
//            // Filter input text
//            _gui->push({.layer = 0, .style = &_gui->_theme.filter_input});
//            strncpy(_filter_buffer, _filter_text.c_str(), sizeof(_filter_buffer));
//            size_t filter_size = _filter_text.size() + 1;
//            size_t filter_size = _filter_text.size() + 1;
//            uint64_t filter_id = _gui->getAddressAsInt(&_filter_text);
//            if (_gui->textField(0, filter_id, _filter_buffer, filter_size, filter_size))
//            {
//                _filter_text = _filter_buffer;
//                applyFilter();
//            }
//            _gui->pop(0);
//        }
//        _gui->pop(0);

        _gui->pop();
    }

    void FileBrowser::updateFileGrid()
    {
        int16_t layer = 0;
        bool tool_tips = false;

        if( _gui->buttonDown() && !_button_down )
        {
            _curr_selection = 0;
            _button_down = true;
        }
        else if( !_gui->buttonDown() )
        {
            _button_down = false;
        }

        // Begin scroll container
        _gui->beginScrollContainer(layer, _scroll_container_id);

        // Start grid container
        _gui->push({.layer = layer, .style = &_gui->_theme.file_container});

        for (size_t i = 0; i < _filtered_files.size(); i++)
        {
            FileEntry& file = _filtered_files[i];

            // Begin column for file item
            _gui->push
            ({
                .uid = &file.uid,
                .layer = layer,
                .rect =
                {
                    .width = 120,
                    .height = 120,
                    .x = 0,
                    .y = 0
                },
                .alignment = ui::Alignment
                {
                    .content = {ui::AlignPosX::CENTER, ui::AlignPosY::CENTER},
                    .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                    .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                    .direction = ui::AlignDir::VERTICAL,
                    .wrap_around = true,
                },
                .border.corner_curves = {8,8,8,8},
                .padding = {6,6,6,6},
                .color = 0xFFFFFF0A,
            });
            {
                // File icon
                ui::WidgetDesc icon_desc;
                icon_desc.uid = &file.icon_uid;
                icon_desc.rect = {0, 0, ICON_SIZE, ICON_SIZE};
                icon_desc.single_click = ui::ClickTrigger::Immediate;
                icon_desc.double_click = ui::ClickTrigger::Immediate;
                icon_desc.color = 0xFFFFFF0A;
                icon_desc.border.corner_curves = {6,6,6,6};

                // Set style based on file type and selection
                /*
                if (file.is_directory)
                {
                    icon_desc.style = &_gui->_theme.folder_icon;
                }
                else
                {
                    icon_desc.style = &_gui->_theme.file_icon;
                }
                 */

                // Use put for the icon (no container)
                _gui->put(icon_desc);

                // Check for interactions
                if (_gui->click( file.icon_uid ))
                {
                    handleFileClick(i, 0/*_gui->layout()->input()->modifierKeys()*/);
                    _curr_selection = i;
                }

                if (_gui->layout()->doubleClick(file.icon_uid))
                {
                    if (file.is_directory)
                    {
                        navigateTo(file.path);
                        _gui->pop();
                        break;
                    }
                }

                _gui->put({.style = &_gui->_theme.y_seperator});

                // Check for drag start on this icon
                if (_gui->pointerDragging() && _gui->mouseover(file.icon_uid))
                {
                    if (!_is_dragging)
                    {
                        startDragOperation(i);
                    }
                }

                // Track hover for tooltips
                if (_gui->mouseover(file.icon_uid) || _gui->mouseover(file.uid))
                {
                    tool_tips = true;
                    _hovered_file_index = i;
                    _tooltip_position = _gui->pointer();
                }

                file.name_text.ptr = file.display_name.c_str();
                _gui->label(layer, file.name_text);
            }
            _gui->pop();

            /*
            // Move to next column or next row
            column_count++;
            if (column_count >= columns)
            {
                column_count = 0;
                _gui->endRow(layer);
                _gui->beginRow(layer);
                _gui->putSpacer(layer);
            }*/
        }

        // End grid container
        _gui->pop();

        // End any remaining containers
        _gui->endScrollContainer();

        if (!_gui->buttonDown())
        {
            _begin_dragging = false;
        }

        if ( !tool_tips )
        {
            _hovered_file_index.reset();
        }
    }

    void pointInBrowserAreaOperation(ui::Layout* layout, const ui::UID* uid, void* data)
    {
        FileBrowser::HotState* param = (FileBrowser::HotState*)data;
        *param->hot = layout->testPointVsRect(layout->getPointerPosition(), layout->elem( *uid )->rect);
    }

    void FileBrowser::updateSelectionBox()
    {
        _gui->pushDeferredOp<FileBrowser::HotState>(&_uid_main, pointInBrowserAreaOperation, {&_hot});

        if (_begin_dragging || _is_dragging || !_hot) return;

        // Update selection box while dragging
        if (_gui->buttonDown())
        {
            if (!_is_selecting)
            {
                if (_gui->dragging())
                {
                    kege::dvec2 current_pos = _gui->pointer();
                    _selection_start.x = current_pos.x;
                    _selection_start.y = current_pos.y;
                    _is_selecting = true;
                }
                else if( !_deselect_selection_box && _curr_selection == 0 )
                {
                    _deselect_selection_box = true;
                }
            }
        }
        else
        {
            _is_selecting = false;
        }

        if ( _is_selecting )
        {
            kege::dvec2 current_pos = _gui->pointer();
            _selection_box.x = std::min(_selection_start.x, current_pos.x);
            _selection_box.y = std::min(_selection_start.y, current_pos.y);
            _selection_box.width = std::abs(current_pos.x - _selection_start.x);
            _selection_box.height = std::abs(current_pos.y - _selection_start.y);

            // Draw selection box on layer 1
            ui::WidgetDesc selection_desc;
            selection_desc.rect = _selection_box;
            selection_desc.color = 0xFFFFFF10;
            selection_desc.layer = 1;
            selection_desc.border.corner_curves = {8,8,8,8};
            _gui->put(selection_desc);

            updateSelectionFromBox();
        }
        else if( _deselect_selection_box )
        {
            clearSelection();
            _deselect_selection_box = false;
        }
    }

    void FileBrowser::updateTooltips()
    {
        if ( !_hovered_file_index.has_value() || !_show_tool_tip )
        {
            return;
        }

        const auto& file = _filtered_files[ _hovered_file_index.value() ];

        // Create tooltip text
        //std::string tooltip_text = "Name: " + file.name + "\n";
        _tooltip_text = "Name: " + file.name + "\n";
        _tooltip_text += "Type: " + std::string(file.is_directory ? "Directory" : "File") + "\n";

        if (!file.is_directory)
        {
            _tooltip_text += "Size: " + formatFileSize(file.size) + "\n";
            if ( !file.extension.empty() )
            {
                _tooltip_text += "Extension: " + file.extension + "\n";
            }
        }

        // Format modification time
        auto time_t = std::chrono::system_clock::to_time_t(file.modified_time);
        std::tm* timeinfo = std::localtime(&time_t);
        char time_buffer[80];
        std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        _tooltip_text += "Modified: " + std::string(time_buffer);

        // Generate UID for tooltip
        //ui::UID tooltip_uid = _gui->layout()->generateUID();

        // Draw tooltip on layer 2
        ui::WidgetDesc tooltip_desc;
        //tooltip_desc.uid = &tooltip_uid;
        tooltip_desc.rect = {float(_tooltip_position.x + 10), float(_tooltip_position.y + 10), 0, 0};
        tooltip_desc.style = &_gui->_theme.tooltip;
        //tooltip_desc.color = 0xFFFFFF0F;
        tooltip_desc.layer = 1;

        ui::Text tooltip_ui_text;
        tooltip_ui_text.ptr = _tooltip_text.c_str();
        tooltip_ui_text.width = 250;
        tooltip_ui_text.height = 150;

        tooltip_desc.text = tooltip_ui_text;

        _gui->put(tooltip_desc);
    }

    void FileBrowser::startDragOperation(size_t initial_index)
    {
        if (_begin_dragging)
            return;

        _begin_dragging = true;
        
        auto i = std::find(_selected_indices.begin(), _selected_indices.end(), initial_index);
        if ( i == _selected_indices.end()) {
            return;
        }

        _is_dragging = true;
        _drag_start = _gui->pointer();
        _dragged_indices.clear();

        // If the initial file is selected, drag all selected files
        if (_filtered_files[initial_index].selected)
        {
            _dragged_indices = _selected_indices;
        }
        else
        {
            _dragged_indices.push_back(initial_index);
        }
    }

    void FileBrowser::completeDragOperation()
    {
        // Find drop target (check if pointer is over a folder)
//        for (size_t i = 0; i < _filtered_files.size(); i++)
//        {
//            const auto& file = _filtered_files[i];
//            if (file.is_directory && isPointOverFile(i, _gui->pointer()))
//            {
//                moveFilesToFolder(_dragged_indices, file.path);
//                break;
//            }
//        }
    }

    //void updateDragPreview(uint64_t frame_id)

    void FileBrowser::updateDragAndDrop()
    {
        if (!_is_dragging)
            return;

        //_gui->mouseover( _uid_main );

        // Draw drag preview
        for (size_t idx : _dragged_indices)
        {
            if (idx < _filtered_files.size())
            {
                //const auto& file = _filtered_files[idx];

                // Draw dragged file icon at pointer position
                ui::WidgetDesc drag_desc;
                //drag_desc.uid = reinterpret_cast<ui::UID*>(_gui->getAddressAsInt(&file) + idx + 1000);
                drag_desc.rect.x = _gui->pointer().x - ICON_SIZE/2;
                drag_desc.rect.y = _gui->pointer().y - ICON_SIZE/2;
                drag_desc.rect.width = ICON_SIZE;
                drag_desc.rect.height = ICON_SIZE;
                //drag_desc.style = file.is_directory ? &_gui->_theme.folder_icon : &_gui->_theme.file_icon;
                drag_desc.layer = 1; // Drag layer (on top)
                drag_desc.color = 0x80FFFF30; // Semi-transparent

                _gui->put(drag_desc);
            }
        }

        // Check for drop
        if (!_gui->buttonDown())
        {
            completeDragOperation();
            _is_dragging = false;
        }
    }

    void FileBrowser::updateContextMenu()
    {
        // TODO: Implement context menu for right-click operations
        // This would show options like Rename, Delete, Copy, Paste, etc.
    }

    void FileBrowser::handleFileClick(size_t index, uint32_t modifiers)
    {
        // Define modifier constants (adjust based on your input system)
        const uint32_t MODIFIER_CTRL = 0x01;
        const uint32_t MODIFIER_SHIFT = 0x02;

        if (modifiers & MODIFIER_CTRL)
        {
            // Toggle selection
            _filtered_files[index].selected = !_filtered_files[index].selected;
            if (_filtered_files[index].selected)
            {
                _selected_indices.push_back(index);
            }
            else
            {
                _selected_indices.erase(
                    std::remove(_selected_indices.begin(), _selected_indices.end(), index),
                    _selected_indices.end()
                );
            }
        }
        else if (modifiers & MODIFIER_SHIFT && !_selected_indices.empty())
        {
            // Range selection
            size_t start = _selected_indices.back();
            size_t end = index;
            if (start > end) std::swap(start, end);

            for (size_t i = start; i <= end; i++)
            {
                if (!_filtered_files[i].selected)
                {
                    _filtered_files[i].selected = true;
                    _selected_indices.push_back(i);
                }
            }
        }
        else
        {
            if (_selected_indices.size() > 1)
            {
                auto i = std::find(_selected_indices.begin(), _selected_indices.end(), index);
                if ( i != _selected_indices.end()) {
                    return;
                }
            }
            // Single selection
            clearSelection();
            _filtered_files[index].selected = true;
            _selected_indices.push_back(index);
        }
    }


    struct SelectionOp
    {
        ui::Rect& selection_box;
        std::vector<size_t>& selected_indices;
        std::vector<FileBrowser::FileEntry>& files;
    };

    void executeSelectionFromBoxOp(ui::Layout* layout, const ui::UID* id, void* data)
    {
        SelectionOp* params = reinterpret_cast<SelectionOp*>(data);

        for (auto& file : params->files)
        {
            file.selected = false;
        }
        params->selected_indices.clear();

        for (size_t i = 0; i < params->files.size(); i++)
        {
             if (layout->intersect( params->selection_box, layout->elem( params->files[i].uid )->rect ))
             {
                 params->files[i].selected = true;
                 params->selected_indices.push_back(i);
             }
        }
    }

    void FileBrowser::updateSelectionFromBox()
    {
        // Clear current selection
        _gui->pushDeferredOp(nullptr, executeSelectionFromBoxOp, SelectionOp{
            _selection_box,
            _selected_indices,
            _filtered_files,
        });
    }

    void FileBrowser::clearSelection()
    {
        for (auto& file : _filtered_files)
        {
            file.selected = false;
        }
        _selected_indices.clear();
    }

    void FileBrowser::navigateTo(const fs::path& path)
    {
        if (fs::exists(path) && fs::is_directory(path))
        {
            _current_path = fs::canonical(path);
            _history.push(_current_path);
            refreshDirectory();
        }
    }

    void FileBrowser::createNewFolder()
    {
        // Generate unique folder name
        std::string base_name = "New Folder";
        fs::path new_folder_path = _current_path / base_name;
        int counter = 1;

        while (fs::exists(new_folder_path))
        {
            new_folder_path = _current_path / (base_name + " (" + std::to_string(counter++) + ")");
        }

        try
        {
            fs::create_directory(new_folder_path);
            refreshDirectory();

            // Select and prepare for rename
            for (size_t i = 0; i < _filtered_files.size(); i++)
            {
                if (_filtered_files[i].path == new_folder_path)
                {
                    clearSelection();
                    _filtered_files[i].selected = true;
                    _selected_indices.push_back(i);
                    // TODO: Initiate rename operation
                    break;
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error creating folder: " << e.what() << std::endl;
        }
    }

    void FileBrowser::moveFilesToFolder(const std::vector<size_t>& indices, const fs::path& target_folder)
    {
        for (size_t idx : indices)
        {
            if (idx < _filtered_files.size())
            {
                const auto& file = _filtered_files[idx];
                fs::path new_path = target_folder / file.path.filename();

                PendingOperation op;
                op.type = PendingOperation::MOVE;
                op.source = file.path;
                op.target = new_path;
                _pending_operations.push_back(op);
            }
        }
    }

    void FileBrowser::processPendingOperations()
    {
        for (const auto& op : _pending_operations)
        {
            try
            {
                switch (op.type)
                {
                    case PendingOperation::MOVE:
                        fs::rename(op.source, op.target);
                        break;
                    case PendingOperation::DELETE:
                        if (fs::is_directory(op.source))
                            fs::remove_all(op.source);
                        else
                            fs::remove(op.source);
                        break;
                    case PendingOperation::CREATE_FOLDER:
                        fs::create_directory(op.source);
                        break;
                    case PendingOperation::RENAME:
                        fs::rename(op.source, op.target);
                        break;
                }
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Error performing operation: " << e.what() << std::endl;
            }
        }

        if (!_pending_operations.empty())
        {
            refreshDirectory();
            _pending_operations.clear();
        }
    }

    std::string FileBrowser::formatFileSize(uint64_t size)
    {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unit_index = 0;
        double formatted_size = static_cast<double>(size);

        while (formatted_size >= 1024.0 && unit_index < 4)
        {
            formatted_size /= 1024.0;
            unit_index++;
        }

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f %s", formatted_size, units[unit_index]);
        return std::string(buffer);
    }

}
