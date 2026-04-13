//
//  ui-resizer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/10/25.
//

#include "ui-layout.hpp"
#include "ui-aligner.hpp"
#include "ui-resizer.hpp"

namespace kege::ui{

    void resolveSizing(uint32_t parent, const Sizing& sizing, float& size, float available_space, int& extend_count )
    {
        switch (sizing.type)
        {
            case kege::ui::SizingType::Fixed:
            {
                //size += sizing.size;
                break;
            }

            case kege::ui::SizingType::Percent:
            {
                size += available_space * sizing.size;
                break;
            }

            case kege::ui::SizingType::Extend:
            {
                if (parent == 0)
                    size += available_space;
                else
                {
                    extend_count += 1;
                    size = 0.f;
                }
                break;
            }

            case kege::ui::SizingType::None:
            case kege::ui::SizingType::Flexible:
            default: break;
        }
    }

    inline bool integrate
    (
        float main_size,
        float cross_size,
        float main_limit,
        const AlignWrap& wrap,
        float& main_sum,
        float& cross_max,
        float& extent_main,
        float& extent_cross
    )
    {
        cross_max = kege::max(cross_max, cross_size);
        main_sum += main_size;

        bool newline = false;
        if (main_sum > main_limit && wrap.enable)
        {
            extent_cross += cross_max;
            main_sum = main_size;
            newline = true;
        }

        extent_main = kege::max(extent_main, main_sum);
        return newline;
    }

