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

    class CreateMeshUI : public kege::RefCounter
    {
    public:
        
        const std::string& getName()const{ return _name;}
        virtual bool create(GUI* gui) = 0;

        CreateMeshUI(const std::string& n): _name(n) {}
        virtual ~CreateMeshUI(){}

    private:

        std::string _name;
    };
}
#endif /* create_mesh_ui_hpp */
