//
//  viewport-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef viewport_panel_hpp
#define viewport_panel_hpp

#include "../ui-panel.hpp"

namespace kege{

    class ViewportPanel : public ui::Panel
    {
    public:

        ViewportPanel( kege::ui::DockManager* dm );
        void updateLayout();

    public:

        kege::ui::Style _style;
        ui::WidgetHandle _main;
    };
}

#endif /* viewport_panel_hpp */
