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

    bool CreateGridMeshUI::create(UI* ui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "grid-mesh-%i", _count++);
            _text = ui->gui()->text(_asset_name, 20);
        }
        ui->push
        ({
            .wid = ui->newElem
            ({
                .width = ui::extend(),
                .height = ui::extend(),
                .quad_color = 0xFFFFFF00,
                .padding = {20,20,20,20},
                .alignment =
                {
                    .gap = {2,2},
                    .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                    .direction = ui::AlignDir::DOWN,
                },
            }),
            .quad = {0,0,0,0},
        });
        {
            ui->labelInput("Name:", _uid[1], _text_input_mode, _text);
            ui->put({.quad = {0,0,50,20}});

            ui::Text text = {.width = 50, .font_size = 20};

            text.data = "Width:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[1], text, _width);
            text.data = "Height:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[2], text, _height);
            text.data = "Columns:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3], text, _cols);
            text.data = "Rows:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[4], text, _rows);

            ui->put({.wid = ui->newElem(ui->theme()->y_seperator) });

            if( ui->submit(_uid[5], "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getAssetManager();
                kege::Ref<kege::Mesh> mesh = new GridMesh( _width, _height, _cols, _rows );

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

    CreateGridMeshUI::CreateGridMeshUI(ui::AssetManagerUI* m)
    :   CreateMeshUI("Grid", m)
    {}

    int CreateGridMeshUI::_count = 1;
}
