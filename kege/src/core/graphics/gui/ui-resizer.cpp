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

    inline void integrate
    (
        float main_size,
        float main_gap,
        float cross_size,
        float cross_gap,
        float main_limit,
        bool  wrap,
        float& main_sum,
        float& cross_max,
        float& extent_main,
        float& extent_cross
    )
    {
        cross_max = kege::max(cross_max, cross_size);
        main_sum += main_size;

        if (main_sum > main_limit && wrap)
        {
            extent_cross += cross_max + cross_gap;
            main_sum = main_size;
        }

        extent_main = kege::max(extent_main, main_sum);
        main_sum += main_gap;
    }

    void resolveExtendSizes( Layout& layout, uint32_t widget_index )
    {
        Widget* widget = layout[ widget_index ];

        Alignment alignment = getAlignment(widget);
        Extent gap = (widget->style != nullptr)? widget->style->gap : Extent{};
        const Padding& padding = (widget->style != nullptr) ? widget->style->padding : widget->padding;

        Extent extent = {};
        Extent run = {};

        float gapsum = (widget->count > 1)? gap.width * (widget->count - 1) : 0.f;
        float h_pad = (padding.left + padding.right);
        float v_pad = (padding.above + padding.below);

        uint32_t child_index = layout.head( widget_index );
        int extend_count[2] = {0,0};

        if (alignment.direction == ui::AlignDir::HORIZONTAL)
        {
            Extent boundary;
            boundary.width  = kege::max(0.f, widget->rect.width - h_pad - gapsum);
            boundary.height = kege::max(0.f, widget->rect.height - v_pad);

            while ( child_index != 0 )
            {
                Widget* child = layout[ child_index ];
                if (child->style != nullptr)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->style->width,
                        child->rect.width,
                        boundary.width,
                        extend_count[0]
                    );
                    resolveSizing
                    (
                        child->parent,
                        child->style->height,
                        child->rect.height,
                        boundary.height,
                        extend_count[1]
                    );
                }
                Positioning positioning = (child->style)? child->style->position: Positioning::Relative;
                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
                {
                    continue;
                }
                integrate
                (
                    child->rect.width,  gap.width,
                    child->rect.height, gap.height,
                    boundary.width,     alignment.wrap_around,
                    run.width, run.height, extent.width, extent.height
                );
                child_index = layout.next( child_index );
            }
            extent.height += run.height;

            if (extend_count[0] || extend_count[1])
            {
                if ( extent.height == 0) extent.height = boundary.height;

                float child_width = (boundary.width - extent.width) / float(extend_count[0]);
                for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
                {
                    Widget* child = layout[ child_index ];
                    if (child->style == nullptr) continue;
                    
                    if( child->style->width.type == ui::SizingType::Extend )
                    {
                        child->rect.width += child_width;
                    }

                    if( child->style->height.type == ui::SizingType::Extend )
                    {
                        child->rect.height += extent.height;
                    }
                }
            }
        }
        else // if (widget->style->align.direction == ui::AlignDir::VERTICAL)
        {
            Extent boundary;
            boundary.width  = kege::max(0.f, widget->rect.width - h_pad);
            boundary.height = kege::max(0.f, widget->rect.height - v_pad - gapsum);

            while ( child_index != 0 )
            {
                Widget* child = layout[ child_index ];
                if (child->style != nullptr)
                {
                    resolveSizing
                    (
                        child->parent,
                        child->style->width,
                        child->rect.width,
                        boundary.width,
                        extend_count[0]
                    );
                    resolveSizing
                    (
                        child->parent,
                        child->style->height,
                        child->rect.height,
                        boundary.height,
                        extend_count[1]
                    );
                }
                Positioning positioning = (child->style)? child->style->position: Positioning::Relative;
                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
                {
                    continue;
                }
                integrate
                (
                    child->rect.height, gap.height,
                    child->rect.width, gap.width,
                    boundary.width, alignment.wrap_around,
                    run.height, run.width, extent.height, extent.width
                );
                child_index = layout.next( child_index );
            }
            extent.width += run.width;

            if (extend_count[0] || extend_count[1])
            {
                if ( extent.width == 0) extent.width = boundary.width;

                float child_height = (boundary.height - extent.height) / float(extend_count[1]);
                for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
                {
                    Widget* child = layout[ child_index ];
                    if (child->style == nullptr) continue;

                    if( child->style->width.type == ui::SizingType::Extend )
                    {
                        child->rect.width += extent.width;
                    }

                    if( child->style->height.type == ui::SizingType::Extend )
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

    void resolveFlexSizes( Layout& layout, uint32_t widget_index, const Extent& boundary )
    {
        Widget* widget = layout[ widget_index ];
        for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
        {
            resolveFlexSizes( layout, child_index, boundary );
        }
        if( widget->style != nullptr )
        {
            if (widget->style->width.type == ui::SizingType::Flexible || widget->style->height.type == ui::SizingType::Flexible)
            {
                const Padding& padding = (widget->style != nullptr) ? widget->style->padding : widget->padding;
                float h_pad = (padding.left + padding.right);
                float v_pad = (padding.above + padding.below);

                Extent flex = {};
                if( widget->text.ptr && (widget->style->width.type == ui::SizingType::Flexible || widget->style->height.type == ui::SizingType::Flexible) )
                {
                    if (widget->style->width.type == ui::SizingType::Flexible)
                    {
                        flex.width += widget->text.width + h_pad;
                    }
                    if (widget->style->height.type == ui::SizingType::Flexible)
                    {
                        flex.height += widget->text.height + v_pad;
                    }
                }

//                Extent extent;
//                if ( widget->style->align.wrap_around )
//                {
//                    if (widget->style->align.direction == ui::AlignDir::HORIZONTAL)
//                    {
//                        float gapsum = (widget->count > 1)? widget->style->gap.width * (widget->count - 1) : 0.f;
//                        extent.width  = kege::max(0.f, widget->rect.width - h_pad - gapsum);
//                        extent.height = kege::max(0.f, widget->rect.height - v_pad);
//                    }
//                    else
//                    {
//                        float gapsum = (widget->count > 1)? widget->style->gap.width * (widget->count - 1) : 0.f;
//                        extent.height = kege::max(0.f, widget->rect.height - v_pad - gapsum);
//                        extent.width  = kege::max(0.f, widget->rect.width - h_pad);
//                    }
//                }
//                else
//                {
//                    extent = boundary;
//                }

                float main_sum = 0.f;
                float cross_max = 0.f;

                for (uint32_t child_index = widget->head; child_index != 0 ; child_index = layout.next( child_index ) )
                {
                    Widget* child = layout[ child_index ];

                    if (widget->style->width.type == ui::SizingType::Percent)
                    {
                        continue;
                    }
                    if (widget->style->height.type == ui::SizingType::Percent)
                    {
                        continue;
                    }

                    if (widget->style->align.direction == ui::AlignDir::HORIZONTAL)
                    {
                        integrate
                        (
                            child->rect.width,  widget->style->gap.width,
                            child->rect.height, widget->style->gap.height,
                            boundary.width,     widget->style->align.wrap_around,
                            main_sum, cross_max, flex.width, flex.height
                        );
                    }
                    else
                    {
                        integrate
                        (
                            child->rect.height, widget->style->gap.height,
                            child->rect.width, widget->style->gap.width,
                            boundary.height, widget->style->align.wrap_around,
                            main_sum, cross_max, flex.height, flex.width
                        );
                    }
                }

                if (widget->style->align.direction == ui::AlignDir::HORIZONTAL)
                {
                    flex.height += cross_max;
                }
                else
                {
                    flex.width += cross_max;
                }

                if (widget->style->width.type == ui::SizingType::Flexible)
                {
                    widget->rect.width += flex.width + h_pad;
                }

                if (widget->style->height.type == ui::SizingType::Flexible)
                {
                    widget->rect.height += flex.height + v_pad;
                }
            }
        }
    }

    void Resizer::resize( Layout& layout, uint32_t widget_index )
    {
        int extendable_count;
        Widget* widget = layout[ widget_index ];
        if( widget->style != nullptr )
        {
            resolveSizing
            (
                widget->parent,
                widget->style->width,
                widget->rect.width,
                layout.getWidth(),
                extendable_count
            );
            resolveSizing
            (
                widget->parent,
                widget->style->height,
                widget->rect.height,
                layout.getHeight(),
                extendable_count
            );
        }
        resolveFlexSizes( layout, widget_index, Extent{ widget->rect.width, widget->rect.height });
        resolveExtendSizes( layout, widget_index );
    }

}
