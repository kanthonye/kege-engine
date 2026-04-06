//
//  create-ellipsoid-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../../../editor-layer.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "create-ellipsoid-ui.hpp"

namespace kege::ui{

    bool CreateEllipsoidMeshUI::create(GUI* gui)
    {
        int16_t layer = 2;
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "ellipsoid-mesh-%i", _count++);
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
            gui->labelScrubber(_uid[1], layer, "RadiusX:", _radius_x);
            gui->labelScrubber(_uid[2], layer, "RadiusY:", _radius_y);
            gui->labelScrubber(_uid[3], layer, "Columns:", _cols);
            gui->labelScrubber(_uid[4], layer, "Rows:", _rows);

            gui->put({.style = &gui->theme().y_seperator});

            if( gui->submit(_uid[5], "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getManager()->getEditor()->getAssetManager();
                kege::Ref<kege::MeshPrimitive> mesh = new EllipsoidMesh( _radius_x, _radius_y, _cols, _rows );

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

    CreateEllipsoidMeshUI::CreateEllipsoidMeshUI(ui::AssetManagerUI* m)
    :   CreateMeshUI("Ellipsoid", m)
    {}

    int CreateEllipsoidMeshUI::_count = 1;
}
