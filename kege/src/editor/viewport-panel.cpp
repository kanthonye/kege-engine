//
//  viewport-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "viewport-panel.hpp"

namespace kege{

    ViewportPanel& ViewportPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        
        _main = layout.make
        ({
            .visible    = true,
            .mouseover  = false,
            .texr       = ui::TexrID
            {
                .id     = 2,
                .x      = 0.f,
                .y      = 0.f,
                .width  = 1.f,
                .height = 1.f,
            },
            .style      = layout.getStyleByName( "viewport" )
        });

        //Communication::add< kege::RenderPassContext*, ViewportPanel >( this );
        return *this;
    }

//    void ViewportPanel::operator()( kege::RenderPassContext* context )
//    {
//        if( context->name() != "scene-pass" )
//        {
//            return;
//        }
//    }

    void ViewportPanel::put( ui::Layout& layout )
    {
        layout.push( _main );
        layout.pop();
    }

    ViewportPanel::ViewportPanel()
    :   _engine( nullptr )
    {}

}
