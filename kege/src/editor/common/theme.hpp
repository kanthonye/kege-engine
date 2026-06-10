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
        kege::ui::Style close_butn;
        kege::ui::Style panel;
        kege::ui::Style scroll_container;
        kege::ui::Style panel_transparnt;
        kege::ui::Style hierarchy_panel;
        kege::ui::Style viewport_panel;
        kege::ui::Style padded_list;

        kege::ui::Style list;
        kege::ui::Style list_elem;
        kege::ui::Style list_elem_focus;

        kege::ui::Style select;
        kege::ui::Style select_elem;
        kege::ui::Style select_elem_focus;

        kege::ui::Style label;
        kege::ui::Style fitted_label;

        kege::ui::Style radio;
        kege::ui::Style radio_active;
        kege::ui::Style button;
        kege::ui::Style disabled_button;

        kege::ui::Style slider_knob;
        kege::ui::Style slider_track;
        kege::ui::Style slider_name;
        kege::ui::Style slider_value;
        kege::ui::Style slider_row;
        kege::ui::Style slider_container;
        kege::ui::Style slider_inner_container;
        kege::ui::Style slider_range;

        kege::ui::Style slide_bar_track;
        kege::ui::Style slide_bar;
        kege::ui::Style slide_bar_value;

        kege::ui::Style row;
        kege::ui::Style row_wrap;
        kege::ui::Style column;
        kege::ui::Style x_seperator;
        kege::ui::Style y_seperator;

        kege::ui::Style numeric;
        kege::ui::Style numeric_focus;
        kege::ui::Style numeric_value;
        kege::ui::Style numeric_label;

        kege::ui::Style scrubber;
        kege::ui::Style scrubber_label;
        kege::ui::Style scrubber_focus;
        kege::ui::Style scrubber_row;

        kege::ui::Style collapsable_header;
        kege::ui::Style collapsable_element;
        kege::ui::Style collapsable_element_hl;
        kege::ui::Style collapsable_removable_header;

        kege::ui::Style container_header;
        kege::ui::Style container_body;

        kege::ui::Style card;
        kege::ui::Style card2;

        kege::ui::Style folder_icon;
        kege::ui::Style selected_item;
        kege::ui::Style selection_box;
        kege::ui::Style file_icon;
        kege::ui::Style tooltip;

        kege::ui::Style option;
        kege::ui::Style option_selection;
        kege::ui::Style option_list;
        kege::ui::Style option_hot;
        kege::ui::Style option_element;

        kege::ui::Style navbar;
        kege::ui::Style filter_container;
        kege::ui::Style filter_input;
        kege::ui::Style broswer_file_path;
        kege::ui::Style file_container;

        kege::ui::Style text_field_editing;
        kege::ui::Style text_field_focus;
        kege::ui::Style text_field_idle;

        kege::ui::Style hierarchy_main_bar;
        kege::ui::Style hierarchy_entity;
        kege::ui::Style hierarchy_entity_selected;

        kege::ui::Style tab;
        kege::ui::Style tab_elem;
        kege::ui::Style tab_label;
        kege::ui::Style tab_elem_focus;

        kege::ui::Style dock;
        kege::ui::Style ghost;
        kege::ui::Style group;
        Theme();
    };


}
#endif /* theme_hpp */
