//
//  viewport-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef viewport_panel_hpp
#define viewport_panel_hpp

#include "editor-panel.hpp"

namespace kege{

    class ViewportPanel : public EditorPanel
    {
    public:

        ViewportPanel( kege::ProjectManager* pm, ui::Layout* l );
        void update();

    public:

        ui::EID _main;
    };
}

#endif /* viewport_panel_hpp */
