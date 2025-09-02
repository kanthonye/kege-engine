//
//  ui-numeric.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric.hpp"

namespace kege::ui{

    bool Numeric::update( ui::Layout& layout, float& num, const char* text )
    {
        if ( !container )
        {
            has_focused = false;
            cursor = 0;

            char snum[16];
            snprintf(snum, 16, "%.3f", num );

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
                .text = snum
            });
        }

        layout.push( container );
            layout.put( label );
            layout.put( value );
        layout.pop();

        bool active = false;
        if ( layout.input()->buttonDown() )
        {
            if ( layout.click( container ) )
            {
                num += (layout.input()->deltaPosition().x - layout.input()->deltaPosition().y) * 0.0125;
                char snum[16];
                snprintf(snum, 16, "%.3f", num );
                value->text.text = snum;
                active = true;
            }
            else if ( layout.doubleClick( container ) )
            {
                has_focused = true;
            }
            else if ( has_focused )
            {
                has_focused = false;
                container->style = layout.getStyleByName( "numeric-container" );
            }
        }

        if ( has_focused )
        {
            layout.input()->onTextInput(ui::Input::INPUT_NUMERIC, &value->text.text, &cursor, &has_focused );
            container->style = layout.getStyleByName( "numeric-focus" );

            if ( !has_focused )
            {
                container->style = layout.getStyleByName( "numeric-container" );
                num = atof( value->text.text.c_str() );
            }
        }

        return has_focused || active;
    }

}
