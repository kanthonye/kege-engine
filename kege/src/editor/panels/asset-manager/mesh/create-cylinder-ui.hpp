//
//  create-cylinder-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef create_cylinder_ui_hpp
#define create_cylinder_ui_hpp

#include "create-mesh-ui.hpp"
#include "../ui-asset-manager.hpp"

namespace kege::ui{

    class CreateCylinderMeshUI : public CreateMeshUI
    {
    public:

        bool create(GUI* gui);
        CreateCylinderMeshUI(AssetManagerUI* manager);

        AssetManagerUI* _manager;

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        double _radius, _height, _rings;
        static int _count;
    };

}
#endif /* create_cylinder_ui_hpp */
