//
//  hierarchy-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef hierarchy_panel_hpp
#define hierarchy_panel_hpp

#include "editor-panel.hpp"
#include "../elements/hierarchy-droplist.hpp"

namespace kege{

    struct SetSelectedEntity
    {
        Entity entity;
    };

    class HierarchyPanel : public kege::EditorPanel
    {
    public:
        
        HierarchyPanel& init( ui::Layout& layout );
        void build( const Entity& root, ui::Layout& layout, int spacer = 0 );
        void put( const Entity& entity, ui::Layout& layout );

        Entity getSelectedEntity();
        HierarchyPanel( kege::ProjectManager* pm, ui::Layout* l );

    public:

        std::map< uint32_t, ui::HierarchyDroplist > _open_states;

        Entity _selected_entity;
        ui::EID _main;
        ui::EID _highlight;
    };
}

#endif /* hierarchy_panel_hpp */
