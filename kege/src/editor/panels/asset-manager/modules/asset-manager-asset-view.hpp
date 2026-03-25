//
//  asset-manager-asset-view.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef asset_manager_asset_view_hpp
#define asset_manager_asset_view_hpp

#include "asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerAssetView : public AssetManagerModule
    {
    public:

        AssetManagerAssetView(AssetManagerUI* m,kege::GUI* g, uint64_t user_id)
        : AssetManagerModule(m,g,user_id){}
        
        void operator()(const std::string& type, void* data);
        void handleAssetClick(size_t asset_index, Modifiers modifiers);
        void updateGridView();
        void updateListView();
        void update();


        void startAssetDrag(size_t initial_index);
        void updateDrag();

    private:


        struct DragObj
        {
            uint64_t index;
            std::string snum;
        };


        std::vector<DragObj> _dragged_asset_handles;
        ui::ViewMode _view_mode;
        kege::dvec2 _drag_start;
        bool _is_dragging_assets;
        bool _butn_down;
    };
}


#endif /* asset_manager_asset_view_hpp */
