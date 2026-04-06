//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "../../dock/ui-dock-manager.hpp"
#include "elements/ui-mesh.hpp"
#include "elements/ui-camera.hpp"
#include "elements/ui-light.hpp"
#include "elements/ui-rigidbody.hpp"
#include "elements/ui-transform.hpp"
#include "elements/ui-particle-effect.hpp"
#include "inspector-panel.hpp"

#include "ui-components.hpp"

namespace kege{

    void InspectorPanel::updateLayout( int16_t layer )
    {
        updateRemoveComponent();
        if ( _selected_entity )
        {
            updateAddComponent(layer);
            updateComponents(layer);
        }
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
        if( _gui->button(_uid_main[7], layer, _text_add) )
        {
            _show_component_selections = !_show_component_selections;
        }

        if ( _show_component_selections )
        {
            int selection;
            if ( _gui->select(_uid_selection, layer, _listed_component, selection) )
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
        kege::AssetManager* am = _manager->getEditor()->getProjectManager()->getAssetManager().ref();
        const ecs::Component::Layout& layout = _ecs->getLayout( _selected_entity );

        ui::ID id[2] = {_uid_main[0], _uid_main[2]};
        _gui->beginScrollContainer(id, layer);
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

        _gui->push({ .layer = layer, .style = &_gui->theme().card });
        ui::ID id[4] = {info.uid[0], info.uid[1], info.uid[2], info.uid[3]};
        switch ( _gui->removableHeader(id, layer, _listed_component[info.index]) )
        {
            case 1: // update component
            {
                info.buildUI( info.uid_comp, layer, am, _gui, _ecs, _selected_entity );
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

    void InspectorPanel::handle(const kege::ui::AssetMetadataDropOff& event)
    {
        if( !_selected_entity ) return;
        for(const auto& a : event.handle)
        {
            switch (a->type)
            {
                case kege::ui::AssetType::MODEL:
                {
                    kege::Renderable* renderable = _ecs->get< kege::Renderable >( _selected_entity );
                    if (renderable == nullptr) {
                        renderable = _ecs->add< kege::Renderable >( _selected_entity );
                    }
                    renderable->mesh_handle = event.handle[0]->handle;
                    strncpy(renderable->fname, event.handle[0]->name.c_str(), kege::min<uint64_t>(63, event.handle[0]->name.length()));
                    break;
                }

                case kege::ui::AssetType::MATERIAL:
                {
                    break;
                }

                case kege::ui::AssetType::TEXTURE:
                    break;
                case kege::ui::AssetType::SHADER:
                    break;
                case kege::ui::AssetType::SOUND:
                    break;
                case kege::ui::AssetType::SCRIPT:
                    break;
                case kege::ui::AssetType::FONT:
                    break;

                default: break;
            }
        }
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

    uint64_t InspectorPanel::getCompKey(int comp_type)
    {
        return (uint64_t(_selected_entity.index) << 32) | comp_type;
    }

    void InspectorPanel::initSelectListElements()
    {
        _listed_component.resize(_infos.size());
        for (int i=0; i<_infos.size(); ++i)
        {
            _listed_component[i] = _gui->layout()->text(_infos[i].name.c_str(), 20);
        }
    }

    InspectorPanel::InspectorPanel( kege::ui::DockManager* dm )
    :   kege::ui::Panel( "Properties", dm )
    ,   _show_component_selections( false )
    ,   _selected_entity{}
    ,   _scroll_amount( 0.f )
    ,   _ecs(dm->getEditor()->getECS())
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

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Camera >(e); ecs->add< kege::Perspective >(e); };
        registerAddComponentAction("PerspCamera", ecs::Component::type< kege::Perspective >(), component_add_fn, ui::perspectiveCamera);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Camera >(e); ecs->add< kege::Orthographic >(e); };
        registerAddComponentAction("OrthoCamera", ecs::Component::type< kege::Orthographic >(), component_add_fn, ui::orthographicCamera);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::Renderable >(e); };
        registerAddComponentAction("Renderable", ecs::Component::type< kege::Renderable >(), component_add_fn, ui::renderable);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::DirectionalLight >(e); };
        registerAddComponentAction("DirectionalLight", ecs::Component::type< kege::DirectionalLight >(), component_add_fn, ui::directionalLight);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::PointLight >(e); };
        registerAddComponentAction("PointLight", ecs::Component::type< kege::PointLight >(), component_add_fn, ui::pointLight);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::SpotLight >(e); };
        registerAddComponentAction("SpotLight", ecs::Component::type< kege::SpotLight >(), component_add_fn, ui::spotLight);

        component_add_fn = [](kege::ECS* ecs, ecs::Entity& e){ ecs->add< kege::ParticleEffect >(e); };
        registerAddComponentAction("Particle Effect", ecs::Component::type< kege::ParticleEffect >(), component_add_fn, ui::particleEffect);

        initSelectListElements();
    }

}
