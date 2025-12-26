//
//  ui-resizer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/10/25.
//

#include "ui-layout.hpp"
#include "ui-resizer.hpp"

namespace kege::ui{

    void Resizer::resolveSizing(uint32_t parent, const Sizing& sizing, float& size, float available_space, int& extend_count )
    {
        switch (sizing.type)
        {
            case kege::ui::SIZE_UNDEFINED:
            {
                size = 0.f;
                break;
            }

            case kege::ui::SIZE_FIXED:
            {
                size = sizing.size;
                break;
            }

            case kege::ui::SIZE_PERCENT:
            {
                size = available_space * sizing.size;
                break;
            }

            case kege::ui::SIZE_EXTEND:
            {
                if (parent == 0)
                    size = available_space;
                else
                {
                    extend_count += 1;
                    size = 0.f;
                }
                break;
            }

            case kege::ui::SIZE_FLEXIBLE:
            default: break;
        }
    }

    void Resizer::calcHorizontalExtent( Layout& layout, uint32_t widget_index )
    {
        Widget* widget = layout[ widget_index ];
        float gapsum = (widget->count > 1)? widget->style->gap.width * (widget->count - 1) : 0.f;
        float h_pad = (widget->style->padding.left + widget->style->padding.right);
        float v_pad = (widget->style->padding.above + widget->style->padding.below);

        Extent boundary;
        boundary.width  = kege::max(0.f, widget->rect.width - h_pad - gapsum);
        boundary.height = kege::max(0.f, widget->rect.height - v_pad);

        Extent row = {};
        Extent max = {};

        int extend_width_count = 0;
        int extend_height_count = 0;

        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            resolveSizing(child->parent, child->style->width, child->rect.width, boundary.width, extend_width_count );
            resolveSizing(child->parent, child->style->height, child->rect.height, boundary.height, extend_height_count );

            if ( child->style->position == Positioning::Independent || child->style->position == Positioning::Absolute )
            {
                continue;
            }

            row.height = kege::max(max.height, child->rect.height);
            row.width += child->rect.width;

            if (row.width + gapsum > boundary.width && widget->style->align.wrap_around)
            {
                max.height += row.height;
                row.width = child->rect.width;
            }
            max.width = kege::max(max.width, row.width);
        }
        max.height += row.height;

