//
//  ui-panel.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-panel.hpp"
#include "../editor-layer.hpp"
#include "../dock/ui-dock-manager.hpp"

namespace kege::ui{

    struct PanelPtr{ Panel* panel; };

    void Panel::updateRectOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data)
    {
        ((PanelPtr*)data)->panel->_rect = layout->elem( widget_id )->rect;
    }

    void Panel::operator()(const kege::ui::AssetMetadataDropOff& event)
    {
        if ( _ui->testPointVsRect(event.position, _rect) )
        {
            handle( event );
        }
    }

    void Panel::update()
    {
        _widget_index = _ui->push
        ({
            .user_id = _uid[0],
            .style = &_ui->theme().panel
        });
        updateLayout();
        _ui->pop();

        _ui->pushDeferredOp(_uid[0], _widget_index, updateRectOp, PanelPtr{this});
    }

    Panel::Panel( const std::string& name, kege::ui::DockManager* dm )
    :   _name( name  )
    ,   _manager( dm )
    ,   _ui(dm->getEditor()->getGUI())
    {
    }
}
