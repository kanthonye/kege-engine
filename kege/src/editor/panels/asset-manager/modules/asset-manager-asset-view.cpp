//
//  asset-manager-asset-view.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-asset-view.hpp"

namespace kege::ui{

    void AssetManagerAssetView::operator()(const std::string& type, void* data)
    {}

    void AssetManagerAssetView::handleAssetClick(size_t asset_index, Modifiers modifiers)
    {
        std::vector<size_t>* selected_indices = getSelectedIndices();

        if (contain(modifiers, Modifiers::Control))
        {
            // Toggle selection
            auto it = std::find(selected_indices->begin(), selected_indices->end(), asset_index);
            if (it != selected_indices->end())
            {
                selected_indices->erase(it);
            }
            else
            {
                selected_indices->push_back(asset_index);
            }
        }
        else if (contain(modifiers, Modifiers::Shift) && !selected_indices->empty())
        {
            // Range selection
            size_t start = selected_indices->back();
            size_t end = asset_index;
            if (start > end) std::swap(start, end);

            for (size_t i = start; i <= end; i++)
            {
                if (std::find(selected_indices->begin(), selected_indices->end(), i) == selected_indices->end()) {
                    selected_indices->push_back(i);
                }
            }
        }
        else
        {
            // Toggle selection
            auto it = std::find(selected_indices->begin(), selected_indices->end(), asset_index);
            if (it != selected_indices->end())
            {
                return;
            }
            // Single selection
            selected_indices->clear();
            selected_indices->push_back(asset_index);
        }
    }
    void AssetManagerAssetView::startAssetDrag(size_t initial_index)
    {
        std::cout << "startAssetDrag\n";
        _is_dragging_assets = true;
        _dragged_asset_handles.clear();
        _drag_start.x = _gui->pointer().x;
        _drag_start.y = _gui->pointer().y;

        std::vector<size_t>* selected_indices = getSelectedIndices();
        std::vector<AssetMetadata>* assets = getAssets();
        // If initial asset is selected, drag all selected assets
        if (std::find(selected_indices->begin(), selected_indices->end(), initial_index) != selected_indices->end())
        {
            for (size_t idx : *selected_indices)
            {
                if (idx < assets->size())
                {
                    _dragged_asset_handles.push_back({ .index = assets->at(idx).handle });
                }
            }
        }
        else
        {
            if (initial_index < assets->size())
            {
                _dragged_asset_handles.push_back({ .index = assets->at(initial_index).handle });
            }
        }
    }
    void AssetManagerAssetView::updateDrag()
    {
        if (!_is_dragging_assets) return;

        if (!_dragged_asset_handles.empty())
        {
            _drag_start.x += _gui->deltaPointer().x;
            _drag_start.y += _gui->deltaPointer().y;
            //std::cout <<_gui->pointer() << " is dragging\n";
        }

        if (!_dragged_asset_handles.empty())
        {
            // Draw drag preview
            std::vector<AssetMetadata>* assets = getAssets();
            DragObj& obj = _dragged_asset_handles.at(0);
            obj.snum = std::to_string(_dragged_asset_handles.size());
            // Draw drag preview at pointer position
            ui::WidgetDesc desc;
            desc.user_id = UI_BASE_ID();
            desc.color = assets->at(0).type_color();
            desc.border.corner_curves = {5,5,5,5};
            desc.position = Positioning::Independent;
            desc.layer = 1; // Top layer
            desc.text.width = 30;
            desc.text.height = 20;
            desc.text.font_size = 20;
            desc.text.color = 0xFFFFFFFF;
            desc.text.ptr = obj.snum.c_str();
            desc.rect = { float(_drag_start.x + 10), float(_drag_start.y + 10), 80, 80 };

            _gui->put(desc);
        }

        // Check for drop
        if (!_gui->leftClickDown() && _is_dragging_assets)
        {
            _is_dragging_assets = false;
            std::cout << " not dragging\n";

            std::vector<AssetMetadata>* assets = getAssets();;
            //kege::Communication::broadcast<DragDropOffAssetMetadata>({ &assets->at(0) });
            // TODO: Check drop target
            // If dropped on SceneEditor, create object with asset
            // If dropped on folder, move asset to that folder
        }
    }
    void AssetManagerAssetView::updateGridView()
    {
        // Begin scroll container for virtual scrolling
        _gui->beginScrollContainer(0, _gui->getAddressAsInt(this));

        // Calculate grid layout
        //float available_width = _gui->layout()->getWidth();
        const float ICON_SIZE = 80.0f;
        const float ITEM_W = 120.0f;
        const float ITEM_H = 120.0f;
        const uint16_t font_size = 20;
        //int columns = std::max(1, static_cast<int>(available_width / (ITEM_WIDTH + ITEM_SPACING)));
        //int column_count = 0;

        _gui->beginRow(0);
        //_gui->putSpacer(0);


        // Virtual scrolling: only render visible items
        // TODO: Calculate visible range based on scroll position

        //std::vector<size_t>* selected_indices = getSelectedIndices();
        std::vector<size_t>* filtered_indices = getFilteredIndices();
        std::vector<AssetMetadata>* assets = getAssets();
        int64_t selection = -1;

        //std::lock_guard<std::mutex> lock(_asset_mutex);
        for (size_t visible_idx : *filtered_indices)
        {
            if (visible_idx >= assets->size()) continue;
            auto& asset = assets->at(visible_idx);

            // Begin column for file item
            _gui->push
            ({
                .user_id = asset.asset_uid,
                .layer = 0,
                .rect =
                {
                    .width = ITEM_W,
                    .height = ITEM_H,
                    .x = 0,
                    .y = 0
                },
                .alignment = ui::Alignment
                {
                    .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                    .direction = ui::AlignDir::DOWN,
                    .items = ui::AlignItem::CENTER,
                },
                .single_click = ClickTrigger::Immediate,
                .double_click = ClickTrigger::Immediate,
                .border.corner_curves = {8,8,8,8},
                .padding = {6,6,6,6},
                .color = 0xFFFFFF0A,
            });
            {
                // File icon
                ui::WidgetDesc icon_desc;
                icon_desc.layer = 0;
                icon_desc.user_id = asset.thumbnail_uid;
                icon_desc.rect = {0, 0, ICON_SIZE, ICON_SIZE};
                icon_desc.color = 0xFFFFFF0A;
                icon_desc.border.corner_curves = {6,6,6,6};

                // Use put for the icon (no container)
                _gui->put(icon_desc);
                _gui->put({.style = &_gui->_theme.y_seperator});

                // Check for interactions
                if (_gui->click( asset.asset_uid ))
                {
                    Modifiers modifiers = _gui->layout()->inputManager()->getMouse()->getModifiers();
                    handleAssetClick(visible_idx, modifiers);
                    selection = visible_idx;
                }

//                if (_gui->layout()->doubleClick(asset.asset_uid))
//                {
//                    if (file.is_directory)
//                    {
//                        navigateTo(file.path);
//                        _gui->pop();
//                        break;
//                    }
//                }

                // Check for drag start on this icon
                if (_gui->pointerDragging() && _gui->mouseover(asset.asset_uid))
                {
                    if (!_is_dragging_assets)
                    {
                        startAssetDrag(visible_idx);
                    }
                }

                // Track hover for tooltips
                if (_gui->mouseover(asset.asset_uid) || _gui->mouseover(asset.asset_uid))
                {
//                    tool_tips = true;
//                    _hovered_file_index = i;
//                    _tooltip_position = _gui->pointer();
                }


//                // Asset name
//                std::string display_name = asset.display_name.empty() ? asset.name : asset.display_name;
//                if (display_name.length() > 15)
//                {
//                    display_name = display_name.substr(0, font_size) + "...";
//                }
//
//                ui::Text name_text = _gui->layout()->text(display_name.c_str(), font_size);
//                _gui->label(0, name_text);
//
//
//                file.name_text.ptr = file.display_name.c_str();

                _gui->label(0, ui::Text{
                    .x      = 0,
                    .y      = 0,
                    .width  = 50,
                    .height = 20,
                    .color  = 0xFFFFFFFF,
                    .font_size = font_size,
                    .ptr    = asset.display_name.data(),
                });
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

        _gui->endRow();
        _gui->endScrollContainer();


        if(!_gui->leftClickDown() && _butn_down)
        {
            _butn_down = false;
        }
        else if(_gui->leftClickDown() && !_butn_down)
        {
            _butn_down = true;
            if(selection < 0)
            {
                getSelectedIndices()->clear();
            }
        }
    }
    void AssetManagerAssetView::updateListView()
    {
        // TODO: Implement list view with columns
        // Similar to grid view but with different layout
    }
    void AssetManagerAssetView::update()
    {
        if (_view_mode == ViewMode::GRID)
        {
            updateGridView();
        }
        else
        {
            updateListView();
        }
        updateDrag();

        std::vector<size_t>* selected_indices = getSelectedIndices();
        std::vector<AssetMetadata>* assets = getAssets();
        for (size_t idx : *selected_indices)
        {
            //_gui->get(assets->at(idx).asset_uid)->color = 0x80FFFF22;
        }
    }
}

