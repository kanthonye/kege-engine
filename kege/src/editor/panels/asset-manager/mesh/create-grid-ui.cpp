//
//  create-plane-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../../../editor-layer.hpp"
#include "create-grid-ui.hpp"

namespace kege::ui{

    bool CreateGridMeshUI::create(GUI* gui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "grid-mesh-%i", _count++);
        }
        gui->push
        ({
            .layer = 2,
            .rect = {0,0,0,0},
            .color = 0xFFFFFF00,
            .padding = {20,20,20,20},
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .width = ui::extend(),
            .height = ui::extend(),
        });
        {
            gui->text("name", _asset_name, _current_size, _buffer_capacity);
            gui->put({.rect = {0,0,50,20}});
            gui->input(UI_BASE_ID(), "Width:", _width);
            gui->input(UI_BASE_ID(), "Height:", _height);
            gui->input(UI_BASE_ID(), "Columns:", _cols);
            gui->input(UI_BASE_ID(), "Rows:", _rows);

            gui->put({.style = &gui->_theme.y_seperator});

            if( gui->submit(UI_BASE_ID(), "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getEditor()->getAssetManager();
                kege::Ref<kege::MeshPrimitive> mesh = new GridMesh( _width, _height, _cols, _rows );

                // Create metadata
                AssetMetadata metadata;
                metadata.handle         = asset_manager->add< kege::Ref<kege::MeshPrimitive> >(_asset_name, mesh);
                metadata.name           = _asset_name;
                metadata.display_name   = metadata.name;
                metadata.original_path  = "";
                metadata.type           = AssetType::MODEL;
                metadata.loader_id      = ".kgm";
                metadata.import_time    = std::chrono::system_clock::now();
                metadata.last_modified  = fs::file_time_type::min();
                metadata.file_size      = 0;
                metadata.missing_source = false;
                metadata.category       = "Mesh";
                metadata.loaded         = true;
                _manager->addAsset(metadata);
                _asset_name[0] = 0;
                return true;
            }
        }
        gui->pop();
        return false;
    }

    CreateGridMeshUI::CreateGridMeshUI(ui::AssetManagerUI* m)
    :   CreateMeshUI("Grid")
    ,   _manager(m)
    {}

    int CreateGridMeshUI::_count = 1;
}
