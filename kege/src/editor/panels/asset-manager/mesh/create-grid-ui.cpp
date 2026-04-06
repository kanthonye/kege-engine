//
//  create-plane-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../../../editor-layer.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "create-grid-ui.hpp"

namespace kege::ui{

    bool CreateGridMeshUI::create(GUI* gui)
    {
        int16_t layer = 2;
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "grid-mesh-%i", _count++);
            _text = gui->layout()->text(_asset_name, 20);
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
            gui->labelInput("Name:", _uid[1], layer, _text_input_mode, _text);
            gui->put({.rect = {0,0,50,20}});
            gui->labelScrubber(_uid[1], layer, "Width:", _width);
            gui->labelScrubber(_uid[2], layer, "Height:", _height);
            gui->labelScrubber(_uid[3], layer, "Columns:", _cols);
            gui->labelScrubber(_uid[4], layer, "Rows:", _rows);

            gui->put({.style = &gui->theme().y_seperator});

            if( gui->submit(_uid[5], "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getManager()->getEditor()->getAssetManager();
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
    :   CreateMeshUI("Grid", m)
    {}

    int CreateGridMeshUI::_count = 1;
}
