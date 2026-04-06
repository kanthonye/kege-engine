//
//  create-asset-mesh-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#include "create-asset-mesh-ui.hpp"
#include "../mesh/create-cubiod-ui.hpp"
#include "../mesh/create-cylinder-ui.hpp"
#include "../mesh/create-ellipsoid-ui.hpp"
#include "../mesh/create-grid-ui.hpp"

namespace kege::ui{

    std::vector< Ref< CreateMeshUI > > CreateAssetMeshUI::_create_mesh_uis;
    std::vector< kege::ui::Text > CreateAssetMeshUI::_shape_labels;

    CreateAssetMeshUI::CreateAssetMeshUI(AssetManagerUI* m,kege::GUI* g)
    : AssetManagerModule(m,g), _selection(0)
    {
        if (_shape_labels.empty())
        {
            /**
             * TODO: This is a bit hacky, but it works for now. We need to find a better way to manage 
             * the creation of mesh UIs. Maybe we can use a factory pattern or something similar.
             */
            _create_mesh_uis.push_back(new CreateCuboidMeshUI(_manager));
            _create_mesh_uis.push_back(new CreateCylinderMeshUI(_manager));
            _create_mesh_uis.push_back(new CreateEllipsoidMeshUI(_manager));
            _create_mesh_uis.push_back(new CreateGridMeshUI(_manager));

            // Generate labels for the UI
            _shape_labels.resize(_create_mesh_uis.size());
            for (int i=0; i<_create_mesh_uis.size(); ++i)
            {
                _shape_labels[i] = _gui->layout()->text(_create_mesh_uis[i]->getName().c_str(), 20);
                _shape_labels[i].color = 0xFFFFFF70;
            }
        }

        _id_offset = (uint32_t) _create_mesh_uis.size();
    }

    void CreateAssetMeshUI::operator()(const std::string& type, void* data)
    {
        if (type == "OpenCreateMeshWindow")
        {
            _open_window = true;
            _rect.width = 600;
            _rect.height = 400;
            _rect.x = (_gui->layout()->getWidth() - _rect.width) * 0.5;
            _rect.y = (_gui->layout()->getHeight() - _rect.height) * 0.5;
        }
    }
    
    void CreateAssetMeshUI::update()
    {
        if (!_open_window) return;

        uint16_t border_radius = 6;
        _open_window = true;
        _rect.width = 600;
        _rect.height = 400;
        _rect.x = (_gui->layout()->getWidth() - _rect.width) * 0.5;
        _rect.y = (_gui->layout()->getHeight() - _rect.height) * 0.5;

        // Generate the outter container for the create mesh window
        _gui->push
        ({
            .layer = 2,
            .rect = _rect,
            .position = Positioning::Independent,
            .color = 0x18141D00,
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            }
        });
        {
            // The selection panel for the different mesh types
            _gui->push
            ({
                .layer = 2,
                .border.corner_curves = {border_radius,0,0,border_radius},
                .rect = {0,0,200,400},
                .color = 0x202020FF,
                .gap = {2,5},
                .padding = {10,10,0,10},
                .alignment =
                {
                    .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                    .direction = ui::AlignDir::DOWN,
                    .items = ui::AlignItem::END,
                }
            });
            {
                for (int i=0; i<_shape_labels.size(); ++i)
                {
                    ui::WidgetId widget_id = _gui->put
                    ({
                        .layer = 2,
                        .user_id = _uid[i],
                        .text = _shape_labels[i],
                        .single_click = ui::ClickTrigger::OnRelease,
                        .rect = {0,0, ((_selection != i) ? 180.f : 200.f), 30},
                        .color = 0xFFFFFF08,
                        .padding = {10,5,5,10},
                    });
                    if( _gui->click( _uid[i] ) )
                    {
                        _selection = i;
                        _gui->get( widget_id )->rect.width = 200;
                    }
                    if( _gui->mouseover( _uid[i] ) )
                    {
                        _gui->get( widget_id )->color = 0xFFFFFF12;
                    }
                }
            }
            _gui->pop();

            // The panel for the selected mesh type
            _gui->push
            ({
                .layer = 2,
                .border.corner_curves = {0,border_radius,border_radius,0},
                .rect = {0,0,400,400},
                .color = 0x18141DFF,
                .padding = {10,10,10,10},
                .alignment =
                {
                    .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                    .direction = ui::AlignDir::DOWN,
                }
            });
            {
                _gui->push
                ({
                    .layer = 2,
                    .border.corner_curves = {border_radius,0,0,border_radius},
                    .width = extend(),
                    .height = fixed(40),
                    .color = 0x20202000,
                    .alignment =
                    {
                        .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                        .direction = ui::AlignDir::RIGHT,
                        .items = ui::AlignItem::CENTER,
                    }
                });
                {
                    // the title and name of the mesh being created
                    _gui->put
                    ({
                        .layer = 2,
                        .text = ui::Text
                        {
                            .width = 100,
                            .height = 24,
                            .color = 0xFFFFFF30,
                            .font_size = 30,
                            .ptr = "Create"
                        },
                        .single_click = ui::ClickTrigger::OnRelease,
                        .rect = {0,0, 70, 40},
                        .color = 0xFFFFFF00,
                        .padding = {10,5,5,10},
                    });
                    _gui->put
                    ({
                        .layer = 2,
                        .text = ui::Text
                        {
                            .width = 100,
                            .height = 24,
                            .color = 0xFFFFFF30,
                            .font_size = 30,
                            .ptr = _create_mesh_uis[_selection]->getName().c_str()
                        },
                        .single_click = ui::ClickTrigger::OnRelease,
                        .rect = {0,0, 100, 40},
                        .color = 0xFFFFFF00,
                        .padding = {10,5,5,10},
                    });
                    _gui->put({.style = &_gui->theme().x_seperator});
                    _gui->charButn(_uid[ _id_offset ], "x", 7, 0);
                }
                _gui->pop();

                // the UI for creating the selected mesh type
                if( _create_mesh_uis[_selection]->create(_gui) )
                {
                    _open_window = false;
                }
                // the UI for creating the selected mesh type
                if( _gui->click(_uid[ _id_offset ]) )
                {
                    _open_window = false;
                }
            }
            _gui->pop();
        }
        _gui->pop();
    }

    CreateAssetMeshUI::~CreateAssetMeshUI()
    {
        _create_mesh_uis.clear();
    }
}

