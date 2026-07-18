//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../editor-layer.hpp"
#include "ui-nav-bar.hpp"

namespace kege::ui{

    NavBar::NavBar
    (
        const kege::ui::Rect& rect,
        kege::GUI* gui,
        kege::UI* ui,
        kege::ProjectManager* pm,
        kege::ECS* ecs
    )
    :   _rect( rect )
    ,   _gui( gui )
    ,   _ui( ui )
    {
        _selected = -1;
        _expand = false;
        _tabs.push_back({"File", 0,0, 100, 20, 20, 0xFFFFFFFF});
        _tabs.push_back({"Window", 0,0, 100, 20, 20, 0xFFFFFFFF});
        _tabs.push_back({"Settings", 0,0, 100, 20, 20, 0xFFFFFFFF});

        _menus.push_back
        ({
            .text = {"File", 0,0, 100, 20, 20, 0xFFFFFFFF},
            .elements =
            {
                {{"New", 0,0, 100, 20, 20, 0xFFFFFFFF}},
                {{"Open", 0,0, 100, 20, 20, 0xFFFFFFFF}},
                {{"Save", 0,0, 100, 20, 20, 0xFFFFFFFF}},
                {{"Quit", 0,0, 100, 20, 20, 0xFFFFFFFF}},
            }
        });
        _menus.push_back
        ({
            .text = {"Window", 0,0, 100, 20, 20, 0xFFFFFFFF},
            .elements =
            {
                {{"Viewport", 0,0, 100, 20, 20, 0xFFFFFFFF}},
                {{"Hierarchy", 0,0, 100, 20, 20, 0xFFFFFFFF}},
                {{"AssetManager", 0,0, 100, 20, 20, 0xFFFFFFFF}},
            }
        });
        _menus.push_back
        ({
            .text = {"Settings", 0,0, 100, 20, 20, 0xFFFFFFFF},
            .elements = {}
        });
    }

    void NavBar::update()
    {
        _ui->push
        ({
            .rect = _rect,
            .border = {.corner_curves = {8,8,8,8}},
            .padding = {2,2,2,2},
            .alignment =
            {
                .type = kege::ui::AlignType::LIST,
                .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                .direction = kege::ui::AlignDir::DOWN,
                .items = kege::ui::AlignItem::START,
                .wrap = { false }
            },
            .color = 0x282b3538,
            .width = kege::ui::extend(),
        });
        updateMenu();
        _ui->pop();
    }

    void defOp(kege::GUI* gui, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        NavBar* param = (NavBar*)data;
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
            _ui->pushRoot
            ({
                .border = {.corner_curves = {8,8,8,8}},
                .padding = {2,2,2,2},
                .alignment =
                {
                    .type = kege::ui::AlignType::LIST,
                    .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .direction = kege::ui::AlignDir::DOWN,
                    .items = kege::ui::AlignItem::START,
                    .wrap = { false }
                },
                .color = 0x282b3538,
                .width = kege::ui::fixed(250),
                .height = kege::ui::flexible(),
            });
            display( _menus[ _selected ] );
            _ui->popRoot();

            //_ui->pushDeferredOp(<#const ui::ID &id#>, <#ui::WidgetId index#>, <#ui::DeferredOperation fn#>, <#const Params &params#>);
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
                .style = (elem.hot) ? &_ui->theme()->tab_elem_focus : &_ui->theme()->tab_elem,
                .alignment =
                {
                    .type = kege::ui::AlignType::LIST,
                    .origin = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .content = {kege::ui::AlignX::LEFT, kege::ui::AlignY::TOP},
                    .direction = kege::ui::AlignDir::RIGHT,
                    .items = kege::ui::AlignItem::START,
                    .wrap = { false }
                },
                .color = 0x282b350F,
                .width = ui::flexible(),
                .height = ui::fixed(20),
                .single_click = ui::ClickTrigger::OnRelease,
            });
            _ui->text( elem.text );
            if (elem.elements.size())
            {
                _ui->label({">"});
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
