//
//  viewport-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef viewport_panel_hpp
#define viewport_panel_hpp

#include "../core/engine/engine.hpp"
#include "../core/gui/ui-input.hpp"
#include "../core/gui/ui-viewer.hpp"
#include "../core/gui/ui-layout.hpp"
#include "../core/gui/ui-font-creator.hpp"

namespace kege{

    class ViewportPanel
    {
    public:

        ViewportPanel& init( Engine* engine, ui::Layout& layout );
        void put( ui::Layout& layout );
        ViewportPanel();

    public:

        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* viewport_panel_hpp */
