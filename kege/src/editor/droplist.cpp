//
//  droplist.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/21/25.
//

#include "droplist.hpp"

namespace kege{

    bool Droplist::open( ui::Layout& layout, bool has_child, int space, const char* text )
    {
        if ( int(container) == 0 )
        {
            container = layout.make
            ({
                .style =
                {
                    .visible = true,
                    .width = ui::extend(),
                    .height = ui::flexible(),
                    .background = ui::bgColor(0xFFFFFF00),
                    .color = ui::rgba(0xFFFFFF83),
                    .mouseover = false,
                    .align =
                    {
                        .origin = ui::ALIGN_TOP_LEFT,
                        .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                        .wrap_around = false
                    },
                },
            });
        }
        if ( int(spacer) == 0 )
        {
            spacer = layout.make
            ({
                .style =
                {
                    .visible = false,
                    .width = ui::fixed(space),
                    .height = ui::extend(10),
                    .mouseover = false,
                },
            });
        }
        if ( int(field) == 0 )
        {
            field = layout.make
            ({
                .style =
                {
                    .visible = true,
                    .font_size = 20,
                    .width = ui::extend(),
                    .height = ui::fixed(20),
                    .background = ui::bgColor(0xFFFFFF0B),
                    .color = ui::rgba(0xFFFFFF83),
                    .click_trigger = ui::ClickTrigger::OnRelease,
                    .align =
                    {
                        .origin = ui::ALIGN_LEFT_CENTER,
                        .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                        .wrap_around = false
                    },
                }
            });
        }
        if ( int(icon) == 0 )
        {
            icon = layout.make
            ({
                .text = ">",
                .style =
                {
                    .visible = true,
                    .border_radius = 5,
                    .width = ui::fixed(16),
                    .height = ui::extend(),
                    .background = ui::bgColor(0xFFFFFF00),
                    .color = ui::rgba(0xFFFFFF83),
                    .padding = {5,0,0,0},
                    .align =
                    {
                        .origin = ui::ALIGN_CENTER_CENTER,
                        .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                        .wrap_around = false
                    },
                }
            });
        }
        if ( int(label) == 0 )
        {
            label = layout.make
            ({
                .style =
                {
                    .visible = true,
                    .font_size = 20,
                    .width = ui::extend(),
                    .height = ui::fixed(20),
                    .background = ui::bgColor(0xFFFFFF00),
                    .color = ui::rgba(0xFFFFFF83),
                    .padding = {0,0,0,0},
                    .mouseover = false,
                },
                .text = text,
            });
        }

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

        if ( has_child )
        {
            icon->text.data = ( state.open[1] )? "-" : "+";
        }
        else
        {
            icon->text.data = "-";
        }

        if ( layout.click( icon ) )
        {
            if ( !state.open[0] )
            {
                state.open[0] = true;
                state.open[1] = !state.open[1];
            }
        }
        else if( !layout.input()->buttonDown() )
        {
            state.open[0] = false;
        }
        return state.open[1];
    }

    void Droplist::beginContent( ui::Layout& layout )
    {
        if( int(content) == 0 )
        {
            content = layout.make
            ({
                .style =
                {
                    .visible = true,
                    .width = ui::extend(),
                    .height = ui::flexible(),
                    .background = ui::bgColor(0xFFFFFF00),
                    .mouseover = false,
                    .gap = {10,1},
                    .align =
                    {
                        .origin = ui::ALIGN_TOP_LEFT,
                        .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                        .wrap_around = false
                    },
                },
            });
        }
        layout.push( content );
    }

    void Droplist::endContent( ui::Layout& layout )
    {
        layout.pop();
    }

}


namespace kege{

