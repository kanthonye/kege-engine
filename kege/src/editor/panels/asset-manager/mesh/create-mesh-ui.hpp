//
//  create-mesh-ui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#ifndef create_mesh_ui_hpp
#define create_mesh_ui_hpp

#include "../../../common/dependencies.hpp"

namespace kege::ui{

    class AssetManagerUI;
    
    class CreateMeshUI : public kege::RefCounter
    {
    public:
        
        const std::string& getName()const{ return _name;}
        virtual bool create(GUI* gui) = 0;

        CreateMeshUI(const std::string& n, AssetManagerUI* m): _name(n), _manager(m) {}
        virtual ~CreateMeshUI(){}

    protected:

        AssetManagerUI* _manager;
        std::string _name;

        ui::Text _text;
        TextFieldMode _text_input_mode;
        
        ui::UID _uid;
    };
}
#endif /* create_mesh_ui_hpp */
