//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "../editor-layer.hpp"
#include "ui-menu-bar.hpp"

namespace kege::ui{

    MenuBar::MenuBar( kege::ui::DockManager* dm )
    :   kege::ui::Panel( "MenuBar", dm )
    {
    }

    void MenuBar::updateLayout()
    {
        _ui->push({});
        updateFileMenu();
        updateSettingMenu();
        _ui->pop();
    }

    void MenuBar::updateFileMenu()
    {
        _ui->push({});
        if ( _ui->button( _uid[0], {"New"} ) )
        {
            //Communication::broadcast<const OpenAddAssetWindow&>({});
        }
        if ( _ui->button(_uid[0], {"Open"}) )
        {
            //Communication::broadcast<const OpenAddAssetWindow&>({});
        }
        if ( _ui->button(_uid[0], {"Save"}) )
        {
            //Communication::broadcast<const OpenAddAssetWindow&>({});
        }
        if ( _ui->button(_uid[0], {"Quit"}) )
        {
            //Communication::broadcast<const OpenAddAssetWindow&>({});
        }
        _ui->pop();
    }

    void MenuBar::updateSettingMenu()
    {
    }

}
