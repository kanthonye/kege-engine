//
//  create-plane-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef create_plane_ui_hpp
#define create_plane_ui_hpp

#include "create-mesh-ui.hpp"
#include "../ui-asset-manager.hpp"

namespace kege::ui{

    class CreateGridMeshUI : public CreateMeshUI
    {
    public:

        bool submit(GUI* gui, const char* label);
        void text(GUI* gui, const char* label);
        void input(GUI* gui, const char* label, double& value);

        bool create(GUI* gui);

        CreateGridMeshUI(AssetManagerUI* manager);

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        double _width, _height, _cols, _rows;
        static int _count;
    };

}
#endif /* create_plane_ui_hpp */
