//
//  create-asset-mesh-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#ifndef create_asset_mesh_ui_hpp
#define create_asset_mesh_ui_hpp

#include "asset-manager-module.hpp"
#include "../mesh/create-mesh-ui.hpp"

namespace kege::ui{

    class CreateAssetMeshUI : public AssetManagerModule
    {
    public:

        CreateAssetMeshUI(AssetManagerUI* m,kege::UI* g);
        void operator()(const std::string& type, void* data);
        void update();

        ~CreateAssetMeshUI();
        
    private:

        static std::vector< Ref< CreateMeshUI > > _create_mesh_uis;
        static std::vector< kege::ui::Text > _shape_labels;


        ui::UID _uid;
        uint32_t _id_offset;

        //ui::ID _close_butn;
        bool _open_window;
        ui::Quad _quad;
        int _selection;

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _buffer[32];
    };

}
#endif /* create_asset_mesh_ui_hpp */
