//
//  inspector-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef inspector_panel_hpp
#define inspector_panel_hpp

#include "ui-elements/droplist.hpp"
#include "ui-elements/ui-numeric3.hpp"
#include "ui-elements/ui-numeric4.hpp"
#include "hierarchy-panel.hpp"

namespace kege{

    class InspectorPanel
    {
    public:

        InspectorPanel& init( Engine* engine, ui::Layout& layout );
        void operator()( const SetSelectedEntity& msg );
        void put( ui::Layout& layout );
        InspectorPanel();

    public:

        std::map<void*, Droplist> _droplists;
        Entity _selected_entity;
        Engine* _engine;
        ui::EID _main;
    };
}

#endif /* inspector_panel_hpp */
