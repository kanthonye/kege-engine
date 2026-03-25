//
//  asset-manager-create-asset.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-create-asset.hpp"


namespace kege::ui{

    AssetManagerCreateAsset::AssetManagerCreateAsset(AssetManagerUI* m,kege::GUI* g, uint64_t user_id)
    : AssetManagerModule(m,g,user_id)
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
            static std::vector<ListElem> list;
            if (list.empty())
            {
                names = std::vector<std::string>{"Mesh", "Material", "Textures", "Shaders", "Sounds", "Scripts"};
                for (int i=0; i<names.size(); ++i)
                {
                    list.push_back(ListElem
                    {
                        .text = ui::Text
                        {
                            .ptr = names[i].c_str(),
                            .width = 100,
                            .font_size = 20,
                            .height = 15,
                            .color = 0xFFFFFFFF
                        }
                    });
                    creates.push_back("OpenCreate" + names[i] + "Window");
                }
            }

            _gui->push
            ({
                .layer = 1,
                .user_id = UI_BASE_ID(),
                .color = 0x000000FF,
                .padding = {20,20,20,20},
                .clip_overflow = true,
                .rect = {0.5f, 20.0f, 300.f, 200.f},
                .position = Positioning::Absolute,
                .single_click = ui::ClickTrigger::Immediate,
                .double_click = ui::ClickTrigger::Immediate,
            });
            int selected_index;
            if( _gui->select(1, list, selected_index) )
            {
                _manager->handle(this, creates[selected_index].c_str(), nullptr);
                _open_window = false;
            }
            _gui->pop();
        }
    }
}


