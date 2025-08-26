//
//  inspector-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef inspector_panel_hpp
#define inspector_panel_hpp

#include "droplist.hpp"
#include "ui-numeric3.hpp"

namespace kege{

    class InspectorPanel
    {
    public:

        InspectorPanel& init( Engine* engine, ui::Layout& layout );
        void put( ui::Layout& layout );
        InspectorPanel();

    public:

        Droplist droplist;
        ui::Numeric3 transform[3];
        Entity _selected_entity;
        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* inspector_panel_hpp */
