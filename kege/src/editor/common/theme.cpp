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
        panel.quad_color = 0x171420FF;
        //panel.corner_curves = {10,10,10,10};
        panel.height = ui::extend();
        panel.width = ui::extend();
        panel.padding = {10,10,10,10};
        panel.alignment =
        {
            .gap = {5,5},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN}
        };

        padded_list = {};
        //padded_list.corner_curves = {20,20,20,20};
        padded_list.quad_color = 0xFFFFFF00;
        padded_list.padding = {10,10,10,10};
        padded_list.height = ui::flexible();
        padded_list.width = ui::extend();
        padded_list.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        panel_transparnt = {};
        panel_transparnt.quad_color = (0xFFFFFF00);
        panel_transparnt.height = ui::extend();
        panel_transparnt.width = ui::extend();
        panel_transparnt.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        scroll_container = {};
        scroll_container.quad_color = (0xFFFFFF00);
        scroll_container.width = ui::extend();
        scroll_container.height = ui::extend();
        scroll_container.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN}
        };

        group = {};
        group.quad_color = (0xFFFFFF00);
        group.width = ui::extend();
        group.height = ui::flexible();
        group.padding = {10, 0, 10, 0};
        group.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };


        dock = {};
        dock.width = ui::extend();
        dock.height = ui::extend();
        //dock.quad_color = 0x0b090fFF;
        dock.alignment = ui::Alignment
        {
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        tab = {};
        tab.quad_color = (0x0B090FFF);
        tab.width = ui::extend();
        tab.height = ui::flexible();
        tab.padding = {4,4,4,4};
        tab.corner_curves = {5,5,5,5};
        tab.alignment =
        {
            .gap = {4,0},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT}
        };

        tab_elem = {};
        tab_elem.quad_color = 0x171420FF;
        tab_elem.width = ui::flexible();
        tab_elem.height = ui::flexible();
        tab_elem.text_align =  ui::AlignText::Center;
        tab_elem.padding = {10,4,14,4};
        tab_elem.corner_curves = {15,15,15,15};
        tab_elem.alignment.gap = {6,0};


        tab_label = {};
        //tab_label.quad_color = (0xFFFFFF00);
        tab_label.width = ui::flexible();
        tab_label.height = ui::fixed(20);
        tab_label.text_align =  ui::AlignText::Left;
        tab_label.font_size = 20;



