//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../../editor-layer.hpp"
#include "ui-nav-bar.hpp"

namespace kege::ui{

    NavBar::NavBar
    (
        const kege::ui::Quad& quad,
        kege::GUI* gui,
        kege::UI* ui,
        kege::ProjectManager* pm,
        kege::ECS* ecs
    )
    :   _quad( quad )
    ,   _gui( gui )
    ,   _ui( ui )
    {
        _selected = -1;
        _expand = false;
        _tabs.push_back({0,0, 100, 20, 0xFFFFFFFF, "File"});
        _tabs.push_back({0,0, 100, 20, 0xFFFFFFFF, "Window"});
        _tabs.push_back({0,0, 100, 20, 0xFFFFFFFF, "Settings"});

        _menus.push_back
        ({
            .text = {0,0, 100, 20, 0xFFFFFFFF, "File"},
            .elements =
            {
                {{0,0, 100, 20, 0xFFFFFFFF, "New"}},
                {{0,0, 100, 20, 0xFFFFFFFF, "Open"}},
                {{0,0, 100, 20, 0xFFFFFFFF, "Save"}},
                {{0,0, 100, 20, 0xFFFFFFFF, "Quit"}},
            }
        });
        _menus.push_back
        ({
            .text = {0,0, 100, 20, 0xFFFFFFFF, "Window"},
            .elements =
            {
                {{0,0, 100, 20, 0xFFFFFFFF, "Viewport", }},
                {{0,0, 100, 20, 0xFFFFFFFF, "Hierarchy"}},
                {{0,0, 100, 20, 0xFFFFFFFF, "AssetManager"}},
            }
        });
        _menus.push_back
        ({
            .text = {0,0, 100, 20, 0xFFFFFFFF, "Settings"},
            .elements = {}
        });
    }

    void NavBar::update()
    {
        _ui->push
        ({
            .wid = _ui->newElem
            ({
                .width = kege::ui::extend(),
                .quad_color = 0x282b3538,
                .padding = {2,2,2,2},
                .alignment =
                {
                    .type = kege::ui::AlignType::LIST,
                    .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .direction = {kege::ui::AlignDir::DOWN, kege::ui::AlignDir::NILL},
                    //.items = kege::ui::AlignItem::START,
                },
                .corner_curves = {8,8,8,8},
            }),
            .quad = _quad,
        });
        updateMenu();
        _ui->pop();
    }

    void defOp(kege::GUI* gui, ui::ID user_id, ui::NodeId node_id, void* data)
    {
        //NavBar* param = (NavBar*)data;
    }

    void NavBar::updateMenu()
    {
//        bool active = false;
//        _ui->push({ .style = &_ui->theme()->tab });
//        for (int i=0; i<_menus.size(); ++i)
//        {
//            display( _menus[i] );
//        }
//        _gui->pop();


        _ui->tab(_uid, _tabs, _selected);
        _ui->pushLayer( ui::LAYER_BASE_OVERLAY );
        if(0 <= _selected && _selected < _tabs.size())
        {
            _ui->beginRoot();
            _ui->push
            ({
                .wid = _ui->newElem
                ({
                    .width = kege::ui::fixed(250),
                    .height = kege::ui::flexible(),
                    .quad_color = 0x282b3538,
                    .padding = {2,2,2,2},
                    .alignment =
                    {
                        .type = kege::ui::AlignType::LIST,
                        .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                        .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                        .direction = {kege::ui::AlignDir::DOWN, kege::ui::AlignDir::NILL},
                        //.items = kege::ui::AlignItem::START,
                    },
                    .corner_curves = {8,8,8,8},
                })
            });
            display( _menus[ _selected ] );
            _ui->endRoot();
            //_ui->pushDeferredOp(const ui::ID &id, ui::NodeId index, ui::DeferredOperation fn, const Params &params);
        }
        _ui->popLayer();
    }

    void NavBar::display( ui::MenuElem& menu )
    {
        for (int index = 0; index < menu.elements.size(); ++index)
        {
            ui::MenuElem& elem = menu.elements[ index ];
            _ui->push
            ({
                .user_id = elem.uid[0],
                .wid = _ui->newElem((elem.hot) ? _ui->theme()->tab_elem_focus : _ui->theme()->tab_elem),
                .single_click = ui::ClickTrigger::OnRelease,
            });
            _ui->text( elem.text );
            if (elem.elements.size())
            {
                _ui->label({.data = ">"});
            }
            _ui->pop();
            if( _gui->mouseover( elem.uid[0] ) )
            {
                elem.hot = false;
            }
        }

        if( _gui->click( menu.uid[0] ) )
        {
            //_selected = id;
        }

        for (int index = 0; index < menu.elements.size(); ++index)
        {
            display( menu.elements[ index ] );
        }
    }
//    void NavBar::display( const MenuElem& menu, int id )
//    {
//        _ui->push
//        ({
//            .alignment =
//            {
//                .type = kege::ui::AlignType::LIST,
//                .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
//                .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
//                .direction = kege::ui::AlignDir::RIGHT,
//                .items = kege::ui::AlignItem::START,
//                .wrap = { false }
//            },
//            .color = 0x282b350F,
//            .width = ui::flexible(),
//            .height = ui::fixed(200),
//        });
//        _ui->text( menu.text );
//        if (menu.elements.size())
//        {
//            _ui->label({">"});
//        }
//        _ui->pop();
//    }

}
