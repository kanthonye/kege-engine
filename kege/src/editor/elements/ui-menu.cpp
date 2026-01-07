//
//  ui-menu.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#include "ui-menu.hpp"

namespace kege::ui{

    bool Menu::open( ui::Layout& layout, const char* text )
    {
//        if ( !field )
//        {
//            field = layout.make
//            ({
//                .style = layout.getStyleByName( "menu-field" ),
//                .single_click = ui::ClickTrigger::OnRelease,
//                .text = text,
//            });
//            content = layout.make
//            ({
//                .style = layout.getStyleByName( "menu-content" ),
//                .mouseover = false,
//            });
//            content->offset.x = 30;
//            content->offset.y = 30;
//        }

        layout.put({ .uid = &field });

        if ( layout.click( field ) )
        {
            state = true;
        }
        else if( layout.input()->buttonDown() )
        {
            state = false;
        }
        return state;
    }

    void Menu::beginContent( ui::Layout& layout )
    {
        layout.push({ .uid = &content });
    }

    void Menu::endContent( ui::Layout& layout )
    {
        layout.pop(0);
    }

}
