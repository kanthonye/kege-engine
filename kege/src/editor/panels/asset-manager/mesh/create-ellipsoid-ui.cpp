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

    bool CreateEllipsoidMeshUI::create(UI* ui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "ellipsoid-mesh-%i", _count++);
            _text = ui->gui()->text(_asset_name, 20);
        }
        ui->push
        ({
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
            ui->labelInput("Name:", _uid[1], _text_input_mode, _text);
            ui->put({.rect = {0,0,50,20}});

            ui::Text text = {.width = 50, .height = 20, .font_size = 20};

            text.ptr = "RadiusX:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[1], text, _radius_x);
            text.ptr = "RadiusY:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[2], text, _radius_y);
            text.ptr = "Columns:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3], text, _cols);
            text.ptr = "Rows:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[4], text, _rows);

            ui->put({.style = &ui->theme()->y_seperator});

            if( ui->submit(_uid[5], "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getAssetManager();
                kege::Ref<kege::Mesh> mesh = new EllipsoidMesh( _radius_x, _radius_y, _cols, _rows );

                // Create metadata
                AssetMetadata metadata;
                metadata.handle         = asset_manager->add< kege::Ref<kege::Mesh> >(_asset_name, mesh);
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
        ui->pop();
        return false;
    }

    CreateEllipsoidMeshUI::CreateEllipsoidMeshUI(ui::AssetManagerUI* m)
    :   CreateMeshUI("Ellipsoid", m)
    {}

    int CreateEllipsoidMeshUI::_count = 1;
}