    void FloatInput::update( ui::Layout& layout, float& value, const char* text )
    {
        put( layout, text );
        if ( layout.input()->buttonDown() )
        {
            if ( layout.click( container ) )
            {
                value += layout.input()->deltaPosition().x;
                char s[16];
                snprintf(s, 16, "%.3f", value );
                data = s;
            }
            else if ( layout.doubleClick( container ) )
            {
                has_focused = true;
                char s[16];
                snprintf(s, 16, "%.3f", value );
                data = s;
            }
            else if ( has_focused )
            {
                has_focused = false;
                container->style.background.color = ui::rgba(0xFFFFFF0B);
            }
        }

        if ( has_focused )
        {
            layout.input()->onTextInput(ui::Input::INPUT_NUMERIC, &data, &cursor, &has_focused );
            container->style.background.color = ui::rgba(0xFFFFFF1B);

            if ( !has_focused )
            {
                container->style.background.color = ui::rgba(0xFFFFFF0B);
                value = atof( data.c_str() );
            }
        }
    }

    void FloatInput::put( ui::Layout& layout, const char* text )
    {
        if ( int( container ) == 0 )
        {
            data = "0.0";
            has_focused = false;
            cursor = 0;

            container = layout.make
            ({
                .click_trigger = ui::ClickTrigger::OnClick,
                .style =
                {
                    .background = ui::bgColor(0xFFFFFF0B),
                    .border_radius = 5,
                    .width = ui::extend(),
                    .height = ui::flexible(),
                    .padding = {8,4,8,4},
                    .gap.width = 2,
                    .align =
                    {
                        .self = ui::AlignSelf::Relative,
                        .origin = ui::ALIGN_TOP_CENTER,
                        .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                        .wrap_around = false
                    },
                },
            });
            label = layout.make
            ({
                .style =
                {
                    .mouseover = false,
                    .background = ui::bgColor(0xFFFFFF00),
                    .width = ui::fixed(20),
                    .height = ui::fixed(20),
                    .color = ui::rgb(0x9C9C9C),
                    .padding = {0,2,0,0},
                    .align =
                    {
                        .text = ui::AlignText::Left,
                        .self = ui::AlignSelf::Relative,
                    },
                    .font_size = 20,
                },
               .text = text
            });
            knob = layout.make
            ({
                .style =
                {
                    .mouseover = false,
                    .background = ui::bgColor(0xFFFFFF00),
                    .width = ui::extend(),
                    .height = ui::fixed(20),
                    .color = ui::rgb(0x9C9C9C),
                    .padding = {0,2,0,0},
                    .border_radius = 5,
                    .align =
                    {
                        .text = ui::AlignText::Right,
                        .self = ui::AlignSelf::Relative,
                        .wrap_around = false
                    },
                    .font_size = 20,
                },
                .text = data.data()
            });
        }

        layout.push( container );
        layout.put( label );
        layout.put( knob );
        layout.pop();
    }

}
namespace kege{

    void Vec3Input::update( ui::Layout& layout, vec3& value, const char* text )
    {
        if ( int( container ) == 0 )
        {
            container = layout.make
            ({
                .style =
                {
                    .background = ui::bgColor(0x1E1E1EFF),
                    .width = ui::extend(),
                    .height = ui::flexible(),
                    .color = ui::rgb(0x9C9C9C),
                    .border_radius = 5,
                    .padding = {20,5,20,10},
                    .gap = {2,5},
                    .align =
                    {
                        .self = ui::AlignSelf::Relative,
                        .origin = ui::ALIGN_TOP_CENTER,
                        .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                        .wrap_around = false
                    },
                },
            });

            content = layout.make
            ({
                .style =
                {
                    .background = ui::bgColor(0x1E1E1E00),
                    .width = ui::extend(),
                    .height = ui::flexible(),
                    .color = ui::rgb(0x9C9C9C),
                    .border_radius = 5,
                    .gap = {3,3},
                    .align =
                    {
                        .self = ui::AlignSelf::Relative,
                        .origin = ui::ALIGN_LEFT_CENTER,
                        .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                        .wrap_around = false
                    },
                },
            });

            label = layout.make
            ({
                .style =
                {
                    .background = ui::bgColor(0xFFFFFF00),
                    .width = ui::extend(),
                    .height = ui::fixed(20),
                    .color = ui::rgb(0x9C9C9C),
                    .padding = {0,2,0,0},
                    .align =
                    {
                        .text = ui::AlignText::Left,
                        .self = ui::AlignSelf::Relative,
                    },
                    .font_size = 20,
                },
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
