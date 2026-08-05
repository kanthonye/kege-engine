//
//  create-cylinder-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../../../editor-layer.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "create-cylinder-ui.hpp"

namespace kege::ui{

    bool CreateCylinderMeshUI::create(UI* ui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "cylinder-mesh-%i", _count++);
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

            text.data = "Radius:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[4], text, _radius);
            text.data = "Height:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[4], text, _height);
            text.data = "Rings:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3], text, _rings);

            ui->put({.wid = ui->newElem(ui->theme()->y_seperator)});

            //uint64_t user_id = UI_BASE_ID();
            if( ui->submit(_uid[4], "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getAssetManager();
                kege::Ref<kege::Mesh> mesh = new CylinderMesh( _radius, _height, _rings );

                // Create metadata
                AssetMetadata metadata;
                metadata.handle         = asset_manager->add< kege::Ref<kege::Mesh> >(_asset_name, mesh);
                metadata.name           = _asset_name;
                metadata.display_name   = metadata.name;
                metadata.original_path  = "";
                metadata.type           = AssetType::MODEL;
                metadata.loader_id      = ".kgm";
                metadata.import_time    = std::chrono::system_clock::now();
                metadata.last_modified  = std::filesystem::file_time_type::min();
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

    CreateCylinderMeshUI::CreateCylinderMeshUI(ui::AssetManagerUI* m)
    : CreateMeshUI("Cylinder", m)
    {}

    int CreateCylinderMeshUI::_count = 1;
}

