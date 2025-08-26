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
        if ( int( field ) == 0 )
        {
            field = layout.make
            ({
                .style = layout.getStyleByName( "menu-field" ),
                .trigger = ui::ClickTrigger::OnRelease,
                .text = text,
            });
            content = layout.make
            ({
                .style = layout.getStyleByName( "menu-content" ),
                .mouseover = false,
            });
            content->x = 30;
            content->y = 30;
        }

        layout.put( field );

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
        layout.push( content );
    }

    void Menu::endContent( ui::Layout& layout )
    {
        layout.pop();
    }

}
