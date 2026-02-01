//
//  inspector-panel.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#ifndef inspector_panel_hpp
#define inspector_panel_hpp

#include "ui-panel.hpp"
#include "hierarchy-panel.hpp"

namespace kege{

    typedef void(*ComponentAddFn)( kege::ECS*, ecs::Entity& );
    typedef std::map< int, ComponentAddFn > ComponentAdderFactory;


    class InspectorPanel : public kege::ui::Panel
    {
    public:

        int select( const std::vector< std::string >& options );
        InspectorPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs );
        void operator()( const ui::SetSelectedEntity& msg );
        void update();

    public:

        void registerAddComponentAction(const char* stype, int component_type, ComponentAddFn component_add_fn);

        kege::ui::Style _styles[5];

        std::vector<ListElem> _component_uis;
        std::vector<std::string> _component_type_names;

        std::map< std::string, int > _string_to_component_type;

        ComponentAdderFactory _component_adder_registry;
        ui::ComponentBuilderFactory _component_builder_registry;
        ui::UID _add_component;
        ui::Tree _tree;


        ui::UID _containers[2];
        float _scroll_amount;

        std::vector< ui::UID > _selection_elements;
        ui::UID _selection_container;
        ui::Text _text_add;

        ecs::Entity _selected_entity;
        bool _show_component_selections;
    };
}

#endif /* inspector_panel_hpp */
