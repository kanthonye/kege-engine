//
//  ui-numeric4.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric4.hpp"

namespace kege::ui{

    bool Numeric4::update( ui::Layout& layout, float& x, float& y, float& z, float& w, const char* text )
    {
        if ( !container )
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

        bool active = false;
        layout.push( container );
        {
            layout.put( label );
            layout.push( content );
            {
                if( nx.update( layout, x, "x:" ) ) active = true;
                if( ny.update( layout, y, "y:" ) ) active = true;
                if( nz.update( layout, z, "z:" ) ) active = true;
                if( nw.update( layout, w, "w:" ) ) active = true;
            }
            layout.pop();
        }
        layout.pop();
        return active;
    }

}
