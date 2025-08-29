//
//  ui-options.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#include "ui-options.hpp"

namespace kege::ui{

    int Options::select( ui::Layout& layout, const char* text, const std::vector<const char*>& list )
    {
        if ( int( field ) == 0 )
        {
            field = layout.make
            ({
                .text = text,
                .trigger = ClickTrigger::OnClick,
                .style = layout.getStyleByName( "option-field" )
            });

            content = layout.make
            ({
                .style = layout.getStyleByName( "option-content" )
            });
            content->y = 40;
            show = false;

            if ( options.empty() )
            {
                options.resize( list.size() );
            }

            for (int i=0; i < list.size(); ++i)
            {
                if ( int( options[i] ) == 0 )
                {
                    options[i] = layout.make
                    ({
                        .text = list[i],
                        .trigger = ClickTrigger::OnClick,
                        .style = layout.getStyleByName( "option-item" )
                    });
                }
                else
                {
                    options[i]->text.text = list[i];
                }
            }
        }



        layout.put( field );

        int selection = -1;
        if ( show )
        {
            layout.push( content );
            for (int i=0; i < options.size(); ++i)
            {
                layout.put( options[i] );

                if( layout.mouseover( options[i] ) )
                {
                    index[1] = index[0];
                    index[0] = i;

                    if( index[1] != index[0] )
                    {
                        if ( index[1] < options.size() )
                        {
                            options[ index[1] ]->style = layout.getStyleByName( "option-item" );
                        }
                        options[ i ]->style = layout.getStyleByName( "option-highllight" );
                    }
                }
                else if( index[0] == i )
                {
                    options[ index[0] ]->style = layout.getStyleByName( "option-item" );
                    index[0] = -1;
                }
            }
            layout.pop();
        }

        if( layout.buttonDown() )
        {
            if ( show )
            {
                for (int i=0; i < options.size(); ++i)
                {
                    if( layout.click( options[i] ) )
                    {
                        selection = i;
                    }
                }
            }

            if ( layout.click( field ) && !state )
            {
                state = true;
                show = !show;
            }
            else if ( !layout.click( field ) )
            {
                show = false;
                state = false;
            }
        }

        return selection;
    }

}
