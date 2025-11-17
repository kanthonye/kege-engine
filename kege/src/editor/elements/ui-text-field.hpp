//
//  ui-text-field.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/12/25.
//

#ifndef ui_text_field_hpp
#define ui_text_field_hpp

#include "ui-tree-node.hpp"

namespace kege::ui{

    struct TextField
    {
        bool update( ui::Layout* layout );
        void init( ui::Layout* layout, const std::string& text );

        ui::TextFieldState state;
        ui::Elem container;
        ui::Elem text;

        uint16_t mode;
        bool modified;
    };

}
#endif /* ui_text_field_hpp */
