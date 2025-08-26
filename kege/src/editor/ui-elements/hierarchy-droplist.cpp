//
//  hierarchy-droplist.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "hierarchy-droplist.hpp"


namespace kege::ui{

    bool HierarchyDroplist::open( ui::Layout& layout, bool has_child, int space, const char* text )
    {
        if ( int(container) == 0 )
        {
            spacer_style = ui::Style
            {
                .width = ui::fixed( space ),
                .height = ui::extend(),
            };

            container = layout.make
            ({
                .style = layout.getStyleByName( "droplist" ),
            });

            spacer = layout.make
            ({
                .visible = false,
                .style = &spacer_style,
            });

            field = layout.make
            ({
                .style = layout.getStyleByName( "droplist-field" ),
                .trigger = ui::ClickTrigger::OnClick,
            });

            icon = layout.make
            ({
                .style = layout.getStyleByName( "droplist-icon" ),
                .trigger = ui::ClickTrigger::OnRelease,
                .text = {"-",0,0,0,0},
            });

            label = layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "droplist-label" ),
                .text = {text,0,0,0,0},
            });
        }

        // droplist hierarchy
        layout.push( container );
        {
            layout.push( field );
            {
                layout.put( spacer );
                layout.put( icon );
                layout.put( label );
            }
            layout.pop();
        }
        layout.pop();

        // droplist logics
        icon->text.text = ( has_child )
        ? (( state.open[1] ) ? "-" : "+")
        : "-";

        if ( layout.click( icon ) )
        {
            if ( !state.open[0] )
            {
                state.open[0] = true;
                state.open[1] = !state.open[1];
            }
        }
        else if( state.open[0] && layout.input()->buttonDown() )
        {
            state.open[0] = false;
        }

        return state.open[1] && has_child;
    }

    void HierarchyDroplist::beginContent( ui::Layout& layout )
    {
        if( int(content) == 0 )
        {
            content = layout.make
            ({
                .style = layout.getStyleByName( "droplist-content" ),
                .mouseover = false,
                .visible = false
            });
        }
        layout.push( content );
    }

    void HierarchyDroplist::endContent( ui::Layout& layout )
    {
        layout.pop();
    }




}
