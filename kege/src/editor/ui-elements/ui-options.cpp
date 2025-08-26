//
//  ui-options.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#include "ui-options.hpp"

namespace kege::ui{

    bool Options::operator()( ui::Layout& layout, const char* text, const std::vector<const char*>& list )
    {
        if ( int( field ) == 0 )
        {
            field = layout.make
            ({
                .text = text,
                .trigger = ClickTrigger::OnRelease,
                .style = layout.getStyleByName( "option-field" )
            });

            content = layout.make
            ({
                .style = layout.getStyleByName( "option-content" )
            });
            content->y = 40;
        }


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
                    .text = text,
                    .style = layout.getStyleByName( "option-item" )
                });
            }
            else
            {
                options[i]->text.text = list[i];
            }
        }

        layout.put( field );
        if ( state )
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
                            options[ index[1] ]->style->background.color = ui::rgba(0xFFFFFF00);
                        }
                        options[ i ]->style->background.color = ui::rgba(0xFFFFFF10);
                    }
                }
            }
            layout.pop();
        }


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

    void Options::display( ui::Layout& layout )
    {
    }

}
