//
//  theme.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 6/10/26.
//

#include "theme.hpp"
namespace kege{

    Theme::Theme()
    {
        panel = {};
        panel.background = ui::Background(0x171420FF);
        //panel.border.corner_curves = {10,10,10,10};
        panel.height = ui::extend();
        panel.width = ui::extend();
        panel.padding = {10,10,10,10};
        panel.gap = {5,5};
        panel.alignment =
        {
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::DOWN
        };

        padded_list = {};
        //padded_list.border.corner_curves = {20,20,20,20};
        padded_list.background = ui::Background(0xFFFFFF00);
        padded_list.padding = {10,10,10,10};
        padded_list.height = ui::flexible();
        padded_list.width = ui::extend();
        padded_list.alignment =
        {
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::DOWN,
        };
        padded_list.gap = {4, 4};

        panel_transparnt = {};
        panel_transparnt.background = ui::Background(0xFFFFFF00);
        panel_transparnt.height = ui::extend();
        panel_transparnt.width = ui::extend();
        panel_transparnt.alignment =
        {
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::DOWN,
        };
        panel_transparnt.gap = {4, 4};

        scroll_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::extend(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        group = kege::ui::Style
        {
            //.border.corner_curves = {20,20,20,20},
            .background = ui::Background(0xFFFFFF00),
            .padding = {10, 0, 10, 0},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        dock.width = ui::extend();
        dock.height = ui::extend();
        dock.padding = {};
        dock.background = ui::Background(0x0b090fFF);
        dock.alignment = ui::Alignment
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = ui::AlignDir::DOWN,
        };

        tab = kege::ui::Style
        {
            .background = ui::Background(0x0B090F00),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {4,0},
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };

        tab_elem = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::flexible(),
            .background = ui::Background(0x171420FF),
            .align_text =  ui::AlignText::Center,
            .padding = {10,4,10,4},
            .gap = {20,0}
        };

        tab_label = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::flexible(),
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Left,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };

