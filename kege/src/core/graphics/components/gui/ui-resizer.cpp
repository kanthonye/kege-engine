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

    void resolveSizing(uint32_t parent, const Sizing& sizing, float& size, float available_space, int& extend_count)
    {
        switch (sizing.type)
        {
            case kege::ui::SizingType::Fixed:
            {
                // Fixed nodes already have quad.width/quad.height assigned
                // before the resizer runs — nothing to resolve here.
                break;
            }

            case kege::ui::SizingType::Percent:
            {
                size += available_space * sizing.size;
                break;
            }

            case kege::ui::SizingType::Extend:
            {
                if (parent == 0) // 0 = null object index; no parent means this node isn't constrained by one
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

    inline bool accumulateLine
    (
        float main_size,
        float cross_size,
        float main_limit,
        const AlignDir& dir,
        float& main_sum,
        float& cross_max,
        float& extent_main,
        float& extent_cross
    )
    {
        bool newline = dir.y != AlignDir::NILL && (main_sum + main_size) > main_limit;

        if (newline)
        {
            // Close out the line that was accumulating: fold its cross-axis
            // extent into the total, then start a new line seeded by this child.
            extent_cross += cross_max;
            cross_max = cross_size;
            main_sum = main_size;
        }
        else
        {
            cross_max = kege::max(cross_max, cross_size);
            main_sum += main_size;
        }

        extent_main = kege::max(extent_main, main_sum);
        return newline;
    }

    void Resizer::resolveExtendSizes( Layout& layout, uint32_t node_index )
    {
        const kege::ui::Node* node = layout[ node_index ];
        const kege::ui::Elem* elem = layout.elem( node );
        const Padding& padding = elem->padding;

        bool horizontal = (elem->alignment.direction.x == ui::AlignDir::LEFT ||
                            elem->alignment.direction.x == ui::AlignDir::RIGHT);

        float h_pad = padding.left + padding.right;
        float v_pad = padding.above + padding.below;
        float main_gap = horizontal ? elem->alignment.gap.width : elem->alignment.gap.height;
        float gapsum = (node->count > 1) ? main_gap * (node->count - 1) : 0.f;

        Extent boundary;
        if (horizontal)
        {
            boundary.width  = kege::max(0.f, node->quad.width  - h_pad - gapsum);
            boundary.height = kege::max(0.f, node->quad.height - v_pad);
        }
        else
        {
            boundary.width  = kege::max(0.f, node->quad.width  - h_pad);
            boundary.height = kege::max(0.f, node->quad.height - v_pad - gapsum);
        }

        Extent extent = {};
        Extent run = {};
        int extend_count = 0;  // count of Extend children along the MAIN axis only
        int discard = 0;       // cross-axis Extend still needs its size reset to 0.f, but its count is unused

        for (uint32_t child_index = node->head; child_index != 0; child_index = layout.next(child_index))
        {
            ui::Node* n = layout[ child_index ];
            kege::ui::Elem* e = layout.elem( n );

            if (e->width.type != SizingType::None)
            {
                resolveSizing( n->parent, e->width, n->quad.width, boundary.width, horizontal ? extend_count : discard );
            }
            if (e->height.type != SizingType::None)
            {
                resolveSizing( n->parent, e->height, n->quad.height, boundary.height, horizontal ? discard : extend_count );
            }

            if (e->position == Positioning::Independent || e->position == Positioning::Absolute)
                continue;

            horizontal
                ? accumulateLine( n->quad.width,  n->quad.height, boundary.width,  elem->alignment.direction, run.width,  run.height, extent.width,  extent.height )
                : accumulateLine( n->quad.height, n->quad.width,  boundary.height, elem->alignment.direction, run.height, run.width,  extent.height, extent.width  );
        }

        // No gate on extend_count here — this loop is a no-op per-child when nothing
        // needs extending, so it's safe (and simpler) to always run it.
        if (horizontal)
        {
            if (extent.height == 0) extent.height = boundary.height;
            else extent.height += run.height;
        }
        else
        {
            if (extent.width == 0) extent.width = boundary.width;
            else extent.width += run.width;
        }

        float main_extend_space = 0.f;
        if (extend_count)
        {
            main_extend_space = horizontal
                ? (boundary.width  - extent.width)  / float(extend_count)
                : (boundary.height - extent.height) / float(extend_count);
        }

        for (uint32_t child_index = node->head; child_index != 0; child_index = layout.next(child_index))
        {
            kege::ui::Node* n = layout[ child_index ];
            kege::ui::Elem* e = layout.elem( n );

            if (horizontal)
            {
                if (e->width.type  == ui::SizingType::Extend) n->quad.width  += main_extend_space;
                if (e->height.type == ui::SizingType::Extend) n->quad.height += extent.height;
            }
            else
            {
                if (e->width.type  == ui::SizingType::Extend) n->quad.width  += extent.width;
                if (e->height.type == ui::SizingType::Extend) n->quad.height += main_extend_space;
            }
        }

        for (uint32_t child_index = node->head; child_index != 0; child_index = layout.next(child_index))
        {
            kege::ui::Node* n = layout[ child_index ];
            if (0 < n->count)
            {
                resolveExtendSizes( layout, child_index );
            }
        }
    }

    void Resizer::resolveFlexSizes( Layout& layout, uint32_t node_index, const Extent& boundary )
    {
        kege::ui::Node* node = layout[ node_index ];
        kege::ui::Elem* elem = layout.elem( node );

        for (uint32_t child_index = node->head; child_index != 0; child_index = layout.next(child_index))
        {
            resolveFlexSizes( layout, child_index, boundary );
        }

        bool width_flexible  = elem->width.type  == ui::SizingType::Flexible;
        bool height_flexible = elem->height.type == ui::SizingType::Flexible;
        if (!width_flexible && !height_flexible)
            return;

        bool horizontal = (elem->alignment.direction.x == ui::AlignDir::LEFT ||
                            elem->alignment.direction.x == ui::AlignDir::RIGHT);

        const Padding& padding = elem->padding;
        float h_pad = padding.left + padding.right;
        float v_pad = padding.above + padding.below;

        Extent flex = {};
        if (node->text.data)
        {
            if (width_flexible)  flex.width  += node->quad.width  + h_pad;
            if (height_flexible) flex.height += node->quad.height + v_pad;
        }

        float main_sum = 0.f;
        float cross_max = 0.f;

        for (uint32_t child_index = node->head; child_index != 0; child_index = layout.next(child_index))
        {
            kege::ui::Node* n = layout[ child_index ];
            const kege::ui::Elem* e = layout.elem( n );

            if (e->width.type == ui::SizingType::Percent || e->height.type == ui::SizingType::Percent)
                continue;

            horizontal
                ? accumulateLine( n->quad.width,  n->quad.height, boundary.width,  elem->alignment.direction, main_sum, cross_max, flex.width,  flex.height )
                : accumulateLine( n->quad.height, n->quad.width,  boundary.height, elem->alignment.direction, main_sum, cross_max, flex.height, flex.width  );
        }

        if (horizontal)
        {
            flex.height += cross_max;

            if (width_flexible)
            {
                float gapsum = (node->count > 1) ? elem->alignment.gap.width * (node->count - 1) : 0.f;
                if (node->text.data != nullptr)
                {
                    node->quad.width += node->text.width + h_pad + gapsum;
                }
                else node->quad.width += flex.width + h_pad + gapsum;
            }
            if (height_flexible)
            {
                if (node->text.data != nullptr)
                {
                    node->quad.height += node->text.font_size + v_pad;
                }
                else node->quad.height += flex.height + v_pad;
            }
        }
        else
        {
            flex.width += cross_max;

            if (width_flexible)
            {
                if (node->text.data != nullptr)
                {
                    node->quad.width += node->text.width + v_pad;
                }
                else node->quad.width += flex.width + h_pad;
            }
            if (height_flexible)
            {
                float gapsum = (node->count > 1) ? elem->alignment.gap.height * (node->count - 1) : 0.f;
                if (node->text.data != nullptr)
                {
                    node->quad.height += node->text.font_size + h_pad + gapsum;
                }
                else node->quad.height += flex.height + v_pad + gapsum;
            }
        }
    }

    void Resizer::resize( Layout& layout, uint32_t node_index )
    {
        kege::ui::Node* node = layout[ node_index ];
        //kege::ui::Elem* elem = layout.elem( node );

        resolveFlexSizes( layout, node_index, Extent{ node->quad.width, node->quad.height });
        resolveExtendSizes( layout, node_index );
    }

}
