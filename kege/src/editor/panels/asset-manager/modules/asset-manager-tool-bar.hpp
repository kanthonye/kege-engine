//
//  asset-manager-tool-bar.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef asset_manager_tool_bar_hpp
#define asset_manager_tool_bar_hpp

#include "asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerToolBar : public AssetManagerModule
    {
    public:

        AssetManagerToolBar(AssetManagerUI* m,kege::UI* g);
        void operator()(const std::string& type, void* data){}
        void update();

    private:


        ui::Text text;
        TextFieldMode mode;
        ViewMode _view_mode;

        char _search_buffer[256];
        size_t _search_size;
        ui::UID _uid;
    };

}
#endif /* asset_manager_tool_bar_hpp */
