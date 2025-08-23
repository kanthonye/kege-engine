//
//  navbar-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "navbar-panel.hpp"

namespace kege{

    void NavbarPanel::put( ui::Layout& layout )
    {
        layout.push( _main );

//        layout.put(layout.make
//         ({
//             .style =
//             {
//                 .background = ui::bgColor(0x1E1E1EFF),
//                 .width = ui::extend(),
//                 .height = ui::fixed(25),
//                 .color = ui::rgb(0x9C9C9C),
//                 .border_radius = 5,
//                 .padding = {8,0,0,0},
//                 .gap = {3,3},
//                 .align =
//                 {
//                     .origin = ui::ALIGN_TOP_CENTER,
//                     .direction = ui::DIRECTION_TOP_TO_BOTTOM,
//                     .wrap_around = false
//                 },
//                 .font_size = 20,
//             },
//            .text = "file"
//         }));

        layout.pop();
    }

    NavbarPanel& NavbarPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::fixed( 30 ),
                //.border_radius = 8,
                //.padding = {},
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                    .wrap_around = false
                },
            },
            //.text = "NavbarPanel",
        });
        return *this;
    }

    NavbarPanel::NavbarPanel()
    :   _engine( nullptr )
    {}

}
