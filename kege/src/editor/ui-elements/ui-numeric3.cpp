//
//  ui-numeric3.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric3.hpp"

namespace kege::ui{

    void Numeric3::update( ui::Layout& layout, vec3& value, const char* text )
    {
        if ( int( container ) == 0 )
        {
            container = layout.make
            ({
                .style = layout.getStyleByName( "vector-container" ),
            });

            content = layout.make
            ({
                .style = layout.getStyleByName( "vector-content" ),
            });

            label = layout.make
            ({
                .style = layout.getStyleByName( "vector-label" ),
                .text = text
            });
        }

        layout.push( container );
            layout.put( label );
            layout.push( content );
                x.update( layout, value.x, "x:" );
                y.update( layout, value.y, "y:" );
                z.update( layout, value.z, "z:" );
            layout.pop();
        layout.pop();
    }

}
