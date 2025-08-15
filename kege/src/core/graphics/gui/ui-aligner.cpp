//
//  ui-aligner.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/27/25.
//
//
#include "ui-layout.hpp"
#include "ui-aligner.hpp"

namespace kege::ui{

    float Aligner::getFixedWidth( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data )
    {
        if ( layout._nodes[ child ].elem.style.width.type == ui::SIZE_EXTEND )
        {
            data.extendable_width_count++;
        }
        else
        {
            if ( layout._nodes[ child ].elem.style.width.type == ui::SIZE_PERCENT )
            {
                layout._nodes[ child ].elem.rect.width = (data.bounding.width - data.gap.width) * layout._nodes[ child ].elem.style.width.size;
            }
            return layout._nodes[ child ].elem.rect.width;
        }
        return 0;
    }

    float Aligner::getFixedHeight( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data )
    {
        Style& style = layout._nodes[ child ].elem.style;
        if ( style.height.type == ui::SIZE_EXTEND )
        {
            data.extendable_height_count++;
        }
        else
        {
            if ( style.height.type == ui::SIZE_PERCENT )
            {
                layout._nodes[ child ].elem.rect.height = (data.bounding.height - data.gap.height) * style.height.size;
            }

            return layout._nodes[ child ].elem.rect.height;
        }
        return 0;
    }

    void Aligner::integrateCoverageHorizontally( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data )
    {
        Style& parent_style = layout._nodes[ parent ].elem.style;
        Style& child_style = layout._nodes[ child ].elem.style;

        if ( parent_style.align.wrap_around )
        {
            // if we have a wrap around in the horizontal direction, then the width can not be flexible
            if ( parent_style.width.type != ui::SIZE_FLEXIBLE )
            {
                float space = data.sum.width + layout._nodes[ child ].elem.rect.width;
                space += parent_style.padding.left + parent_style.padding.right;

                // Check if the child overflows the parent's width
                if ( space > layout._nodes[ parent ].elem.rect.width )
                {
                    data.sum.height += data.max.height + parent_style.gap.height;  // Move Y to the next row
                    data.sum.width  = 0; // Reset X to start of the row
                    data.max.height = 0; // Reset max row height
                }
            }
        }

        /*
         * This handles cases where you have a horizontal-alignment elements
         * with multiple elements with vertical expandable height. Since the
         * remaining height gets divided by extendable_height_count,
         * extendable_height_count need to be 1, so that all vertical expandable
         * height have the same length.
         */
        if ( child_style.height.type == ui::SIZE_EXTEND )
        {
            data.extendable_height_count = 1;
        }
        else
        {
            // Track the tallest element in the row
            data.max.height = kege::max< float >( data.max.height, layout._nodes[ child ].elem.rect.height );
        }

        data.sum.width += getFixedWidth( layout, parent, child, data );
        data.region.height = kege::max( data.region.height, data.sum.height );
        data.region.width = kege::max( data.region.width, data.sum.width );
    }

    void Aligner::integrateCoverageVertically( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data )
    {
        Style& parent_style = layout._nodes[ parent ].elem.style;
        Style& child_style = layout._nodes[ child ].elem.style;

        if ( parent_style.align.wrap_around  )
        {
            // if we have a wrap around in the vertical direction, then the height can not be flexible
            if ( parent_style.height.type != ui::SIZE_FLEXIBLE  )
            {
                float space = data.sum.height + layout._nodes[ child ].elem.rect.height;
                space += parent_style.padding.above + parent_style.padding.below;

                // Check if the child overflows the parent's height
                if ( data.sum.height + layout._nodes[ child ].elem.rect.height > data.bounding.height )
                {
                    data.sum.width += data.max.width + parent_style.gap.width; // Move X to the next column
                    data.sum.height = 0; // Reset Y to start of the column
                    data.max.width = 0; // Reset max column width
                }
            }
        }

        /*
         * This handles cases where you have a vertical-alignment elements
         * with multiple elements having horizontal expandable widths. Since the
         * remaining width gets divided by extendable_width_count,
         * extendable_width_count need to be 1, so that all horizontal expandable
         * width have the same length.
         */
        if ( child_style.width.type == ui::SIZE_EXTEND )
        {
            data.extendable_width_count = 1;
        }
        else
        {
            // Track the widest element in the column
            data.max.width = kege::max< float >( data.max.width, layout._nodes[ child ].elem.rect.width );
        }

        data.sum.height += getFixedHeight( layout, parent, child, data );
        data.region.height = kege::max( data.region.height, data.sum.height );
        data.region.width = kege::max( data.region.width, data.sum.width );
    }

