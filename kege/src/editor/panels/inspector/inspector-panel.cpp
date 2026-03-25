//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "elements/ui-mesh.hpp"
#include "elements/ui-camera.hpp"
#include "elements/ui-light.hpp"
#include "elements/ui-rigidbody.hpp"
#include "elements/ui-transform.hpp"
#include "elements/ui-particle-effect.hpp"
#include "inspector-panel.hpp"

namespace kege{

    InspectorPanel::InspectorPanel( kege::EditorLayer* editor )
    :   kege::ui::Panel( "Properties", editor )
    ,   _show_component_selections( false )
    ,   _selected_entity{}
    ,   _scroll_amount( 0.f )
    ,   _ecs(editor->getECS())
    {
        _text_add = _gui->layout()->text("Add Component", 20);

        _styles[1] = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10,10,10,10},
            .gap = {2,2},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN
            }
        };

        Communication::add< const ui::SetSelectedEntity&, InspectorPanel >( this );

        ComponentAddFn component_add_fn;

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Transform >(e); };
        registerAddComponentAction("Transform", ecs::Component::type< kege::Transform >(), component_add_fn, ui::transform);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Rigidbody >(e); };
        registerAddComponentAction("Rigidbody", ecs::Component::type< kege::Rigidbody >(), component_add_fn, ui::rigidbody);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){
            ecs->add< kege::Camera >(e);
            ecs->add< kege::Perspective >(e);
        };
        registerAddComponentAction("Camera", ecs::Component::type< kege::Camera >(), component_add_fn, ui::camera);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Renderable >(e); };
        registerAddComponentAction("Renderable", ecs::Component::type< kege::Renderable >(), component_add_fn, ui::mesh);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Light >(e); };
        registerAddComponentAction("Light", ecs::Component::type< kege::Light >(), component_add_fn, ui::light);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::ParticleEffect >(e); };
        registerAddComponentAction("Particle Effect", ecs::Component::type< kege::ParticleEffect >(), component_add_fn, ui::particleEffect);

        initSelectListElements();
    }

    void InspectorPanel::operator()(const kege::ui::DragDropOffAssetMetadata& event)
    {
    }

    void InspectorPanel::registerAddComponentAction
    (
        const char* stype,
        int comp_type,
        ComponentAddFn add_funct,
        ui::InspectorComponentUIBuilder build_funct
    )
    {
        _infos.push_back
        ({
            .addComponent = add_funct,
            .buildUI = build_funct,
            .type = comp_type,
            .name = stype
        });

        uint32_t index = static_cast<uint32_t>((_infos.size() - 1));
        _component_adder_registry[ comp_type ] = index;
        _infos[ index ].index = index;
    }

    void InspectorPanel::operator()( const ui::SetSelectedEntity& msg )
    {
        _selected_entity = msg.entity;
    }

    void InspectorPanel::update()
    {
        int16_t layer = 0;
        updateRemoveComponent();
        _gui->push({ .layer = layer, .style = &_gui->_theme.panel});
        if ( _selected_entity )
        {
            updateAddComponent(layer);
            updateComponents(layer);
        }
        _gui->pop();

    }

    void InspectorPanel::updateRemoveComponent()
    {
        for(int i=0; i<_deleted_components.size(); ++i)
        {
            _ecs->remove( _deleted_components[ i ], _selected_entity );
        }
        _deleted_components.clear();
    }

    void InspectorPanel::updateAddComponent(int16_t layer)
    {
        if( _gui->button(layer, _add_component, _text_add) )
        {
            _show_component_selections = !_show_component_selections;
        }

        if ( _show_component_selections )
        {
            int selection;
            if ( _gui->select(layer, &_styles[1], _listed_component, selection) )
            {
                if (selection < _infos.size())
                {
                    _infos[selection].addComponent( _ecs, _selected_entity );
                    _show_component_selections = false;
                }
            }
        }
    }

    void InspectorPanel::updateComponents(int16_t layer)
    {
        kege::AssetManager* am = _editor->getProjectManager()->getAssetManager().ref();
        const ecs::Component::Layout& layout = _ecs->getLayout( _selected_entity );
        
        _gui->beginScrollContainer(layer, _gui->getAddressAsInt(this));
        for (int i = 0; i < layout.attributes.size(); ++i)
        {
            updateUI( layer, layout.attributes[i].info->type, layout, am );
        }
        _gui->endScrollContainer();
    }

    void InspectorPanel::updateUI(int16_t layer, int32_t comp_type, const ecs::Component::Layout& layout, kege::AssetManager* am)
    {
        auto itr = _component_adder_registry.find( comp_type );
        if ( itr == _component_adder_registry.end() )
        {
            return;
        }

        const CompInfo& info = _infos[ itr->second ];

        _gui->push({ .layer = layer, .style = &_gui->_theme.card });
        switch ( _gui->removableHeader(layer, getCompKey(comp_type), _listed_component[info.index].text) )
        {
            case 1: // update component
            {
                info.buildUI( am, layer, _gui, _ecs, _selected_entity );
            }
            break;

            case 2: // remove component
            {
                _deleted_components.push_back(comp_type);
            }
            break;

            default: break;
        }
        _gui->pop();
    }

    uint64_t InspectorPanel::getCompKey(int comp_type)
    {
        return (uint64_t(_selected_entity.index) << 32) | comp_type;
    }

    void InspectorPanel::initSelectListElements()
    {
        _listed_component.resize(_infos.size());
        for (int i=0; i<_infos.size(); ++i)
        {
            _listed_component[i].text = _gui->layout()->text(_infos[i].name.c_str(), 20);
        }
    }
}
