//
//  asset-manager-module.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#ifndef asset_manager_module_hpp
#define asset_manager_module_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui{

    class AssetManagerUI;
    class AssetManagerModule : public kege::RefCounter
    {
    public:

        virtual void operator()(const std::string& type, void* data) = 0;
        virtual void update() = 0;

        const std::vector<AssetMetadata>* getAssets()const;
        std::vector<AssetMetadata>* getAssets();

        const std::vector<size_t>* getSelectedIndices()const;
        std::vector<size_t>* getSelectedIndices();

        const std::vector<size_t>* getFilteredIndices()const;
        std::vector<size_t>* getFilteredIndices();

        std::mutex& getAssetMutex();

        AssetManagerModule(AssetManagerUI* m,kege::UI* g):_manager(m), _ui(g){}
        virtual ~AssetManagerModule(){}

    protected:

        AssetManagerUI* _manager;
        kege::UI* _ui;
    };
}
#endif /* asset_manager_module_hpp */
