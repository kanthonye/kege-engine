//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "ui-camera.hpp"
#include "ui-rigidbody.hpp"
#include "ui-transform.hpp"
#include "inspector-panel.hpp"

namespace kege{

    InspectorPanel::InspectorPanel( kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   kege::ui::Panel( "Properties", pm, gui, e )
    ,   _show_component_selections( false )
    ,   _selected_entity{}
    ,   _scroll_amount( 0.f )
    {
        _text_add = gui->layout()->text("Add Component", 20);

        _styles[1] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };

        Communication::add< const ui::SetSelectedEntity&, InspectorPanel >( this );

        _component_builder_registry[ ecs::Component::type< kege::Camera >()    ] = ui::camera;
        _component_builder_registry[ ecs::Component::type< kege::Rigidbody >() ] = ui::rigidbody;
        _component_builder_registry[ ecs::Component::type< kege::Transform >() ] = ui::transform;
        _component_builder_registry[ ecs::Component::type< kege::Renderable >()  ] = ui::mesh;
        _component_builder_registry[ ecs::Component::type< kege::Light >()     ] = ui::light;
        _component_builder_registry[ ecs::Component::type< kege::ParticleEffect >()     ] = ui::particleEffect;

        ComponentAddFn component_add_fn;

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Transform >(e); };
        registerAddComponentAction("Transform", ecs::Component::type< kege::Transform >(), component_add_fn);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Rigidbody >(e); };
        registerAddComponentAction("Rigidbody", ecs::Component::type< kege::Rigidbody >(), component_add_fn);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ecs->add< kege::Camera >(e)->projection = new Perspective();};
        registerAddComponentAction("Camera", ecs::Component::type< kege::Camera >(), component_add_fn);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Renderable >(e); };
        registerAddComponentAction("Mesh", ecs::Component::type< kege::Renderable >(), component_add_fn);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Light >(e); };
        registerAddComponentAction("Light", ecs::Component::type< kege::Light >(), component_add_fn);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){
            ecs->add< kege::ParticleEffect >(e);
        };
        registerAddComponentAction("Light", ecs::Component::type< kege::ParticleEffect >(), component_add_fn);
    }

    void InspectorPanel::registerAddComponentAction(const char* stype, int component_type,  ComponentAddFn component_add_fn)
    {
        _component_adder_registry[ component_type ] = component_add_fn;
        _string_to_component_type[ stype ] = component_type;

        _component_uis.push_back({});
        _component_type_names.push_back(stype);
        ListElem& elem = _component_uis[ _component_uis.size() - 1 ];
        elem.text = _gui->layout()->text(stype, 20);
    }

    void InspectorPanel::operator()( const ui::SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }

    void InspectorPanel::update()
    {
        int16_t layer = 0;
        _gui->push({ .layer = layer, .style = &_gui->_theme.panel});
        if ( _selected_entity )
        {
            if( _gui->button(layer, _add_component, _text_add) )
            {
                _show_component_selections = !_show_component_selections;
            }

            if ( _show_component_selections )
            {
                int selection;
                if ( _gui->select(layer, &_styles[1], _component_uis, selection) )
                {
                    auto i = _string_to_component_type.find( _component_type_names[ selection ] );
                    if (i != _string_to_component_type.end())
                    {
                        _component_adder_registry[ i->second ]( _ecs, _selected_entity );
                        _show_component_selections = false;
                    }
                }
            }

            const ecs::Component::Layout& layout = _ecs->getLayout( _selected_entity );
            kege::AssetManager* am = _project_manager->getAssetManager().ref();
            _gui->beginScrollContainer(layer, _gui->getAddressAsInt(this));
            for (int i = 0; i < layout.attributes.size(); ++i)
            {
                auto m = _component_builder_registry.find( layout.attributes[i].info->type );
                if ( m != _component_builder_registry.end() )
                {
                    m->second( am, layer, _gui, _ecs, _selected_entity );
                }
            }
            _gui->endScrollContainer();
        }
        _gui->pop();

    }

}
