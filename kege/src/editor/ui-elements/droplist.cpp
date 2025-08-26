//
//  droplist.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/21/25.
//

#include "droplist.hpp"

namespace kege{

    bool Droplist::begin( ui::Layout& layout, bool has_child, int space, const char* text )
    {
        if ( int(container) == 0 )
        {
            spacer = ui::Style
            {
                .width = ui::fixed( space ),
                .height = ui::extend(),
            };

            container = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-container" ),
            });
            content = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-content" ),
            });
            field = layout.make
            ({
                .trigger = ui::ClickTrigger::OnRelease,
                .style = layout.getStyleByName( "properties-field" ),
            });
            icon = layout.make
            ({
                .text = "+",
                .mouseover = false,
                .style = layout.getStyleByName( "properties-icon" ),
            });
            label = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-label" ),
                .text = text,
            });
        }

        layout.push( container );
        {
            layout.push( field );
            {
                //layout.put( spacer );
                layout.put( icon );
                layout.put( label );
            }
            layout.pop();
        }

        if ( layout.click( field ) )
        {
            if ( !state.open[0] )
            {
                state.open[0] = true;
                state.open[1] = !state.open[1];
                icon->text.text = ( state.open[1] )? "-" : "+";
            }
        }
        else if( !layout.input()->buttonDown() )
        {
            state.open[0] = false;
        }
        return state.open[1];
    }

    void Droplist::beginContent( ui::Layout& layout )
    {
        layout.push( content );
    }

    void Droplist::endContent( ui::Layout& layout )
    {
        layout.pop();
    }

    void Droplist::end( ui::Layout& layout )
    {
        layout.pop();
    }

}




