//
//  ui-button.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/23/25.
//

#include "ui-button.hpp"

namespace kege::ui{

    bool Button::clicked( ui::Layout& layout, const char* text )
    {
//        if ( !field )
//        {
//            field = layout.make
//            ({
//                .text = text,
//                .style = layout.getStyleByName( "button" ),
//            });
//        }
        layout.put({ .id = &field, .text = text });
        return layout.click( field );
    }

}