    void Aligner::computeCoverageData( Layout& layout, uint32_t pid, CoverageData& data )
    {
        Style& parent_style = layout._nodes[ pid ].elem.style;

        data.padding.width = parent_style.padding.left  + parent_style.padding.right;
        data.padding.height = parent_style.padding.above + parent_style.padding.below;
        data.bounding.width  = kege::max(0.f, layout._nodes[ pid ].elem.rect.width  - data.padding.width);
        data.bounding.height = kege::max(0.f, layout._nodes[ pid ].elem.rect.height - data.padding.height);
        data.gap.width  = (parent_style.gap.width  * (layout._nodes[ pid ].count - 1));
        data.gap.height = (parent_style.gap.height * (layout._nodes[ pid ].count - 1));
        data.extendable_width_count = 0;
        data.extendable_height_count = 0;

        if ( parent_style.align.direction == ui::HORIZONTALLY )
        {
            integrateCoverageHorizontally( layout, pid, layout._nodes[ pid ].head, data );
            for (int i = layout._nodes[ layout._nodes[ pid ].head ].next; 0 != i; i = layout._nodes[ i ].next )
            {
                data.sum.width += parent_style.gap.width;
                integrateCoverageHorizontally( layout, pid, i, data );
            }
        }
        else if ( parent_style.align.direction == ui::VERTICALLY )
        {
            integrateCoverageVertically( layout, pid, layout._nodes[ pid ].head, data );
            for (int i = layout._nodes[ layout._nodes[ pid ].head ].next; 0 != i; i = layout._nodes[ i ].next )
            {
                data.sum.height += parent_style.gap.height;
                integrateCoverageVertically( layout, pid, i, data );
            }
        }
        else
        {
            for (int i = layout._nodes[ pid ].head; 0 != i; i = layout._nodes[ i ].next )
            {
                data.sum.height = kege::max< float >( data.sum.height, layout._nodes[ i ].elem.rect.height );
                data.sum.width = kege::max< float >( data.sum.width, layout._nodes[ i ].elem.rect.width );
            }
        }
    }

    /**
     * @brief
     * Compute the lengths of a flexible ui element. flexible elements adjust their width and height
     * to have a minimum fit to enclose its children.
     *
     * @param pid The ui element we need to compute the dimensions for.
     */
    void Aligner::computeFlexibleLengths( Layout& layout, uint32_t pid )
    {
        Style& style = layout._nodes[ pid ].elem.style;

        /**
         * If this flexible element is not a parent then its dimensions are determind by its padding.
         */
        if ( !layout._nodes[ pid ].parent )
        {
            if ( style.width.type == ui::SIZE_FLEXIBLE )
            {
                layout._nodes[ pid ].elem.rect.width = style.padding.left + style.padding.right;
            }
            if ( style.height.type == ui::SIZE_FLEXIBLE )
            {
                layout._nodes[ pid ].elem.rect.height = style.padding.above + style.padding.below;
            }
            return;
        }

        /**
         * compute the coverage data for all children
         */
        CoverageData data = {};
        computeCoverageData( layout, pid, data );

        data.region.height = kege::max( data.region.height, kege::max( data.max.height, data.sum.height ) );
        data.region.width = kege::max( data.region.width, kege::max( data.max.width, data.sum.width ) );
        if( data.sum.height != 0 ) data.region.height += data.max.height;
        if( data.sum.width != 0 ) data.region.width += data.max.width;
        
        /**
         * determine the dimensions of this element
         */
        if ( style.width.type == ui::SIZE_FLEXIBLE )
        {
            layout._nodes[ pid ].elem.rect.width = data.region.width;
            layout._nodes[ pid ].elem.rect.width += data.padding.width;
        }

        if ( style.height.type == ui::SIZE_FLEXIBLE )
        {
            layout._nodes[ pid ].elem.rect.height = data.region.height;
            layout._nodes[ pid ].elem.rect.height += data.padding.height;
        }
    }

