//
//  ui-text-field.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/12/25.
//

#include "ui-text-field.hpp"

namespace kege::ui{

    bool TextField::update( ui::Layout* layout )
    {
        layout->push({ .id = &container });
        layout->put({ .id = &this->text });
        layout->pop();

        bool clicked = false;

        if ( layout->click( container ) )
        {
            clicked = true;
            mode += 1;
            if ( mode >= 2 )
            {
                state.has_focus = true;
            }
        }
        else if ( layout->doubleClick( container ) )
        {
            mode = 2;
            state.has_focus = true;
        }
        else if ( !layout->hasFocus( container ) && mode > 0 )
        {
            state.has_focus = false;
            mode = 0;
        }

        if ( state.has_focus )
        {
            modified = true;
            //state.has_focus = layout->onTextInput( this->text, &this->text->text.text );
            if ( !state.has_focus )
            {
                mode = 0;
            }
        }

        return clicked;
    }

    void TextField::init( ui::Layout* layout, const std::string& txt )
    {
//        container = layout->make
//        ({
//            .single_click = ui::ClickTrigger::OnRelease,
//            .double_click = ui::ClickTrigger::Immediate,
//            .style = layout->getStyleByName( "bar" ),
//        });
//
//        this->text = layout->make
//        ({
//            .mouseover = false,
//            .style = layout->getStyleByName( "text" ),
//            .text = {txt.c_str(), 0,0,0,0}
//        });
        modified = false;
        mode = 0;
    }
}
