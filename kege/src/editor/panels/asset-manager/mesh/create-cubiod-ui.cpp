//
//  create-cubiod-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#include "../../../editor-layer.hpp"
#include "create-cubiod-ui.hpp"

namespace kege::ui{

    bool CreateCuboidMeshUI::submit(GUI* gui, const char* label)
    {
        uint64_t user_id = UI_BASE_ID();
        gui->put
        ({
            .layer = 2,
            .user_id = user_id,
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
        return gui->click(user_id);
    }

    void CreateCuboidMeshUI::text(GUI* gui, const char* label)
    {
        gui->push
        ({
            .layer = 2,
            //.border.corner_curves = {border_radius,0,0,border_radius},
            .color = 0xFFFFFF08,
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER,
            },
            .width = ui::extend(),
            .height = ui::fixed(40),
        });
        gui->put
        ({
            .layer = 2,
            .text = ui::Text{.width = 60, .height = 20, .color = 0xFFFFFF30, .font_size = 24, .ptr = label},
            .single_click = ui::ClickTrigger::OnRelease,
            .rect = {0,0, 60, 30},
            .color = 0xFFFFFF00,
            .padding = {10,5,5,10},
        });
        gui->textField(2, gui->getAddressAsInt(_asset_name), _asset_name, _buffer_capacity, _current_size);
        gui->pop();
    }

    void CreateCuboidMeshUI::input(GUI* gui, const char* label, double& value)
    {
        gui->push
        ({
            .layer = 2,
            .padding = {10,5,10,5},
            //.border.corner_curves = {border_radius,0,0,border_radius},
            //.rect = {0,0,200,30},
            .color = 0xFFFFFF08,
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .items = ui::AlignItem::CENTER,
            },
            .width = ui::extend(),
            .height = ui::fixed(40),
        });
        gui->put
        ({
            .layer = 2,
            .text = ui::Text{.width = 60, .height = 24, .color = 0xFFFFFF30, .font_size = 24, .ptr = label},
            .single_click = ui::ClickTrigger::OnRelease,
            .rect = {0,0, 60, 24},
            .color = 0xFFFFFF00,
            //.padding = {10,5,5,10},
        });
        gui->scrubber(0, 2, value);
        gui->pop();
    }

    bool CreateCuboidMeshUI::create(GUI* gui)
    {
        if (_asset_name[0] == 0)
        {
            snprintf(_asset_name, 31, "cube-mesh-%i", _count++);
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
            text(gui, "Name:");
            gui->put({.rect = {0,0,50,20}});
            input(gui, "Width:", _width);
            input(gui, "Height:", _height);
            input(gui, "Depth:", _depth);

            gui->put({.style = &gui->_theme.y_seperator});

            if( submit(gui, "Submit") )
            {
                ref::AssetManager asset_manager = _manager->getEditor()->getAssetManager();
                kege::Ref<kege::MeshPrimitive> mesh = new CuboidMesh( kege::vec3(0.0), kege::vec3(_width, _height, _depth) );

                // Create metadata
                AssetMetadata metadata;
                metadata.handle         = asset_manager->add< kege::Ref<kege::MeshPrimitive> >(_asset_name, mesh);
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
        gui->pop();
        return false;
    }

    CreateCuboidMeshUI::CreateCuboidMeshUI(ui::AssetManagerUI* m)
    : CreateMeshUI("Cubiod")
    ,   _manager(m)
    {}

    int CreateCuboidMeshUI::_count = 1;
}

