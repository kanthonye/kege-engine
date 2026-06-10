//
//  create-cubiod-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#include "../../../editor-layer.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "create-cubiod-ui.hpp"

namespace kege::ui{

    bool CreateCuboidMeshUI::submit(UI* ui, const char* label)
    {
        ui->put
        ({
            .user_id = _uid[0],
            .text = ui::Text{
                .width = 50,
                .height = 20,
                .color = 0xFFFFFF30,
                .font_size = 24,
                .align = ui::AlignText::Center,
                .ptr = label
            },
            .single_click = ui::ClickTrigger::OnRelease,
            .color = 0xFFFFFF08,
            .padding = {10,5,5,10},
            .width = ui::extend(),
            .height = ui::fixed(30),
        });
        return ui->click(_uid[0]);
    }

    bool CreateCuboidMeshUI::create(UI* ui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "cube-mesh-%i", _count++);
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

            text.ptr = "Width:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[2], text, _width);
            text.ptr = "Height:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3], text, _height);
            text.ptr = "Depth:";
            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[4], text, _depth);

            ui->put({.style = &ui->theme()->y_seperator});

            if( submit(ui, "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getAssetManager();
                kege::Ref<kege::Mesh> mesh = new CuboidMesh( kege::vec3(0.0), kege::vec3(_width, _height, _depth) );

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

    CreateCuboidMeshUI::CreateCuboidMeshUI(ui::AssetManagerUI* m)
    : CreateMeshUI("Cubiod", m)
    {}

    int CreateCuboidMeshUI::_count = 1;
}

