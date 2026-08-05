//
//  asset-manager-create-asset.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-create-asset.hpp"


namespace kege::ui{

    AssetManagerCreateAsset::AssetManagerCreateAsset(AssetManagerUI* m,kege::UI* g)
    : AssetManagerModule(m,g)
    {}

    void AssetManagerCreateAsset::operator()(const std::string& type, void* data)
    {
        if (type == "OpenAddAssetWindow")
        {
            _open_window = !_open_window;
        }
        else if (type == "CloseAddAssetWindow")
        {
            _open_window = false;
        }
    }

    void AssetManagerCreateAsset::update()
    {
        if (_open_window)
        {
            static std::vector<std::string> creates;
            static std::vector<std::string> names;
            static std::vector< kege::ui::Text > list;
            if (list.empty())
            {
                names = std::vector<std::string>{"Mesh", "Material", "Textures", "Shader", "Sounds", "Scripts"};
                for (int i=0; i<names.size(); ++i)
                {
                    list.push_back
                    ({
                        .width = 100,
                        .font_size = 20,
                        .color = 0xFFFFFFFF,
                        .data = names[i].c_str(),
                    });
                    creates.push_back("OpenCreate" + names[i] + "Window");
                }
            }

            uint32_t id_offset = (uint32_t)names.size();
            _ui->push
            ({
                .user_id = _uid[ id_offset ],
                .wid = _ui->newElem
                ({
                    .position = Positioning::Absolute,
                    .padding = {20,20,20,20},
                }),
                .quad = {0.5f, 20.0f, 300.f, 200.f, 0x000000FF},
                .single_click = ui::ClickTrigger::Immediate,
                .double_click = ui::ClickTrigger::Immediate,
                .clip_overflow = true,
            });
            int selected_index;
            if( _ui->select(_uid, list, selected_index) )
            {
                _manager->handle(this, creates[selected_index].c_str(), nullptr);
                _open_window = false;
            }
            _ui->pop();
        }
    }
}