    /**
     * @brief
     * Compute the lengths of a extendable ui element. extendable elements adjust their width and height
     * to fill up the remaining space.
     *
     * @param eid The ui element we need to compute the dimensions for.
     */
    Extent2D Aligner::computeExtent( Layout& layout, uint32_t eid )
    {
        /**
         * compute the coverage data for all children
         */
        CoverageData data = {};
        computeCoverageData( layout, eid, data );

        /**
         * determine the dimensions of this element
         */
        if ( 0 < data.extendable_width_count || 0 < data.extendable_height_count )
        {
            float remaining_width = data.bounding.width - data.sum.width;
            float extender_width = ( 0 < data.extendable_width_count )? remaining_width / data.extendable_width_count: 0;

            float remaing_height = data.bounding.height - data.sum.height;
            float extender_height = ( 0 < data.extendable_height_count )? (remaing_height / data.extendable_height_count): 0;

            /*
             * set the extendable width of this element
             */
            for (int i = layout._nodes[ eid ].head; 0 != i; i = layout._nodes[ i ].next )
            {
                if ( layout._nodes[ i ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ i ].elem.rect.x = layout._nodes[ i ].elem.x;
                    layout._nodes[ i ].elem.rect.y = layout._nodes[ i ].elem.y;
                    continue;
                }

                Style& style = layout._nodes[ i ].elem.style;

                /*
                 * set the extendable width of this element
                 */
                if ( style.width.type == ui::SIZE_EXTEND )
                {
                    if ( style.width.size < 1.0 )
                    {
                        layout._nodes[ i ].elem.rect.width = remaining_width * style.width.size;
                    }
                    else
                    {
                        layout._nodes[ i ].elem.rect.width = extender_width;
                    }

                    if ( data.extendable_width_count > 0 )
                    {
                        data.region.width += layout._nodes[ i ].elem.rect.width;
                        data.extendable_width_count--;
                    }
                }

                /*
                 * set the extendable height of this element
                 */
                if ( style.height.type == ui::SIZE_EXTEND )
                {
                    if ( style.height.size < 1.0 )
                    {
                        layout._nodes[ i ].elem.rect.height = remaing_height * style.height.size;
                    }
                    else
                    {
                        layout._nodes[ i ].elem.rect.height = extender_height;
                    }

                    if ( data.extendable_height_count > 0 )
                    {
                        data.region.height += layout._nodes[ i ].elem.rect.height;
                        data.extendable_height_count--;
                    }
                }
            }
        }

        if ( data.sum.width < data.max.width ) data.sum.width = data.max.width;
        if ( data.sum.height < data.max.height ) data.sum.height = data.max.height;
        data.region.height = kege::max( data.region.height, data.sum.height );
        data.region.width = kege::max( data.region.width, data.sum.width );
        return data.region;
    }

