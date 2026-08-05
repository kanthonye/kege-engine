//
//  theme.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 6/10/26.
//

#ifndef theme_hpp
#define theme_hpp

#include "../../core/graphics/components/gui/ui-gui.hpp"

namespace kege{

    struct Theme
    {
        Theme();

        kege::ui::Elem close_butn;
        kege::ui::Elem panel;
        kege::ui::Elem scroll_container;
        kege::ui::Elem panel_transparnt;
        kege::ui::Elem hierarchy_panel;
        kege::ui::Elem viewport_panel;
        kege::ui::Elem padded_list;

        kege::ui::Elem list;
        kege::ui::Elem list_elem;
        kege::ui::Elem list_elem_focus;

        kege::ui::Elem select;
        kege::ui::Elem select_elem;
        kege::ui::Elem select_elem_focus;

        kege::ui::Elem label;
        kege::ui::Elem fitted_label;

        kege::ui::Elem radio;
        kege::ui::Elem radio_active;
        kege::ui::Elem button;
        kege::ui::Elem disabled_button;

        kege::ui::Elem slider_knob;
        kege::ui::Elem slider_track;
        kege::ui::Elem slider_name;
        kege::ui::Elem slider_value;
        kege::ui::Elem slider_row;
        kege::ui::Elem slider_container;
        kege::ui::Elem slider_inner_container;
        kege::ui::Elem slider_range;

        kege::ui::Elem slide_bar_track;
        kege::ui::Elem slide_bar;
        kege::ui::Elem slide_bar_value;

        kege::ui::Elem row;
        kege::ui::Elem row_wrap;
        kege::ui::Elem column;
        kege::ui::Elem x_seperator;
        kege::ui::Elem y_seperator;

        kege::ui::Elem numeric;
        kege::ui::Elem numeric_focus;
        kege::ui::Elem numeric_value;
        kege::ui::Elem numeric_label;

        kege::ui::Elem scrubber;
        kege::ui::Elem scrubber_label;
        kege::ui::Elem scrubber_focus;
        kege::ui::Elem scrubber_row;

        kege::ui::Elem collapsable_header;
        kege::ui::Elem collapsable_element;
        kege::ui::Elem collapsable_element_hl;
        kege::ui::Elem collapsable_removable_header;

        kege::ui::Elem container_header;
        kege::ui::Elem container_body;

        kege::ui::Elem card;
        kege::ui::Elem card2;

        kege::ui::Elem folder_icon;
        kege::ui::Elem selected_item;
        kege::ui::Elem selection_box;
        kege::ui::Elem file_icon;
        kege::ui::Elem tooltip;

        kege::ui::Elem option;
        kege::ui::Elem option_selection;
        kege::ui::Elem option_list;
        kege::ui::Elem option_hot;
        kege::ui::Elem option_element;

        kege::ui::Elem navbar;
        kege::ui::Elem filter_container;
        kege::ui::Elem filter_input;
        kege::ui::Elem broswer_file_path;
        kege::ui::Elem file_container;

        kege::ui::Elem text_field_editing;
        kege::ui::Elem text_field_focus;
        kege::ui::Elem text_field_idle;

        kege::ui::Elem hierarchy_main_bar;
        kege::ui::Elem hierarchy_entity;
        kege::ui::Elem hierarchy_entity_selected;

        kege::ui::Elem tab;
        kege::ui::Elem tab_elem;
        kege::ui::Elem tab_label;
        kege::ui::Elem tab_elem_focus;

        kege::ui::Elem dock;
        kege::ui::Elem ghost;
        kege::ui::Elem group;
    };


}
#endif /* theme_hpp */
