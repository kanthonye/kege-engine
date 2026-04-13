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

        bool submit(UI* ui, const char* label);
        void text(UI* ui, const char* label);
        void input(UI* ui, const char* label, double& value);

        bool create(UI* ui);

        CreateEllipsoidMeshUI(AssetManagerUI* manager);

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        float _radius_x, _radius_y, _cols, _rows;
        static int _count;
    };

}
#endif /* create_ellipsoid_ui_hpp */
