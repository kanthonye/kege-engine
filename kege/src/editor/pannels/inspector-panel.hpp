//
//  inspector-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef inspector_panel_hpp
#define inspector_panel_hpp

#include "../elements/droplist.hpp"
#include "../elements/ui-numeric.hpp"
#include "ui-panel.hpp"
#include "hierarchy-panel.hpp"

namespace kege{

    typedef std::map< int, void(*)( kege::ECS*, ecs::Entity& ) > EntityComponentFactory;

    class InspectorPanel : public kege::ui::Panel
    {
    public:

        int select( const std::vector< std::string >& options );
        InspectorPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs );
        void operator()( const SetSelectedEntity& msg );
        void update();

    public:

        void add(const std::string& stype, int component_type, void(*funct)( kege::ECS*, ecs::Entity& ));

        kege::ui::Style _styles[5];
        
        std::vector<std::pair<kege::UID, std::string>> _component_uis;

        std::map< std::string, int > _string_to_component_type;
        //std::vector< std::string > _component_string_types;

        EntityComponentFactory _component_factory;
        ui::UIElementCreator _ui_element_creator;
        UID _add_component;
        ui::Tree _tree;
        UID _main;
        
        std::vector< UID > _selection_elements;
        UID _selection_container;

        ecs::Entity _selected_entity;
        bool _show_component_selections;
    };
}

#endif /* inspector_panel_hpp */
