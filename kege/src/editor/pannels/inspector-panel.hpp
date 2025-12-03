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
#include "../elements/ui-panel.hpp"
#include "hierarchy-panel.hpp"

namespace kege{

    typedef std::map< int, void(*)( kege::ECS*, ecs::Entity& ) > EntityComponentFactory;

    class InspectorPanel : public kege::ui::Panel
    {
    public:

        int select( ui::Layout* layout, const std::vector< std::string >& options );
        InspectorPanel( kege::ProjectManager* pm, ui::Layout* l, kege::ECS* ecs );
        void operator()( const SetSelectedEntity& msg );
        void update();

    public:

        void add(const std::string& stype, int component_type, void(*funct)( kege::ECS*, ecs::Entity& ));

        std::map< std::string, int > _string_to_component_type;
        std::vector< std::string > _component_string_types;

        EntityComponentFactory _entity_component_factory;
        ui::UIElementCreator _ui_element_creator;
        ui::Elem _add_component;
        ui::Tree _tree;
        ui::Elem _main;
        
        std::vector< ui::Elem > _selection_elements;
        ui::Elem _selection_container;

        ecs::Entity _selected_entity;
        bool _show_component_selections;
    };
}

#endif /* inspector_panel_hpp */