    void Resizer::resolveExtendSizes( Layout& layout, uint32_t widget_index )
    {
        Widget* widget = layout[ widget_index ];

        const Padding& padding = widget->padding;

        float gapsum = (widget->count > 1)? widget->gap.width * (widget->count - 1) : 0.f;
        float h_pad = (padding.left + padding.right);
        float v_pad = (padding.above + padding.below);

        Extent extent = {};
        Extent run = {};

        uint32_t child_index = layout.head( widget_index );
        int extend_count[2] = {0,0};

        int line_count = 0;

        if (widget->alignment.direction == ui::AlignDir::LEFT || widget->alignment.direction == ui::AlignDir::RIGHT)
        {
            Extent boundary;
            boundary.width  = kege::max(0.f, widget->rect.width - h_pad - gapsum);
            boundary.height = kege::max(0.f, widget->rect.height - v_pad);

            while ( child_index != 0 )
            {
                Widget* child = layout[ child_index ];
                if (child->width.type != SizingType::None)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->width,
                        child->rect.width,
                        boundary.width,
                        extend_count[0]
                    );
                }
                if (child->height.type != SizingType::None)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->height,
                        child->rect.height,
                        boundary.height,
                        extend_count[1]
                    );
                }

                if ( child->position == Positioning::Independent || child->position == Positioning::Absolute )
                {
                    child_index = layout.next( child_index );
                    continue;
                }

                bool newline = integrate
                (
                    child->rect.width, child->rect.height,
                    boundary.width, widget->alignment.wrap,
                    run.width, run.height, extent.width, extent.height
                );
                if (newline)line_count += 1;

                child_index = layout.next( child_index );
            }

            if (extend_count[0] || extend_count[1])
            {
                if ( extent.height == 0) extent.height = boundary.height;
                else extent.height += run.height;

                float child_width = (boundary.width - extent.width) / float(extend_count[0]);
                for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
                {
                    Widget* child = layout[ child_index ];
                    
                    if( child->width.type == ui::SizingType::Extend )
                    {
                        child->rect.width += child_width;
                    }

                    if( child->height.type == ui::SizingType::Extend )
                    {
                        child->rect.height += extent.height;
                    }
                }
            }
        }
        else // 
        {
            Extent boundary;
            boundary.width  = kege::max(0.f, widget->rect.width - h_pad);
            boundary.height = kege::max(0.f, widget->rect.height - v_pad - gapsum);

            while ( child_index != 0 )
            {
                Widget* child = layout[ child_index ];
                if (child->width.type != SizingType::None)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->width,
                        child->rect.width,
                        boundary.width,
                        extend_count[0]
                    );
                }
                if (child->height.type != SizingType::None)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->height,
                        child->rect.height,
                        boundary.height,
                        extend_count[1]
                    );
                }

                if ( child->position == Positioning::Independent || child->position == Positioning::Absolute )
                {
                    child_index = layout.next( child_index );
                    continue;
                }
                bool newline = integrate
                (
                    child->rect.height, child->rect.width,
                    boundary.width, widget->alignment.wrap,
                    run.height, run.width, extent.height, extent.width
                );
                if (newline) line_count += 1;
                
                child_index = layout.next( child_index );
            }

            if (extend_count[0] || extend_count[1])
            {
                if ( extent.width == 0) extent.width = boundary.width;
                else extent.width += run.width;

                float child_height = (boundary.height - extent.height) / float(extend_count[1]);
                for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
                {
                    Widget* child = layout[ child_index ];

                    if( child->width.type == ui::SizingType::Extend )
                    {
                        child->rect.width += extent.width;
                    }

                    if( child->height.type == ui::SizingType::Extend )
                    {
                        child->rect.height += child_height;
                    }
                }
            }
        }

        for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            Widget* child = layout[ child_index ];
            if (0 < child->count)
            {
                resolveExtendSizes( layout, child_index );
            }
        }
    }

    void Resizer::resolveFlexSizes( Layout& layout, uint32_t widget_index, const Extent& boundary )
    {
        Widget* widget = layout[ widget_index ];
        for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            resolveFlexSizes( layout, child_index, boundary );
        }

        if (widget->width.type == ui::SizingType::Flexible || widget->height.type == ui::SizingType::Flexible)
        {
            const Padding& padding = widget->padding;
            float h_pad = (padding.left + padding.right);
            float v_pad = (padding.above + padding.below);

            Extent flex = {};
            if( widget->text.ptr && (widget->width.type == ui::SizingType::Flexible || widget->height.type == ui::SizingType::Flexible) )
            {
                if (widget->width.type == ui::SizingType::Flexible)
                {
                    flex.width += widget->text.width + h_pad;
                }
                if (widget->height.type == ui::SizingType::Flexible)
                {
                    flex.height += widget->text.height + v_pad;
                }
            }

            int line_count = 0;
            float main_sum = 0.f;
            float cross_max = 0.f;
            bool newline = false;

            uint32_t child_index = widget->head;
            while ( child_index != 0 )
            {
                Widget* child = layout[ child_index ];

                if (widget->width.type == ui::SizingType::Percent)
                {
                    child_index = layout.next( child_index );
                    continue;
                }
                if (widget->height.type == ui::SizingType::Percent)
                {
                    child_index = layout.next( child_index );
                    continue;
                }
                if (widget->alignment.direction == ui::AlignDir::LEFT || widget->alignment.direction == ui::AlignDir::RIGHT)
                {
                    newline = integrate
                    (
                        child->rect.width, child->rect.height,
                        boundary.width, widget->alignment.wrap,
                        main_sum, cross_max, flex.width, flex.height
                    );
                    if (newline) line_count += 1;

                    child_index = layout.next( child_index );
                }
                else
                {
                    newline = integrate
                    (
                        child->rect.height, child->rect.width,
                        boundary.height, widget->alignment.wrap,
                        main_sum, cross_max, flex.height, flex.width
                    );
                    if (newline) line_count += 1;

                    child_index = layout.next( child_index );
                }
            }

            if (widget->alignment.direction == ui::AlignDir::LEFT || widget->alignment.direction == ui::AlignDir::RIGHT)
            {
                flex.height += cross_max;

                if (widget->width.type == ui::SizingType::Flexible)
                {
                    float gapsum = (widget->count > 1)? widget->gap.width * (widget->count - 1) : 0.f;
                    widget->rect.width += flex.width + h_pad + gapsum;
                }
                if (widget->height.type == ui::SizingType::Flexible)
                {
                    widget->rect.height += flex.height + v_pad;
                }
            }
            else
            {
                flex.width += cross_max;

                if (widget->width.type == ui::SizingType::Flexible)
                {
                    widget->rect.width += flex.width + h_pad;
                }
                if (widget->height.type == ui::SizingType::Flexible)
                {
                    float gapsum = (widget->count > 1)? widget->gap.height * (widget->count - 1) : 0.f;
                    widget->rect.height += flex.height + v_pad + gapsum;
                }
            }
        }
    }

    void Resizer::resize( Layout& layout, uint32_t widget_index )
    {
        int extendable_count;
        Widget* widget = layout[ widget_index ];
        resolveFlexSizes( layout, widget_index, Extent{ widget->rect.width, widget->rect.height });
        resolveExtendSizes( layout, widget_index );
    }

}
