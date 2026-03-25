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

        AssetManagerToolBar(AssetManagerUI* m,kege::GUI* g, uint64_t user_id)
        :   AssetManagerModule(m,g,user_id),_search_size(0){}
        void operator()(const std::string& type, void* data){}
        void update();

    private:

        ViewMode _view_mode;

        char _search_buffer[256];
        size_t _search_size;
    };

}
#endif /* asset_manager_tool_bar_hpp */
