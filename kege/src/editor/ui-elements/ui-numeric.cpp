//
//  ui-numeric.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric.hpp"

namespace kege::ui{

    void Numeric::update( ui::Layout& layout, float& num, const char* text )
    {
        if ( int( container ) == 0 )
        {
            data = "0.000";
            has_focused = false;
            cursor = 0;

            container = layout.make
            ({
                .trigger = ui::ClickTrigger::OnClick,
                .style = layout.getStyleByName( "numeric-container" ),
            });
            label = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "numeric-label" ),
                .text = text
            });
            value = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "numeric-value" ),
                .text = data.data()
            });
        }

        layout.push( container );
            layout.put( label );
            layout.put( value );
        layout.pop();

        if ( layout.input()->buttonDown() )
        {
            if ( layout.click( container ) )
            {
                num += layout.input()->deltaPosition().x;
                char s[16];
                snprintf(s, 16, "%.3f", num );
                data = s;
            }
            else if ( layout.doubleClick( container ) )
            {
                has_focused = true;
                char s[16];
                snprintf(s, 16, "%.3f", num );
                data = s;
            }
            else if ( has_focused )
            {
                has_focused = false;
                container->style = layout.getStyleByName( "numeric-container" );
            }
        }

        if ( has_focused )
        {
            layout.input()->onTextInput(ui::Input::INPUT_NUMERIC, &data, &cursor, &has_focused );
            container->style = layout.getStyleByName( "numeric-focus" );

            if ( !has_focused )
            {
                container->style = layout.getStyleByName( "numeric-container" );
                num = atof( data.c_str() );
            }
        }
    }

}
