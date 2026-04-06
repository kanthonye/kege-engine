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

        bool create(GUI* gui);

        CreateCuboidMeshUI(AssetManagerUI* manager);

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];

        double _width, _height, _depth;
        static int _count;
    };

}
#endif /* create_cubiod_ui_hpp */