        if (extend_height_count || extend_width_count)
        {
            if ( row.height == 0) row.height = boundary.height;

            float child_width = (boundary.width - max.width) / float(extend_width_count);
            for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
            {
                Widget* child = layout[ child_index ];
                if( child->style->width.type == ui::SizingType::SIZE_EXTEND )
                {
                    child->rect.width = child_width;
                }

                if( child->style->height.type == ui::SizingType::SIZE_EXTEND )
                {
                    child->rect.height = row.height;
                }
            }
        }

        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            if (0 < child->count)
            {
                if (child->style->align.direction == ui::AlignDir::HORIZONTAL)
                {
                    calcHorizontalExtent( layout, child_index );
                }
                else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
                {
                    calcVerticalExtent( layout, child_index );
                }
            }
        }
    }

    void Resizer::calcVerticalExtent( Layout& layout, uint32_t widget_index )
    {
        Widget* widget = layout[ widget_index ];
        float gapsum = (widget->count > 1)? widget->style->gap.height * (widget->count - 1) : 0.f;
        float h_pad = (widget->style->padding.left + widget->style->padding.right);
        float v_pad = (widget->style->padding.above + widget->style->padding.below);

        Extent boundary;
        boundary.width  = kege::max(0.f, widget->rect.width - h_pad);
        boundary.height = kege::max(0.f, widget->rect.height - v_pad - gapsum);

        Extent extent = {};
        Extent max = {};

        int extend_width_count = 0;
        int extend_height_count = 0;

        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            resolveSizing(child->parent, child->style->width, child->rect.width, boundary.width, extend_width_count );
            resolveSizing(child->parent, child->style->height, child->rect.height, boundary.height, extend_height_count );

            if ( child->style->position == Positioning::Independent || child->style->position == Positioning::Absolute )
            {
                continue;
            }

            extent.width = kege::max(max.width, child->rect.width);
            extent.height += child->rect.height;

            if (widget->style->align.wrap_around && extent.height + gapsum > boundary.height)
            {
                extent.height = child->rect.height;
                max.width += extent.width;
            }
            max.height = kege::max(max.height, extent.height);
        }
        max.width += extent.width;

        if (extend_height_count || extend_width_count)
        {
            if ( extent.width == 0) extent.width = boundary.width;
            float child_height = (boundary.height - max.height) / float(extend_height_count);

            for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
            {
                Widget* child = layout[ child_index ];
                if( child->style->height.type == ui::SizingType::SIZE_EXTEND )
                {
                    child->rect.height = child_height;
                }

                if( child->style->width.type == ui::SizingType::SIZE_EXTEND )
                {
                    child->rect.width = extent.width;
                }
            }
        }

        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            if (0 < child->count)
            {
                if (child->style->align.direction == ui::AlignDir::HORIZONTAL)
                {
                    calcHorizontalExtent( layout, child_index );
                }
                else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
                {
                    calcVerticalExtent( layout, child_index );
                }
            }
        }
    }

    void Resizer::calcHorizontalFlex( Layout& layout, uint32_t widget_index )
    {
        Extent flex = {};
        Widget* widget = layout[ widget_index ];
        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            if (0 < child->count)
            {
                if (child->style->align.direction == ui::AlignDir::HORIZONTAL)
                {
                    calcHorizontalFlex( layout, child_index );
                }
                else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
                {
                    calcVerticalFlex( layout, child_index );
                }
            }
            flex.width += child->rect.width;
            flex.height = max(flex.height, child->rect.height);
        }

        vec2 text_extent = {};
        if(!widget->text.text.empty())
        {
            text_extent = layout.computeExtent(widget->style->font_size, widget->text.text.c_str());
        }

        if(widget->style->width.type == kege::ui::SIZE_FLEXIBLE)
        {
            widget->rect.width = (widget->count > 1)? widget->style->gap.width * (widget->count - 1) : 0.f;
            widget->rect.width += flex.width + text_extent.x + widget->style->padding.left + widget->style->padding.right;
        }

        if(widget->style->height.type == kege::ui::SIZE_FLEXIBLE)
        {
            widget->rect.height = flex.height + text_extent.y + widget->style->padding.above + widget->style->padding.below;
        }
    }

    void Resizer::calcVerticalFlex( Layout& layout, uint32_t widget_index )
    {
        Extent flex = {};
        Widget* widget = layout[ widget_index ];
        for (NodeIndex child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            if (0 < child->count)
            {
                if (child->style->align.direction == ui::AlignDir::HORIZONTAL)
                {
                    calcHorizontalFlex( layout, child_index );
                }
                else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
                {
                    calcVerticalFlex( layout, child_index );
                }
            }
            flex.height += child->rect.height;
            flex.width = max(flex.width, child->rect.width);
        }

        vec2 text_extent = {};
        if(!widget->text.text.empty())
        {
            text_extent = layout.computeExtent(widget->style->font_size, widget->text.text.c_str());
        }

        if(widget->style->width.type == kege::ui::SIZE_FLEXIBLE)
        {
            widget->rect.width = flex.width + text_extent.x + widget->style->padding.left + widget->style->padding.right;
        }

        if(widget->style->height.type == kege::ui::SIZE_FLEXIBLE)
        {
            widget->rect.height = (widget->count > 1)? widget->style->gap.height * (widget->count - 1) : 0.f;
            widget->rect.height += flex.height + text_extent.y + widget->style->padding.above + widget->style->padding.below;
        }
    }

    void Resizer::resize( Layout& layout, uint32_t widget_index )
    {
        int extendable_count;

        Widget* widget = layout[ widget_index ];
        resolveSizing(widget->parent, widget->style->width,  widget->rect.width,  layout.getWidth(),  extendable_count );
        resolveSizing(widget->parent, widget->style->height, widget->rect.height, layout.getHeight(), extendable_count );

        if (widget->style->align.direction == ui::AlignDir::HORIZONTAL)
        {
            calcHorizontalFlex( layout, widget_index );
            calcHorizontalExtent( layout, widget_index );
        }
        else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
        {
            calcVerticalFlex( layout, widget_index );
            calcVerticalExtent( layout, widget_index );
        }
    }

    void Resizer::resize( Layout& layout )
    {
        for (int root_index = 0; root_index < layout._root_count; ++root_index)
        {
            resize( layout, layout._roots[ root_index ] );
        }
    }

}