    kege::vec2 Aligner::getAlignmentStartingPosition( Layout& layout, uint32_t pid, float width, float height )
    {
        float x = layout._nodes[ pid ].elem.rect.x;
        float y = layout._nodes[ pid ].elem.rect.y;
        Style& style = layout._nodes[ pid ].elem.style;

        switch ( style.align.origin )
        {
            default:
            case ui::ALIGN_TOP_LEFT:
            {
                x += style.padding.left;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_TOP_CENTER:
            {
                x += (layout._nodes[ pid ].elem.rect.width - width) * 0.5;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_TOP_RIGHT:
            {
                x += layout._nodes[ pid ].elem.rect.width - width - style.padding.right;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_LEFT_CENTER:
            {
                x += style.padding.left;
                y += (layout._nodes[ pid ].elem.rect.height - height) * 0.5;
                break;
            }
            case ui::ALIGN_CENTER_CENTER:
            {
                x += (layout._nodes[ pid ].elem.rect.width  - width ) * 0.5;
                y += (layout._nodes[ pid ].elem.rect.height - height) * 0.5;
                break;
            }
            case ui::ALIGN_RIGHT_CENTER:
            {
                x += layout._nodes[ pid ].elem.rect.width - width - style.padding.right;
                y += (layout._nodes[ pid ].elem.rect.height - height) / 2;
                break;
            }
            case ui::ALIGN_BOTTOM_LEFT:
            {
                x += style.padding.left;
                y += layout._nodes[ pid ].elem.rect.height - height - style.padding.below;
                break;
            }
            case ui::ALIGN_BOTTOM_CENTER:
            {
                x += (layout._nodes[ pid ].elem.rect.width - width) / 2;
                y += layout._nodes[ pid ].elem.rect.height - height - style.padding.below;
                break;
            }
            case ui::ALIGN_BOTTOM_RIGHT:
            {
                x += layout._nodes[ pid ].elem.rect.width  - width - style.padding.right;
                y += layout._nodes[ pid ].elem.rect.height - height - style.padding.below;
                break;
            }
        }
        return {x, y};
    }

    void Aligner::alignChildren( Layout& layout, uint32_t pid, Extent2D& extent )
    {
        if ( !layout._nodes[ pid ].parent )
        {
            return;
        }

        // Calculate starting position based on alignment direction and starting position
        kege::vec2 start, length;
        float max_w = 0, max_h = 0;
        int cid = layout._nodes[ pid ].head;

        start = getAlignmentStartingPosition( layout, pid, extent.width, extent.height );

        /*
         * Step 5: Position the Elements:
         *
         * Iterate through the list again, positioning each element based on their calculated widths and heights.
         */

        Style& style = layout._nodes[ pid ].elem.style;

        /*
         * Position the Horizontal Elements:
         */
        if ( style.align.direction == ui::HORIZONTALLY )
        {
            layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + start.x;
            layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + start.y;

            length.y = 0;
            length.x = layout._nodes[ cid ].elem.rect.width;
            max_h = kege::max< float >( max_h, layout._nodes[ cid ].elem.rect.height );

            for ( cid = layout._nodes[ layout._nodes[ pid ].head ].next; cid > 0; cid = layout._nodes[ cid ].next )
            {
                if ( layout._nodes[ cid ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y;
                    continue;
                }

                Style& child_style = layout._nodes[ cid ].elem.style;

                if ( child_style.align.immovable )
                {
                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + start.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + start.y;
                }
                else
                {
                    length.x += style.gap.width;

                    if ( style.align.wrap_around )
                    {
                        float space = length.x + layout._nodes[ cid ].elem.rect.width;
                        space += style.padding.left + style.padding.right;
                        // Check if the child overflows the parent's width
                        if ( space > layout._nodes[ pid ].elem.rect.width )
                        {
                            length.x  = 0; // Reset X to start of the row
                            length.y += max_h + style.gap.height;  // Move Y to the next row
                            max_h = 0;          // Reset max row height
                        }
                    }

                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + length.x + start.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + length.y + start.y;
                    length.x += layout._nodes[ cid ].elem.rect.width;

                    // Track the tallest element in the row
                    max_h = kege::max< float >( max_h, layout._nodes[ cid ].elem.rect.height );
                }
            }
        }

        /*
         * Position the Vertical Elements:
         */
        else if ( style.align.direction == ui::VERTICALLY )
        {
            layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + start.x;
            layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + start.y;

            length.x = 0;
            length.y = layout._nodes[ cid ].elem.rect.height;
            max_w = kege::max< float >( max_w, layout._nodes[ cid ].elem.rect.height );

            for ( cid = layout._nodes[ layout._nodes[ pid ].head ].next; cid > 0; cid = layout._nodes[ cid ].next )
            {
                if ( layout._nodes[ cid ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y;
                    continue;
                }

                Style& child_style = layout._nodes[ cid ].elem.style;

                if ( child_style.align.immovable )
                {
                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + start.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + start.y;
                    continue;
                }
                else
                {
                    length.y += style.gap.height;
                    
                    if ( style.align.wrap_around )
                    {
                        float space = length.y + layout._nodes[ cid ].elem.rect.height;
                        space += style.padding.above + style.padding.below;
                        // Check if the child overflows the parent's height
                        if ( space > layout._nodes[ pid ].elem.rect.height )
                        {
                            length.y = 0; // Reset Y to start of the column
                            length.x += max_w + style.gap.width;  // Move X to the next column
                            max_w = 0;          // Reset max column width
                        }
                    }

                    layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + length.x + start.x;
                    layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + length.y + start.y;
                    length.y += layout._nodes[ cid ].elem.rect.height;

                    // Track the widest element in the column
                    max_w = kege::max<float>( max_w, layout._nodes[ cid ].elem.rect.width );
                }
            }
        }

        else
        {
            layout._nodes[ cid ].elem.rect.x = layout._nodes[ cid ].elem.x + start.x;
            layout._nodes[ cid ].elem.rect.y = layout._nodes[ cid ].elem.y + start.y;
        }
    }

    bool Aligner::shouldPreCalcFlexibleSize( Layout& layout, uint32_t eid )const
    {
        Style& style = layout._nodes[ eid ].elem.style;

        if ( style.width.type == ui::SIZE_FLEXIBLE )
        {
            if ( style.align.direction == ui::HORIZONTALLY )
            {
                if ( style.align.wrap_around == true )
                {
                    return false;
                }
            }
            return true;
        }
        if ( style.height.type == ui::SIZE_FLEXIBLE )
        {
            if ( style.align.direction == ui::VERTICALLY )
            {
                if ( style.align.wrap_around == true )
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    void Aligner::alignElements( Layout& layout, uint32_t pid )
    {
        Extent2D child_boundary = computeExtent( layout, pid );
        alignChildren( layout, pid, child_boundary );

        for (int cid = layout._nodes[ pid ].head; 0 != cid; cid = layout._nodes[ cid ].next )
        {
            if ( cid > 0 )
            {
                alignElements( layout, cid );
            }
        }
    }


    vec2 Aligner::getAlignmentStartingPosition( Layout& layout, uint32_t cid, const Extent2D& extent, uint32_t pid )
    {
        float x = layout._nodes[ cid ].elem.rect.x;
        float y = layout._nodes[ cid ].elem.rect.y;
        Style& style = layout._nodes[ cid ].elem.style;

        switch ( style.align.origin )
        {
            default:
            case ui::ALIGN_TOP_LEFT:
            {
                x += style.padding.left;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_TOP_CENTER:
            {
                x += (layout._nodes[ cid ].elem.rect.width - extent.width) * 0.5;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_TOP_RIGHT:
            {
                x += layout._nodes[ cid ].elem.rect.width - extent.width - style.padding.right;
                y += style.padding.above;
                break;
            }
            case ui::ALIGN_LEFT_CENTER:
            {
                x += style.padding.left;
                y += (layout._nodes[ cid ].elem.rect.height - extent.height) * 0.5;
                break;
            }
            case ui::ALIGN_CENTER_CENTER:
            {
                x += (layout._nodes[ cid ].elem.rect.width  - extent.width ) * 0.5;
                y += (layout._nodes[ cid ].elem.rect.height - extent.height) * 0.5;
                break;
            }
            case ui::ALIGN_RIGHT_CENTER:
            {
                x += layout._nodes[ cid ].elem.rect.width - extent.width - style.padding.right;
                y += (layout._nodes[ cid ].elem.rect.height - extent.height) / 2;
                break;
            }
            case ui::ALIGN_BOTTOM_LEFT:
            {
                x += style.padding.left;
                y += layout._nodes[ cid ].elem.rect.height - extent.height - style.padding.below;
                break;
            }
            case ui::ALIGN_BOTTOM_CENTER:
            {
                x += (layout._nodes[ cid ].elem.rect.width - extent.width) / 2;
                y += layout._nodes[ cid ].elem.rect.height - extent.height - style.padding.below;
                break;
            }
            case ui::ALIGN_BOTTOM_RIGHT:
            {
                x += layout._nodes[ cid ].elem.rect.width  - extent.width - style.padding.right;
                y += layout._nodes[ cid ].elem.rect.height - extent.height - style.padding.below;
                break;
            }
        }
        return {x, y};
    }

    Extent2D Aligner::getColumnExtent( Layout& layout, uint32_t curr_index, uint32_t pid )
    {
        Extent2D bounding, padding, gap;
        Style& style = layout._nodes[ pid ].elem.style;

        padding.height = style.padding.above + style.padding.below;
        padding.width = style.padding.left  + style.padding.right;
        bounding.height = kege::max(0.f, layout._nodes[ pid ].elem.rect.height - padding.height);
        bounding.width = kege::max(0.f, layout._nodes[ pid ].elem.rect.width  - padding.width);
        gap.height = style.gap.height * (layout._nodes[ pid ].count - 1);
        gap.width = style.gap.width * (layout._nodes[ pid ].count - 1);

        int count = layout._nodes[ pid ].count;
        Extent2D extent = {};
        int item;
        float item_height = 0;
        bool extendable_width = false;
        int extendable_height_count = 0;
        for (item = curr_index; item != 0; item = layout._nodes[ item ].next )
        {
            if ( layout._nodes[ item ].elem.style.align.self == AlignSelf::Absolute )
            {
                layout._nodes[ item ].elem.rect.x = layout._nodes[ item ].elem.x;
                layout._nodes[ item ].elem.rect.y = layout._nodes[ item ].elem.y;
                continue;
            }

            Style& child_style = layout._nodes[ item ].elem.style;
            if ( child_style.height.type == ui::SIZE_EXTEND )
            {
                extendable_height_count += 1;
            }
            else
            {
                if ( child_style.height.type == ui::SIZE_PERCENT )
                {
                    item_height = (bounding.height - gap.height) * child_style.height.size;
                }
                else
                {
                    item_height = layout._nodes[ item ].elem.rect.height;
                }

                if ( extent.height + item_height > bounding.height && style.align.wrap_around )
                {
                    break;
                }
                extent.height += item_height;
            }

            if ( child_style.width.type == ui::SIZE_EXTEND )
            {
                extendable_width = true;
            }
            else
            {
                extent.width = kege::max<uint32_t>( extent.width, layout._nodes[ item ].elem.rect.width );
            }

            extent.height += (count-- > 1) ? style.gap.width : 0;
        }

        if ( 0 < extendable_height_count || extendable_width )
        {
            float remaining_height = bounding.height - extent.height;
            float extender_height = ( 0 < extendable_height_count )? remaining_height / extendable_height_count: 0;

            /*
             * set the extendable height of this element
             */
            for (item = curr_index; item != 0; item = layout._nodes[ item ].next )
            {
                if ( layout._nodes[ item ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ item ].elem.rect.x = layout._nodes[ item ].elem.x;
                    layout._nodes[ item ].elem.rect.y = layout._nodes[ item ].elem.y;
                    continue;
                }

                Style& child_style = layout._nodes[ item ].elem.style;

                /*
                 * set the extendable height of this element
                 */
                if ( child_style.height.type == ui::SIZE_EXTEND )
                {
                    if ( child_style.height.size < 1.0 )
                    {
                        layout._nodes[ item ].elem.rect.height = remaining_height * child_style.height.size;
                    }
                    else
                    {
                        layout._nodes[ item ].elem.rect.height = extender_height;
                    }
                    extent.height += layout._nodes[ item ].elem.rect.height;
                }

                /*
                 * set the extendable width of this element
                 */
                if ( child_style.width.type == ui::SIZE_EXTEND )
                {
                    layout._nodes[ item ].elem.rect.width = bounding.width;
                }

                extent.width = kege::max<uint32_t>( extent.width, layout._nodes[ item ].elem.rect.width );
            }
        }
        return extent;
    }
    
    Extent2D Aligner::getRowExtent( Layout& layout, uint32_t curr_index, uint32_t pid )
    {
        Extent2D bounding, padding, gap;
        Style& style = layout._nodes[ pid ].elem.style;

        padding.height = style.padding.above + style.padding.below;
        padding.width = style.padding.left  + style.padding.right;
        bounding.height = kege::max(0.f, layout._nodes[ pid ].elem.rect.height - padding.height);
        bounding.width = kege::max(0.f, layout._nodes[ pid ].elem.rect.width  - padding.width);
        gap.height = style.gap.height * (layout._nodes[ pid ].count - 1);
        gap.width = style.gap.width * (layout._nodes[ pid ].count - 1);

        int count = layout._nodes[ pid ].count;
        Extent2D extent = {};
        float item_width = 0;
        int item = curr_index;
        bool extendable_height = false;
        int extendable_width_count = 0;
        for (; item != 0; item = layout._nodes[ item ].next )
        {
            if ( layout._nodes[ item ].elem.style.align.self == AlignSelf::Absolute )
            {
                layout._nodes[ item ].elem.rect.x = layout._nodes[ item ].elem.x;
                layout._nodes[ item ].elem.rect.y = layout._nodes[ item ].elem.y;
                continue;
            }

            Style& child_style = layout._nodes[ item ].elem.style;

            if ( child_style.width.type == ui::SIZE_EXTEND )
            {
                extendable_width_count += 1;
            }
            else
            {
                if ( child_style.width.type == ui::SIZE_PERCENT )
                {
                    item_width = (bounding.width - gap.width) * child_style.width.size;
                }
                else
                {
                    item_width = layout._nodes[ item ].elem.rect.width;
                }

                if ( extent.width + item_width > bounding.width && style.align.wrap_around )
                {
                    break;
                }
                extent.width += item_width;
            }

            if ( child_style.height.type == ui::SIZE_EXTEND )
            {
                extendable_height = true;
            }
            else
            {
                extent.height = kege::max<uint32_t>( extent.height, layout._nodes[ item ].elem.rect.height );
            }

            extent.width += (count-- > 1) ? style.gap.width : 0;
        }

        if ( 0 < extendable_width_count || extendable_height )
        {
            float remaining_width = bounding.width - extent.width;
            float extender_width = ( 0 < extendable_width_count )? remaining_width / extendable_width_count: 0;

            /*
             * set the extendable width of this element
             */
            for ( item = curr_index; item != 0; item = layout._nodes[ item ].next )
            {
                if ( layout._nodes[ item ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ item ].elem.rect.x = layout._nodes[ item ].elem.x;
                    layout._nodes[ item ].elem.rect.y = layout._nodes[ item ].elem.y;
                    continue;
                }

                Style& child_style = layout._nodes[ item ].elem.style;

                /*
                 * set the extendable width of this element
                 */
                if ( child_style.width.type == ui::SIZE_EXTEND )
                {
                    if ( child_style.width.size < 1.0 )
                    {
                        layout._nodes[ item ].elem.rect.width = remaining_width * child_style.width.size;
                    }
                    else
                    {
                        layout._nodes[ item ].elem.rect.width = extender_width;
                    }
                    extent.width += layout._nodes[ item ].elem.rect.width;
                }

                /*
                 * set the extendable height of this element
                 */
                if ( child_style.height.type == ui::SIZE_EXTEND )
                {
                    layout._nodes[ item ].elem.rect.height = bounding.height;
                }

                extent.height = kege::max<uint32_t>( extent.height, layout._nodes[ item ].elem.rect.height );
                count++;
            }
        }

        return extent;
    }

    void Aligner::alignHorizontally( Layout& layout, uint32_t pid )
    {
        float row_height = 0;
        vec2 length = {0.f, 0.f};
        uint32_t curr_index = layout._nodes[ pid ].head;
        int count = layout._nodes[ pid ].count;

        Style& style = layout._nodes[ pid ].elem.style;

        while ( curr_index != 0 )
        {
            Extent2D extent = getRowExtent( layout, curr_index, pid );
            if ( extent.width == 0 ) {
                break;
            }
            vec2 start = getAlignmentStartingPosition( layout, pid, extent, pid );

            for (; length.x < extent.width && curr_index != 0; curr_index = layout._nodes[ curr_index ].next )
            {
                if ( layout._nodes[ curr_index ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ curr_index ].elem.rect.x = layout._nodes[ curr_index ].elem.x;
                    layout._nodes[ curr_index ].elem.rect.y = layout._nodes[ curr_index ].elem.y;
                    continue;
                }

                layout._nodes[ curr_index ].elem.rect.x = layout._nodes[ curr_index ].elem.x + length.x + start.x;
                layout._nodes[ curr_index ].elem.rect.y = layout._nodes[ curr_index ].elem.y + length.y + start.y;

                length.x += (count-- > 1.f)
                ? layout._nodes[ curr_index ].elem.rect.width + style.gap.width
                : layout._nodes[ curr_index ].elem.rect.width;

                row_height = kege::max( row_height, layout._nodes[ curr_index ].elem.rect.height );
            }
            length.x = 0.f;
            length.y += row_height + style.gap.height;
        }
    }

    void Aligner::alignVertically( Layout& layout, uint32_t pid )
    {
        float row_width = 0;
        vec2 length = {0.f, 0.f};
        uint32_t curr_index = layout._nodes[ pid ].head;
        int count = layout._nodes[ pid ].count;

        while ( curr_index != 0 )
        {
            Extent2D extent = getColumnExtent( layout, curr_index, pid );
            if ( extent.height == 0 ) {
                break;
            }
            vec2 start = getAlignmentStartingPosition( layout, pid, extent, pid );
            for (; length.y < extent.height && curr_index != 0; curr_index = layout._nodes[ curr_index ].next )
            {
                if ( layout._nodes[ curr_index ].elem.style.align.self == AlignSelf::Absolute )
                {
                    layout._nodes[ curr_index ].elem.rect.x = layout._nodes[ curr_index ].elem.x;
                    layout._nodes[ curr_index ].elem.rect.y = layout._nodes[ curr_index ].elem.y;
                    continue;
                }

                float x = (extent.width - layout._nodes[ curr_index ].elem.rect.width) * 0.5;
                layout._nodes[ curr_index ].elem.rect.x = layout._nodes[ curr_index ].elem.x + length.x + start.x + x;
                layout._nodes[ curr_index ].elem.rect.y = layout._nodes[ curr_index ].elem.y + length.y + start.y;

                length.y += (count-- > 1)
                ? layout._nodes[ curr_index ].elem.rect.height + layout._nodes[ pid ].elem.style.gap.height
                : layout._nodes[ curr_index ].elem.rect.height;

                row_width = kege::max( row_width, layout._nodes[ curr_index ].elem.rect.width );
            }
            length.x += row_width;
            length.y = 0.f;
        }
    }

    void Aligner::align( Layout& layout, uint32_t pid )
    {
        //Node& elem = layout._nodes[ pid ];
        switch ( layout._nodes[ pid ].elem.style.align.direction )
        {
            case Direction::HORIZONTALLY:
            {
                alignHorizontally( layout, pid );
                break;
            }

            case Direction::VERTICALLY:
            {
                alignVertically( layout, pid );
                break;
            }

            default:
            {
                for (uint32_t eid = layout._nodes[ pid].head; eid != 0 ; eid = layout._nodes[ eid ].next )
                {
                    layout._nodes[ eid ].elem.rect.x = layout._nodes[ pid ].elem.x;
                    layout._nodes[ eid ].elem.rect.y = layout._nodes[ pid ].elem.y;
                }
                break;
            }
        }

        for (uint32_t eid = layout._nodes[ pid].head; eid != 0 ; eid = layout._nodes[ eid ].next )
        {
            align( layout, eid );
        }
    }

    Aligner::Aligner()
    {}
}
