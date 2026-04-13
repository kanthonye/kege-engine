//
//  asset-manager-create-asset.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef asset_manager_create_asset_hpp
#define asset_manager_create_asset_hpp

#include "asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerCreateAsset : public AssetManagerModule
    {
    public:

        AssetManagerCreateAsset(AssetManagerUI* m,kege::UI* g);
        void operator()(const std::string& type, void* data);
        void update();

    private:

        std::vector< Ref<AssetManagerModule> > _modules;
        kege::ui::UID _uid;
        bool _open_window;
    };

}
#endif /* asset_manager_create_asset_hpp */