//        tab = {};
//        tab.quad_color = (0x10101000);
//        tab.height = ui::fixed(20);
//        tab.width = ui::extend();
//        tab.alignment =
//        {
//            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
//            .direction = {ui::AlignDir::RIGHT}
//        };
//
//        tab_elem = {};
//        tab_elem.height = ui::fixed(20);
//        tab_elem.width = ui::fixed(120);
//        //tab_elem.quad_color = 0xBBA0FFFF;
//        tab_elem.text_align = ui::AlignText::Center;
//        tab_elem.font_size = 20;
//        tab_elem.alignment =
//        {
//            .content = {ui::AlignX::CENTER, ui::AlignY::TOP},
//            .direction = {ui::AlignDir::RIGHT, ui::AlignDir::DOWN}
//        };
//
//        tab_elem_focus = {};
//        tab_elem_focus.height = ui::fixed(20);
//        tab_elem_focus.width = ui::fixed(120);
//        tab_elem_focus.quad_color = (0xBBA0FFFF);
//        tab_elem_focus.text_align = ui::AlignText::Center;
//        tab_elem_focus.corner_curves = {4,4,4,4};
//        tab_elem_focus.font_size = 20;



        ghost = {};
        ghost.quad_color = (0xBBA0FFFF);
        ghost.width = ui::fixed(60);
        ghost.height = ui::fixed(60);
        ghost.corner_curves = {8,8,8,8};
        ghost.text_align =  ui::AlignText::Center;
        ghost.font_size = 20;
        ghost.position = ui::Positioning::Absolute;


        y_seperator = {};
        y_seperator.quad_color = 0xFFFFFF00;
        y_seperator.width = ui::fixed(0);
        y_seperator.height = ui::extend();

        x_seperator = {};
        x_seperator.quad_color = 0xFFFFFF00;
        x_seperator.width = ui::extend();
        x_seperator.height = ui::fixed(0);


        label = {};
        label.quad_color = 0xFFFFFF00;
        label.width = ui::extend();
        label.height = ui::fixed(18);
        label.text_align = ui::AlignText::Left;
        label.padding = {2,0,0,0};
        label.font_size = 20;

        fitted_label = {};
        fitted_label.quad_color = 0xFFFFFF00;
        fitted_label.text_align = ui::AlignText::Left;
        fitted_label.padding = {2,0,0,0};
        fitted_label.height = ui::fixed(18);
        fitted_label.width = ui::flexible();
        fitted_label.font_size = 20;

        row = {};
        row.height = ui::flexible();
        row.width = ui::extend();
        row.quad_color = (0xFFFFFF00);
        row.alignment =
        {
            .gap = {4, 4},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };


        row_wrap = {};
        row_wrap.height = ui::flexible();
        row_wrap.width = ui::extend();
        row_wrap.quad_color = (0xFFFFFF00);
        row_wrap.alignment =
        {
            .gap = {8, 8},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT,ui::AlignDir::DOWN},
        };


        column = {};
        column.quad_color = (0xFFFFFF08);
        column.height = ui::flexible();
        column.width = ui::extend();
        column.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };


        slider_container = {};
        slider_container.height = ui::flexible();
        slider_container.width = ui::extend();
        slider_container.quad_color = (0xFFFFFF0B);
        slider_container.corner_curves = {6,6,6,6};
        slider_container.padding = {6,5,6,5};
        slider_container.alignment =
        {
            .gap = {6,0},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT}
        };

        slider_inner_container = {};
        slider_inner_container.quad_color = (0xFFFFFF0B);
        slider_inner_container.height = ui::flexible();
        slider_inner_container.width = ui::extend();
        slider_inner_container.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };


        slider_row = {};
        slider_row.height = ui::flexible();
        slider_row.width = ui::extend();
        slider_row.quad_color = (0xFFFFFF00);
        slider_row.corner_curves = {6,6,6,6};
        slider_row.alignment =
        {
            .gap = {6, 6},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT}
        };


        slider_name = {};
        slider_name.quad_color = 0xFFFFFFFF;
        slider_name.text_align = ui::AlignText::Left;
        slider_name.padding = {2,0,0,0};
        slider_name.height = ui::fixed(18);
        slider_name.width = ui::flexible();
        slider_name.font_size = 20;


        slider_value = {};
        slider_value.quad_color = 0xFFFFFFFF;
        slider_value.text_align = ui::AlignText::Left;
        slider_value.padding = {2,0,0,0};
        slider_value.height = ui::fixed(18);
        slider_value.width = ui::flexible();
        slider_value.font_size = 20;

        slider_range = {};
        slider_range.quad_color = 0xFFFFFF50;
        slider_range.text_align = ui::AlignText::Left;
        slider_range.padding = {2,0,0,0};
        slider_range.height = ui::fixed(18);
        slider_range.width = ui::flexible();
        slider_range.font_size = 20;


        radio = {};
        radio.height = ui::fixed(10);
        radio.width = ui::fixed(10);
        radio.quad_color = 0xFFBB0040;
        radio.corner_curves = {5,5,5,5};

        radio_active = {};
        radio_active.height = ui::fixed(10);
        radio_active.width = ui::fixed(10);
        radio_active.quad_color = 0xFFBB00FF;
        radio_active.corner_curves = {5,5,5,5};


        button = {};
        button.height = ui::flexible();
        button.width = ui::extend();
        button.quad_color = 0x0f0d16FF;
        button.padding = {15,6,10,6};
        button.text_align =  ui::AlignText::Center;
        button.corner_curves = {15,15,15,15};


        disabled_button = {};
        disabled_button.height = ui::fixed(20);
        disabled_button.width = ui::extend();
        disabled_button.quad_color = 0xFFFFFFF0;
        disabled_button.text_align =  ui::AlignText::Center;


        collapsable_removable_header = {};
        collapsable_removable_header.quad_color = (0x232323FF);
        collapsable_removable_header.corner_curves = {10,10,10,10};
        collapsable_removable_header.height = ui::fixed(20);
        collapsable_removable_header.width = ui::extend();
        collapsable_removable_header.padding = {6,0,6,0};
        collapsable_removable_header.alignment.gap = {4,0};


        container_header = {};
        container_header.quad_color = (0x232323FF);
        container_header.corner_curves = {0,0,0,0};
        container_header.height = ui::fixed(20);
        container_header.width = ui::extend();
        container_header.padding = {0,0,0,0};
        container_header.alignment =
        {
            .gap = {4,0},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT},
        };


        container_body = {};
        container_body.quad_color = (0x23232300);
        container_body.corner_curves = {0,0,0,0};
        container_body.height = ui::flexible();
        container_body.width = ui::extend();
        container_body.padding = {4,4,4,4};
        container_body.alignment =
        {
            .gap = {4,4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };


        scrubber_row = {};
        scrubber_row.height = ui::flexible();
        scrubber_row.width = ui::extend();
        scrubber_row.quad_color = (0xFFFFFF10);
        scrubber_row.padding = {8,0,8,0};
        scrubber_row.corner_curves = {6,6,6,6};
        scrubber_row.alignment =
        {
            .gap = {4, 4},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };

        scrubber = {};
        scrubber.quad_color = ( 0xFFFFFF06 );
        scrubber.height = ui::fixed(20);
        scrubber.width = ui::extend();
        scrubber.text_align = ui::AlignText::Right;
        scrubber.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };

        scrubber_focus = {};
        scrubber_focus.quad_color = ( 0xFFFFFF1B );
        scrubber_focus.height = ui::fixed(20);
        scrubber_focus.width = ui::extend();
        scrubber_focus.padding = {8,0,8,0};
        scrubber_focus.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };
        scrubber_focus.text_align = ui::AlignText::Right;

        scrubber_label = {};
        scrubber_label.quad_color = ( 0xFFFFFF00 );
        scrubber_label.height = ui::fixed(20);
        scrubber_label.width = ui::fixed(20);
        scrubber_label.text_align = ui::AlignText::Left;



        card = {};
        card.quad_color = ( 0xFFFFFF0B );
        card.corner_curves = {6,6,6,6};
        card.height = ui::flexible();
        card.width = ui::extend();
        card.padding = {10, 10, 10, 10};
        card.alignment =
        {
            .gap = {4, 4},
            .direction = {ui::AlignDir::DOWN}
        };


        card2 = {};
        card2.quad_color = ( 0xFFFFFF0B );
        card2.corner_curves = {6,6,6,6};
        card2.height = ui::flexible();
        card2.width = ui::extend();
        card2.padding = {10, 10, 10, 10};
        card2.alignment =
        {
            .gap = {4, 4},
            .direction = {ui::AlignDir::RIGHT}
        };



        navbar = {};
        navbar.quad_color = (0xFFFFFF0F);
        navbar.corner_curves = {20,20,20,20};
        navbar.padding = {6,6,6,6};
        navbar.height = ui::flexible();
        navbar.width = ui::extend();
        navbar.alignment =
        {
            .gap = {6,6},
            .direction = {ui::AlignDir::RIGHT}
        };

        broswer_file_path = {};
        broswer_file_path.quad_color = (0xFFFFFF0F);
        broswer_file_path.corner_curves = {10,10,10,10};
        broswer_file_path.padding = {10,0,10,0};
        broswer_file_path.height = ui::fixed(20);
        broswer_file_path.width = ui::extend();
        broswer_file_path.text_align = ui::AlignText::Left;

        filter_container = {};
        filter_container.quad_color = (0xFFFFFF00);
        filter_container.height = ui::fixed(20);
        filter_container.width = ui::percent(30);
        filter_container.alignment =
        {
            .gap = {4, 4},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };

        filter_input = {};
        filter_input.quad_color = (0xFFFFFF0D);
        filter_input.corner_curves = {10,10,10,10};
        filter_input.padding = {10,0,10,0};
        filter_input.height = ui::fixed(20);
        filter_input.width = ui::extend();

        file_container = {};
        file_container.quad_color = (0xFFFFFF00);
        file_container.padding = {10,10,10,10};
        file_container.height = ui::flexible();
        file_container.width = ui::extend();
        file_container.alignment =
        {
            .gap = {8, 8},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT,ui::AlignDir::DOWN},
        };


        option = {};
        option.quad_color = (0xFFFFFF06);
        option.corner_curves = {6,6,6,6};
        option.height = ui::flexible();
        option.width = ui::extend();
        option.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        option_list = {};
        option_list.quad_color = (0xFFFFFF10);
        option_list.padding = {5,5,5,5};
        option_list.height = ui::flexible();
        option_list.width = ui::extend();
        option_list.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        option_selection = {};
        option_selection.quad_color = 0xFFFFFF40;
        option_selection.text_align = ui::AlignText::Left;
        option_selection.padding = {8,0,0,0};
        option_selection.height = ui::fixed(20);
        option_selection.width = ui::extend();
        option_selection.font_size = 20;

        option_hot = {};
        option_hot.quad_color = 0xFFFFFF30;
        option_hot.corner_curves = {4,4,4,4};
        option_hot.text_align = ui::AlignText::Left;
        option_hot.padding = {8,0,0,0};
        option_hot.height = ui::fixed(20);
        option_hot.width = ui::extend();
        option_hot.font_size = 20;

        option_element = {};
        option_element.quad_color = 0xFFFFFF10;
        option_element.corner_curves = {4,4,4,4};
        option_element.text_align = ui::AlignText::Left;
        option_element.padding = {8,0,0,0};
        option_element.height = ui::fixed(20);
        option_element.width = ui::extend();
        option_element.font_size = 20;





        text_field_editing = {};
        text_field_editing.quad_color = 0x456E66FA;
        text_field_editing.corner_curves = {4,4,4,4};
        text_field_editing.text_align = ui::AlignText::Left;
        text_field_editing.padding = {8,0,0,0};
        text_field_editing.height = ui::fixed(20);
        text_field_editing.width = ui::extend();
        text_field_editing.font_size = 20;

        text_field_focus = {};
        text_field_focus.quad_color = 0xFFFFFF0A;
        text_field_focus.corner_curves = {4,4,4,4};
        text_field_focus.text_align = ui::AlignText::Left;
        text_field_focus.padding = {8,0,0,0};
        text_field_focus.height = ui::fixed(20);
        text_field_focus.width = ui::extend();
        text_field_focus.font_size = 20;

        text_field_idle = {};
        text_field_idle.quad_color = 0xFFFFFF05;
        text_field_idle.corner_curves = {4,4,4,4};
        text_field_idle.text_align = ui::AlignText::Left;
        text_field_idle.padding = {8,0,0,0};
        text_field_idle.height = ui::fixed(20);
        text_field_idle.width = ui::extend();
        text_field_idle.font_size = 20;

        hierarchy_main_bar  = {};
        hierarchy_main_bar.quad_color = (0x222222FF);
        hierarchy_main_bar.padding = {5,5,5,5};
        hierarchy_main_bar.height = ui::flexible();
        hierarchy_main_bar.width = ui::extend();
        hierarchy_main_bar.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };

        hierarchy_entity = {};
        hierarchy_entity.quad_color = (0xFFFFFF10);
        hierarchy_entity.corner_curves = {2,2,2,2};
        hierarchy_entity.padding = {4,4,4,4};
        hierarchy_entity.height = ui::fixed(24);
        hierarchy_entity.width = ui::extend();
        hierarchy_entity.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };

        hierarchy_entity = {};
        hierarchy_entity.quad_color = (0xFFFFFF03);
        hierarchy_entity.corner_curves = {4,4,4,4};
        hierarchy_entity.padding = {4,4,4,4};
        hierarchy_entity.height = ui::fixed(24);
        hierarchy_entity.width = ui::extend();
        hierarchy_entity.alignment =
        {
            .gap = {4, 4},
            .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT},
        };
        hierarchy_entity_selected = hierarchy_entity;



        close_butn = {};
        close_butn.height = ui::fixed(18);
        close_butn.width = ui::fixed(18);
        close_butn.quad_color = 0xFFFFFF20;
        close_butn.text_align =  ui::AlignText::Center;
        close_butn.font_size = 20;
        close_butn.corner_curves = {2,2,2,2};

        slider_knob = {};
        slider_knob.height = ui::fixed(15);
        slider_knob.width = ui::fixed(15);
        slider_knob.quad_color = 0xFF22FFFF;
        slider_knob.text_align =  ui::AlignText::Center;

        slider_track = {};
        slider_track.height = ui::fixed(15);
        slider_track.width = ui::extend();
        slider_track.quad_color = 0xFFFFFF0B;
        slider_track.text_align = ui::AlignText::Center;


        slide_bar = {};
        slide_bar.quad_color = 0xFF22FFFF;
        slide_bar.position = ui::Positioning::Absolute;
        slide_bar.height = ui::fixed(14);
        slide_bar.width = ui::fixed(14);
        slide_bar = {};
        slide_bar.quad_color = 0xFFFFFF20;
        slide_bar.height = ui::fixed(14);
        slide_bar.width = ui::extend();
        slide_bar.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };
        slide_bar_value = {};
        slide_bar_value.position = ui::Positioning::Absolute;
        slide_bar_value.quad_color = 0xFFFFFF00;
        slide_bar_value.text_align = ui::AlignText::Center;
        slide_bar_value.height = ui::fixed(14);
        slide_bar_value.width = ui::percent(100);
        slide_bar_value.font_size = 20;


        collapsable_header = {};
        collapsable_header.quad_color = (0x23232300);
        collapsable_header.height = ui::fixed(20);
        collapsable_header.width = ui::extend();
        collapsable_header.padding = {6,0,6,0};
        collapsable_header.alignment.gap = {4,0};

        list = {};
        list.quad_color = (0x232323FF);
        list.height = ui::flexible();
        list.width = ui::extend();
        list.alignment =
        {
            .gap = {2,2},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        list_elem = {};
        list_elem.corner_curves = {4,4,4,4};
        list_elem.height = ui::fixed(20);
        list_elem.width = ui::extend();
        list_elem.quad_color = 0xFFFFFF10;
        list_elem.font_size = 20;

        list_elem_focus = {};
        list_elem_focus.corner_curves = {4,4,4,4};
        list_elem_focus.height = ui::fixed(20);
        list_elem_focus.width = ui::extend();
        list_elem_focus.quad_color = 0xFFFFFF30;
        list_elem_focus.font_size = 20;



        select = {};
        select.quad_color = (0x232323FF);
        select.height = ui::flexible();
        select.width = ui::extend();
        select.alignment =
        {
            .gap = {2,2},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN},
        };

        select_elem = {};
        select_elem.corner_curves = {4,4,4,4};
        select_elem.height = ui::fixed(20);
        select_elem.width = ui::extend();
        select_elem.quad_color = 0xFFFFFF10;
        select_elem.font_size = 20;

        select_elem_focus = {};
        select_elem_focus.corner_curves = {4,4,4,4};
        select_elem_focus.height = ui::fixed(20);
        select_elem_focus.width = ui::extend();
        select_elem_focus.quad_color = 0xFFFFFF30;
        select_elem_focus.font_size = 20;

        hierarchy_panel = {};
        hierarchy_panel.quad_color = (0x433333FF);
        hierarchy_panel.height = ui::extend();
        hierarchy_panel.width = ui::fixed(400);
        hierarchy_panel.alignment =
        {
            .content = {ui::AlignX::CENTER, ui::AlignY::TOP},
            .direction = {ui::AlignDir::DOWN}
        };

        viewport_panel = {};
        viewport_panel.height = ui::extend();
        viewport_panel.width = ui::extend();
        viewport_panel.quad_color = (0x222222FF);
        viewport_panel.alignment =
        {
            .content = {ui::AlignX::RIGHT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT}
        };


        numeric = {};
        numeric.quad_color = (0xFFFFFF0B);
        numeric.height = ui::fixed(18);
        numeric.width = ui::extend();
        numeric.padding = {8,2,8,2};
        numeric.alignment =
        {
            .gap = {2,2},
            .content = {ui::AlignX::LEFT, ui::AlignY::TOP},
            .direction = {ui::AlignDir::RIGHT}
        };

        numeric_label = {};
        numeric_label.quad_color = (0xFFFFFF00);
        numeric_label.text_align = ui::AlignText::Right;
        numeric_label.height = ui::fixed(18);
        numeric_label.width = ui::fixed(100);
        numeric_label.font_size = 20;
        numeric_label.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };

        numeric_value = {};
        numeric_value.quad_color = (0xFFFFFF00);
        numeric_value.text_align = ui::AlignText::Left;
        numeric_value.corner_curves = {5,5,5,5};
        numeric_value.height = ui::fixed(18);
        numeric_value.width = ui::extend();
        numeric_value.font_size = 20;
        numeric_value.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };

        numeric_focus = {};
        numeric_focus.quad_color = (0xFFFFFF1B);
        numeric_focus.text_align = ui::AlignText::Left;
        numeric_focus.corner_curves = {5,5,5,5};
        numeric_focus.height = ui::fixed(18);
        numeric_focus.width = ui::extend();
        numeric_focus.font_size = 20;
        numeric_focus.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT}
        };


        folder_icon = {};
        folder_icon.quad_color = (0xFFFFFF1B);
        folder_icon.text_align = ui::AlignText::Left;
        folder_icon.corner_curves = {5,5,5,5};
        folder_icon.height = ui::fixed(40);
        folder_icon.width = ui::fixed(60);
        folder_icon.font_size = 20;

        file_icon = {};
        file_icon.quad_color = (0xFFFFFF1B);
        file_icon.text_align = ui::AlignText::Left;
        file_icon.corner_curves = {5,5,5,5};
        file_icon.height = ui::fixed(40);
        file_icon.width = ui::fixed(20);
        file_icon.font_size = 20;

        selected_item = {};
        selected_item.quad_color = (0xFFFFFF1B);
        selected_item.text_align = ui::AlignText::Left;
        selected_item.corner_curves = {5,5,5,5};
        selected_item.height = ui::fixed(40);
        selected_item.width = ui::fixed(40);
        selected_item.font_size = 20;

        selection_box = {};
        selection_box.quad_color = (0xFFFFFF1B);
        selection_box.text_align = ui::AlignText::Left;
        selection_box.corner_curves = {5,5,5,5};
        selection_box.height = ui::fixed(40);
        selection_box.width = ui::fixed(40);
        selection_box.font_size = 20;

        tooltip = {};
        tooltip.quad_color = (0xCC7F41FF);
        tooltip.text_align = ui::AlignText::Left;
        tooltip.corner_curves = {5,5,5,5};
        tooltip.padding = {10,10,10,10};
        tooltip.height = ui::fixed(150);
        tooltip.width = ui::fixed(250);
        tooltip.font_size = 20;
        tooltip.alignment =
        {
            .content = {ui::AlignX::LEFT, ui::AlignY::CENTER},
            .direction = {ui::AlignDir::RIGHT,ui::AlignDir::DOWN},
        };


//        window = {};
//        tooltip.quad_color = 0x18141DFF;
//        window.padding = {10,10,10,10};
//        window.alignment = ui::Alignment
//            {
//                .origin = {ui::AlignX::LEFT, ui::AlignY::TOP},
//                .direction = ui::AlignDir::DOWN,
//            },
//            .position = ui::Positioning::Independent,
//            .clip_overflow = true
//        }
    }
}
