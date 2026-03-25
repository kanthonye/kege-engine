//
//  ui-panel.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_panel_hpp
#define ui_panel_hpp

#include "../common/dependencies.hpp"

namespace kege{
    class EditorLayer;
}

namespace kege::ui{

    struct Panel : public kege::RefCounter
    {
        virtual void operator()(const kege::ui::DragDropOffAssetMetadata& event){}
        const std::string getName()const{ return _name; }
        kege::EditorLayer* getEditor(){ return _editor; }
        virtual void update() = 0;

        Panel( const std::string& name, kege::EditorLayer* e );
        virtual ~Panel(){}

        kege::EditorLayer* _editor;
        kege::GUI* _gui;
        std::string _name;
    };

}
#endif /* ui_panel_hpp */
