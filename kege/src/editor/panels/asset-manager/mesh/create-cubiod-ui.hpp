//
//  create-cubiod-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#ifndef create_cubiod_ui_hpp
#define create_cubiod_ui_hpp

#include "create-mesh-ui.hpp"
#include "../ui-asset-manager.hpp"

namespace kege::ui{

    class CreateCuboidMeshUI : public CreateMeshUI
    {
    public:

        bool submit(GUI* gui, const char* label);
        void text(GUI* gui, const char* label);
        void input(GUI* gui, const char* label, double& value);

        bool create(GUI* gui);

        CreateCuboidMeshUI(AssetManagerUI* manager);

        AssetManagerUI* _manager;

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        WidgetHandle _handle_width;
        WidgetHandle _handle_height;
        WidgetHandle _handle_depth;

        double _width, _height, _depth;
        static int _count;
    };

}
#endif /* create_cubiod_ui_hpp */
