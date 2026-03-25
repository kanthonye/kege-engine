//
//  create-ellipsoid-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef create_ellipsoid_ui_hpp
#define create_ellipsoid_ui_hpp

#include "create-mesh-ui.hpp"
#include "../ui-asset-manager.hpp"

namespace kege::ui{

    class CreateEllipsoidMeshUI : public CreateMeshUI
    {
    public:

        bool submit(GUI* gui, const char* label);
        void text(GUI* gui, const char* label);
        void input(GUI* gui, const char* label, double& value);

        bool create(GUI* gui);

        CreateEllipsoidMeshUI(AssetManagerUI* manager);

        AssetManagerUI* _manager;

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        WidgetHandle _submit_button_handle;
        double _radius_x, _radius_y, _cols, _rows;
        static int _count;
    };

}
#endif /* create_ellipsoid_ui_hpp */
