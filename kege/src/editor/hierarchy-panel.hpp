//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "../core/engine/engine.hpp"
#include "../core/gui/ui-input.hpp"
#include "../core/gui/ui-viewer.hpp"
#include "../core/gui/ui-layout.hpp"
#include "../core/gui/ui-font-creator.hpp"
#include "ui-elements/hierarchy-droplist.hpp"

namespace kege{

    class HierarchyPanel
    {
    public:
        
        HierarchyPanel& init( Engine* engine, ui::Layout& layout );
        void build( const Entity& root, ui::Layout& layout, int spacer = 0 );
        void put( ui::Layout& layout );

        Entity getSelectedEntity();
        HierarchyPanel();

    public:

        std::map< uint32_t, ui::HierarchyDroplist > _open_states;

        Engine* _engine;
        Entity _selected_entity;
        ui::EID _main;
        ui::EID _highlight;
    };
}

#endif /* hierarchy_panel_hpp */
