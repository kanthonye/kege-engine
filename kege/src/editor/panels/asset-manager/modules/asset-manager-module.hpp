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

        AssetManagerModule(AssetManagerUI* m,kege::GUI* g, uint64_t user_id):_manager(m), _gui(g), _user_id(user_id){}
        virtual ~AssetManagerModule(){}

    protected:

        AssetManagerUI* _manager;
        kege::GUI* _gui;
        uint64_t _user_id;
    };
}
#endif /* asset_manager_module_hpp */
