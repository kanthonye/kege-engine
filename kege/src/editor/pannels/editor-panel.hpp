//
//  editor-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef editor_panel_hpp
#define editor_panel_hpp

#include "ui-tree-node.hpp"
#include "../../core/app/project-manager.hpp"

namespace kege{

    class EditorPanel : public kege::RefCounter
    {
    public:

        virtual void update() = 0;

        EditorPanel( kege::ProjectManager* pm, ui::Layout* l )
        :   _project_manager( pm )
        ,   _layout( l )
        {}

        virtual ~EditorPanel(){}

        kege::ProjectManager* _project_manager;
        ui::Layout* _layout;
    };
    
}

namespace kege::ref{
    typedef kege::Ref< kege::EditorPanel > EditorPanel;
}
#endif /* editor_panel_hpp */