        ghost = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF20),
            .align_text =  ui::AlignText::Center,
            .position = ui::Positioning::Absolute,
            .text_color = 0xBBA0FFFF,
            .height = ui::fixed(40),
            .width = ui::fixed(100),
            .font_size = 20,
            .border.corner_curves = {8,8,8,8},
        };



        Theme::y_seperator = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .height = ui::extend(),
            .width = ui::fixed(0),
        };

        Theme::x_seperator = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .height = ui::fixed(0),
            .width = ui::extend(),
        };

        Theme::label = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
        };

        Theme::fitted_label = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };

        row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };

        Theme::row_wrap = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            },
            .gap = {8, 8},
        };

        column = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF08),
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };



        Theme::slider_container = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF0B),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            },
            .border.corner_curves = {6,6,6,6},
            .padding = {6,5,6,5},
            .gap.width = 6,
        };
        Theme::slider_inner_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };

        Theme::slider_row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF00),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            },
            .gap = {6, 6},
            .border.corner_curves = {6,6,6,6},
        };
        Theme::slider_name = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };
        Theme::slider_value = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };
        Theme::slider_range = kege::ui::Style
        {
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {2,0,0,0},
            .text_color = 0xFFFFFF50,
            .height = ui::fixed(18),
            .width = ui::flexible(),
            .font_size = 20,
        };



        Theme::radio = kege::ui::Style
        {
            .height = ui::fixed(10),
            .width = ui::fixed(10),
            .background = 0xFFBB0040,
            .border.corner_curves = {5,5,5,5},
        };
        Theme::radio_active = kege::ui::Style
        {
            .height = ui::fixed(10),
            .width = ui::fixed(10),
            .background = 0xFFBB00FF,
            .border.corner_curves = {5,5,5,5},
        };

        Theme::button = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .text_color = 0xFFFFFFFF
        };
        disabled_button = kege::ui::Style
        {
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .align_text =  ui::AlignText::Center,
            .text_color = 0xFFFFFFF0
        };

        Theme::collapsable_removable_header = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .border.corner_curves = {10,10,10,10},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {6,0,6,0},
            .gap = {4,0}
        };

        container_header = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .border.corner_curves = {0,0,0,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {0,0,0,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4,0}
        };
        container_body = kege::ui::Style{
            .background = ui::Background(0x23232300),
            .border.corner_curves = {0,0,0,0},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {4,4,4,4},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4,4}
        };



        Theme::scrubber_row = kege::ui::Style
        {
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0xFFFFFF10),
            .padding = {8,0,8,0},
            .border.corner_curves = {6,6,6,6},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        Theme::scrubber = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF06 ),
            .height = ui::fixed(20),
            .width = ui::extend(),
            //.padding = {4,0,4,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            },
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
        };
        Theme::scrubber_focus = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF1B ),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {8,0,8,0},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            },
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
        };
        Theme::scrubber_label = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF00 ),
            .height = ui::fixed(20),
            .width = ui::fixed(20),
            //.padding = {8,0,8,0},
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
        };


        Theme::card = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF0B ),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10, 10, 10, 10},
            .alignment =
            {
                .direction = ui::AlignDir::DOWN
            },
            .gap = {4, 4},
        };


        card2 = kege::ui::Style
        {
            .background = ui::Background( 0xFFFFFF0B ),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .padding = {10, 10, 10, 10},
            .alignment =
            {
                .direction = ui::AlignDir::RIGHT
            },
            .gap = {4, 4},
        };



        Theme::navbar = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0F),
            .border.corner_curves = {20,20,20,20},
            .padding = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {6,6},
            .alignment =
            {
                .direction = ui::AlignDir::RIGHT
            },
        };
        Theme::broswer_file_path = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0F),
            .border.corner_curves = {10,10,10,10},
            .padding = {10,0,10,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .text_color = 0xFFFFFFFF,
            .align_text = ui::AlignText::Left,
        };
        Theme::filter_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .height = ui::fixed(20),
            .width = ui::percent(30),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };

        Theme::filter_input = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF0D),
            .border.corner_curves = {10,10,10,10},
            .padding = {10,0,10,0},
            .height = ui::fixed(20),
            .width = ui::extend(),
        };

        Theme::file_container = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF00),
            .padding = {10,10,10,10},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            },
            .gap = {8, 8},
        };


        option = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF06),
            .border.corner_curves = {6,6,6,6},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };
        option_list = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .padding = {5,5,5,5},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            },
            .gap = {4, 4},
        };
        option_selection = kege::ui::Style
        {
            .background = 0xFFFFFF40,
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        option_hot = kege::ui::Style
        {
            .background = 0xFFFFFF30,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        option_element = kege::ui::Style
        {
            .background = 0xFFFFFF10,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };





        text_field_editing = kege::ui::Style
        {
            .background = 0x456E66FA,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        text_field_focus = kege::ui::Style
        {
            .background = 0xFFFFFF0A,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };
        text_field_idle = kege::ui::Style
        {
            .background = 0xFFFFFF05,
            .border.corner_curves = {4,4,4,4},
            .align_text = ui::AlignText::Left,
            .padding = {8,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(20),
            .width = ui::extend(),
            .font_size = 20,
        };

        hierarchy_main_bar  = kege::ui::Style
        {
            .background = ui::Background(0x222222FF),
            .padding = {5,5,5,5},
            .height = ui::flexible(),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF10),
            .border.corner_curves = {2,2,2,2},
            .padding = {4,4,4,4},
            .height = ui::fixed(24),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity = kege::ui::Style
        {
            .background = ui::Background(0xFFFFFF03),
            .border.corner_curves = {4,4,4,4},
            .padding = {4,4,4,4},
            .height = ui::fixed(24),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT,
            },
            .gap = {4, 4},
        };
        hierarchy_entity_selected = hierarchy_entity;
        hierarchy_entity_selected.background = ui::Background(0xFFFFFF20);



        close_butn = kege::ui::Style{
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .font_size = 20,
            .border.corner_curves = {2,2,2,2},
        };

        slider_knob = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::fixed(15),
            .background = 0xFF22FFFF,
            .align_text =  ui::AlignText::Center,
        };
        slider_track = kege::ui::Style{
            .height = ui::fixed(15),
            .width = ui::extend(),
            .background = 0xFFFFFF0B,
            .align_text = ui::AlignText::Center,
        };


        slide_bar = kege::ui::Style{
            .background = 0xFF22FFFF,
            .position = ui::Positioning::Absolute,
            .height = ui::fixed(14),
            .width = ui::fixed(14),
        };
        slide_bar = kege::ui::Style{
            .background = 0xFFFFFF20,
            .height = ui::fixed(14),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        slide_bar_value = kege::ui::Style{
            .position = ui::Positioning::Absolute,
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Center,
            .height = ui::fixed(14),
            .width = ui::percent(100),
            .font_size = 20,
        };


        collapsable_header = kege::ui::Style{
            .background = ui::Background(0x23232300),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .padding = {6,0,6,0},
            .gap = {4,0}
        };

        list = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            }
        };
        list_elem = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };
        list_elem_focus = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };


        select = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN,
            }
        };
        select_elem = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };
        select_elem_focus = kege::ui::Style{
            .border.corner_curves = {4,4,4,4},
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .font_size = 20,
            .text_color = 0xFFFFFFFF,
        };


        tab = kege::ui::Style{
            .background = ui::Background(0x10101000),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };
        tab_elem = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = 0x171420FF,
            .align_text = ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };
        tab_elem_focus = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = ui::Background(0x171420FF),
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {4,4,4,4},
            .font_size = 20,
        };

        hierarchy_panel = kege::ui::Style{
            .background = ui::Background(0x433333FF),
            .height = ui::extend(),
            .width = ui::fixed(400),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignX::CENTER, ui::AlignY::TOP},
                .direction = ui::AlignDir::DOWN
            }
        };
        viewport_panel = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x222222FF),
            //.padding = {10,10,10,10},
            .alignment =
            {
                .content = {ui::AlignX::RIGHT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };


        numeric = kege::ui::Style{
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::fixed(18),
            .width = ui::extend(),
            .padding = {8,2,8,2},
            .gap = {2,2},
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_label = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(100),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_value = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };
        numeric_focus = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT
            }
        };


        folder_icon = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(60),
            .font_size = 20,
        };
        file_icon = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(20),
            .font_size = 20,
        };
        selected_item = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(40),
            .font_size = 20,
        };
        selection_box = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(40),
            .width = ui::fixed(40),
            .font_size = 20,
        };
        tooltip = kege::ui::Style{
            .background = ui::Background(0xCC7F41FF),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .padding = {10,10,10,10},
            .height = ui::fixed(150),
            .width = ui::fixed(250),
            .font_size = 20,
            .alignment =
            {
                .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
                .direction = ui::AlignDir::RIGHT,
                .wrap = {true, ui::AlignDir::DOWN},
            }
        };
    }
}
