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
    //typedef std::map< int, ComponentAddFn > ComponentAdderFactory;


    class InspectorPanel : public kege::ui::Panel
    {
    public:

        struct CompInfo
        {
            ui::InspectorComponentUIBuilder buildUI;
            ComponentAddFn addComponent;
            std::string name;
            int32_t type;
            size_t index;
        };
        typedef std::map< int, uint32_t > ComponentAdderFactory;
        //typedef std::map< int, ui::InspectorComponentUIBuilder > ComponentBuilderFactory;


        void operator()(const kege::ui::DragDropOffAssetMetadata& event);

        InspectorPanel( kege::EditorLayer* editor );
        int select( const std::vector< std::string >& options );
        void operator()( const ui::SetSelectedEntity& msg );
        void update();

        void updateAddComponent(int16_t layer);
        void updateComponents(int16_t layer);
        void updateUI(int16_t layer, int32_t comp_type, const ecs::Component::Layout& layout, kege::AssetManager* am);
        void updateRemoveComponent();

        uint64_t getCompKey(int comp_type);


        void initSelectListElements();
//        uint32_t stringToCompType(const std::string& name);

    public:

        void registerAddComponentAction
        (
            const char* stype,
            int comp_type,
            ComponentAddFn component_add_fn,
            ui::InspectorComponentUIBuilder buildUI
        );

        kege::ui::Style _styles[5];

//        std::map< std::string, int > _string_to_component_type;
        ComponentAdderFactory _component_adder_registry;
        
        std::vector< int > _deleted_components;
        std::vector< ListElem > _listed_component;
        std::vector< CompInfo > _infos;

        ui::WidgetHandle _add_component;
        ui::Tree _tree;


        kege::ECS* _ecs;
        ui::WidgetHandle _containers[2];
        float _scroll_amount;

        //std::vector< ui::WidgetHandle > _selection_elements;
        ui::WidgetHandle _selection_container;
        ui::Text _text_add;

        ecs::Entity _selected_entity;
        bool _show_component_selections;
    };
}

#endif /* inspector_panel_hpp */
