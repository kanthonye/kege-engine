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

    float getWidth( Layout& layout, NodeIndex pid )
    {
        float width = 0;
        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            width += getWidth( layout, eid );
        }

        const Style* style = layout[ pid ]->style;
        switch ( style->width.type )
        {
            case ui::SizingType::SIZE_FIXED:
            {
                layout[ pid ]->rect.width = style->width.size;
                break;
            }

            case ui::SizingType::SIZE_PERCENT:
            {
                layout[ pid ]->rect.width = layout[ layout.parent( pid ) ]->rect.width * style->width.size;
                width += layout[ pid ]->rect.width;
                break;
            }

            case ui::SizingType::SIZE_EXTEND:
            {
                break;
            }

            case ui::SizingType::SIZE_FLEXIBLE:
            {
                layout[ pid ]->rect.width = width;
                break;
            }

            default: break;
        }

        return layout[ pid ]->rect.width;
    }



    void alignTopToBottom( Layout& layout, NodeIndex pid )
    {
        int count = 0;
        vec2 offset;

        offset.x = layout[ pid ]->rect.x + layout[ pid ]->style->padding.left;
        offset.y = layout[ pid ]->rect.y + layout[ pid ]->style->padding.above;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            if ( layout[ eid ]->style->position == Positioning::Absolute )
            {
                layout[ eid ]->rect.x = layout[ eid ]->x;
                layout[ eid ]->rect.y = layout[ eid ]->y;
                count++;
                continue;
            }

            layout[ eid ]->rect.x = layout[ eid ]->x + offset.x;
            layout[ eid ]->rect.y = layout[ eid ]->y + offset.y;

            offset.y += layout[ eid ]->rect.height;
            if( count < layout.count( pid ))
            {
                offset.y += layout[ pid ]->style->gap.height;
            }
            count++;
        }
    }

    void alignBottomToTop( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        int count = 0;

        vec2 offset;
        offset.x = layout[ pid ]->rect.x + style->padding.left;
        offset.y = layout[ pid ]->rect.y + style->padding.above;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            if ( s->position == Positioning::Absolute )
            {
                layout[ eid ]->rect.x = layout[ eid ]->x;
                layout[ eid ]->rect.y = layout[ eid ]->y;
                count++;
                continue;
            }

            offset.y -= layout[ eid ]->rect.height;

            layout[ eid ]->rect.x = layout[ eid ]->x + offset.x;
            layout[ eid ]->rect.y = layout[ eid ]->y + offset.y;

            if( count < layout.count( pid ) )
            {
                offset.y -= style->gap.height;
            }
            count++;
        }
    }

    void alignLeftToRight( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        int count = 0;

        vec2 offset;
        offset.x = layout[ pid ]->rect.x + style->padding.left;
        offset.y = layout[ pid ]->rect.y + style->padding.above;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            if ( s->position == Positioning::Absolute )
            {
                layout[ eid ]->rect.x = layout[ eid ]->x;
                layout[ eid ]->rect.y = layout[ eid ]->y;
                count++;
                continue;
            }

            layout[ eid ]->rect.x = layout[ eid ]->x + offset.x;
            layout[ eid ]->rect.y = layout[ eid ]->y + offset.y;

            offset.x += layout[ eid ]->rect.width;
            if( count < layout.count( pid ) )
            {
                offset.x += style->gap.width;
            }
            count++;
        }
    }

    void alignRightToLeft( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        int count = 0;

        vec2 offset;
        offset.x = layout[ pid ]->rect.x + layout[ pid ]->rect.width + style->padding.left;
        offset.y = layout[ pid ]->rect.y + layout[ pid ]->rect.height + style->padding.above;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            if ( s->position == Positioning::Absolute )
            {
                layout[ eid ]->rect.x = layout[ eid ]->x;
                layout[ eid ]->rect.y = layout[ eid ]->y;
                count++;
                continue;
            }

            offset.x -= layout[ eid ]->rect.width;

            layout[ eid ]->rect.x = layout[ eid ]->x + offset.x;
            layout[ eid ]->rect.y = layout[ eid ]->y + offset.y;

            if( count < layout.count( pid ) - 1 )
            {
                offset.x -= style->gap.width;
            }
            count++;
        }
    }

    float getPercentWidth( Layout& layout, NodeIndex eid )
    {
        const Style* style =layout[ eid ]->style;
        Widget* elem = layout[ eid ];
        elem->rect.width = 0;
        if ( 1 <= layout.parent( eid ) )
        {
            uint32_t parent = layout.parent( eid );
            elem->rect.width += style->padding.left;
            elem->rect.width += style->padding.right;
            elem->rect.width += style->gap.width * (layout.count( parent ) - 1);
            elem->rect.width += layout[ parent ]->rect.width;
        }
        elem->rect.width = elem->rect.width * style->width.size;
        return elem->rect.width;
    }

    float getPercentHeight( Layout& layout, NodeIndex eid )
    {
        const Style* style = layout[ eid ]->style;
        Widget* elem = layout[ eid ];
        elem->rect.height = 0;

        if ( 1 <= layout.parent( eid ) )
        {
            uint32_t parent = layout.parent( eid );
            elem->rect.height += style->padding.above;
            elem->rect.height += style->padding.below;
            elem->rect.height += style->gap.height * (layout.count( parent ) - 1);
            elem->rect.height += layout[ parent ]->rect.height;
        }
        elem->rect.height = elem->rect.height * style->height.size;
        return elem->rect.height;
    }

    float getFlexibleWidth( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        Widget* elem = layout[ pid ];

        elem->rect.width = (style->width.type == ui::SizingType::SIZE_FIXED)
        ? style->width.size
        : 0;

        bool percentage = false;
        uint32_t count = 0;

        if ( 1 <= layout.parent( pid ) )
        {
            const Style* s = layout[ layout.parent( pid ) ]->style;
            elem->rect.width += s->padding.left;
            elem->rect.width += s->padding.right;
        }

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            switch ( s->width.type )
            {
                case ui::SizingType::SIZE_FIXED:
                {
                    elem->rect.width += layout[ eid ]->rect.width;
                    count += 1;
                    break;
                }

                case ui::SizingType::SIZE_FLEXIBLE:
                {
                    elem->rect.width += getFlexibleWidth( layout, eid );
                    count += 1;
                    break;
                }

                case ui::SizingType::SIZE_PERCENT:
                {
                    percentage = true;
                    break;
                }

                default: break;
            }
        }

        if ( percentage )
        {
            float width = 0;
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->width.type == ui::SizingType::SIZE_PERCENT )
                {
                    width += elem->rect.width * s->width.size;
                    count += 1;
                }
            }
            elem->rect.width = width;
        }

        if ( 2 <= count )
        {
            elem->rect.width += style->gap.width * (count - 1);
        }

        return elem->rect.width;
    }

    float getFlexibleHeight( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;

        float height = (style->height.type == ui::SizingType::SIZE_FIXED)
        ? style->height.size
        : 0;

        uint32_t count = 0;
        bool percentage = false;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            switch ( s->height.type )
            {
                case ui::SizingType::SIZE_FIXED:
                {
                    if ( style->align.direction == ui::DIRECTION_LEFT_TO_RIGHT )
                    {
                        height = max( height, layout[ eid ]->rect.height );
                    }
                    else
                    {
                        height += layout[ eid ]->rect.height;
                    }
                    count += 1;
                    break;
                }

                case ui::SizingType::SIZE_FLEXIBLE:
                {
                    float h = getFlexibleHeight( layout, eid );
                    if ( style->align.direction == ui::DIRECTION_LEFT_TO_RIGHT )
                    {
                        height = max( height, h );
                    }
                    else
                    {
                        height += h;
                    }
                    count += 1;
                    break;
                }

                case ui::SizingType::SIZE_PERCENT:
                {
                    percentage = true;
                    break;
                }

                default: break;
            }
        }

        if ( percentage )
        {
            /**
             * after the fixed heights are summed, compute the children ui percentage widths of the summed width
             */
            float h = 0.f;
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->height.type == ui::SizingType::SIZE_PERCENT )
                {
                    h += layout[ pid ]->rect.height * s->height.size;
                    count += 1;
                }
            }
            height = h;
        }

        /**
         * gap between elements are only accountable if the elements are being aligned in a vertical manner.
         */
        if ( 2 <= count )
        {
            if
            (
                style->align.direction == ui::DIRECTION_TOP_TO_BOTTOM ||
                style->align.direction == ui::DIRECTION_BOTTOM_TO_TOP
            )
            {
                height += style->gap.height * (count - 1);
            }
        }

        height += style->padding.above;
        height += style->padding.below;
        layout[ pid ]->rect.height = height;
        return height;
    }

    float updateChildrenWidth( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        float pad = 0;
        float width = 0;
        uint32_t extend_count = 0;
        bool percentage = false;

        pad += style->padding.left;
        pad += style->padding.right;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            switch ( s->width.type )
            {
                case ui::SizingType::SIZE_FIXED:
                {
                    width += layout[ eid ]->rect.width;
                    break;
                }

                case ui::SizingType::SIZE_FLEXIBLE:
                {
                    width += getFlexibleWidth( layout, eid );
                    break;
                }

                case ui::SizingType::SIZE_PERCENT:
                {
                    percentage = true;
                    break;
                }

                case ui::SizingType::SIZE_EXTEND:
                {
                    extend_count += 1;
                    break;
                }

                default: break;
            }
        }

        if ( percentage )
        {
            float w = 0;
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->width.type == ui::SizingType::SIZE_PERCENT )
                {
                    w += width * s->width.size;
                }
            }
            width += w;
        }

        if ( 0 < extend_count  )
        {
            float w = layout[ pid ]->rect.width - pad;

            if
            (
                style->align.direction == DIRECTION_LEFT_TO_RIGHT ||
                style->align.direction == DIRECTION_RIGHT_TO_LEFT
            )
            {
                w = w - width - style->gap.width * (layout.count( pid ) - 1);
                w = w / float(extend_count);
            }

            for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->width.type == ui::SizingType::SIZE_EXTEND )
                {
                    layout[ eid ]->rect.width = w;
                    width += w;
                }
            }
        }

        return width;
    }

    float updateChildrenHeight( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        float pad = 0;
        float height = 0;
        uint32_t extend_count = 0;
        bool percentage = false;

        pad += style->padding.above;
        pad += style->padding.below;

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            const Style* s = layout[ eid ]->style;
            switch ( s->height.type )
            {
                case ui::SizingType::SIZE_FIXED:
                {
                    height += layout[ eid ]->rect.height;
                    break;
                }

                case ui::SizingType::SIZE_FLEXIBLE:
                {
                    height += getFlexibleHeight( layout, eid );
                    break;
                }

                case ui::SizingType::SIZE_PERCENT:
                {
                    percentage = true;
                    break;
                }

                case ui::SizingType::SIZE_EXTEND:
                {
                    extend_count += 1;
                    break;
                }

                default: break;
            }
        }

        if ( percentage )
        {
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->height.type == ui::SizingType::SIZE_PERCENT )
                {
                    layout[ eid ]->rect.height = layout[ pid ]->rect.height * s->height.size;
                    height += layout[ eid ]->rect.height;
                }
            }
        }

        if ( 0 < extend_count  )
        {
            float h = layout[ pid ]->rect.height - pad;

            if
            (
                style->align.direction == DIRECTION_TOP_TO_BOTTOM ||
                style->align.direction == DIRECTION_BOTTOM_TO_TOP
            )
            {
                h = h - height - style->gap.height * (layout.count( pid ) - 1);
                h = h / float(extend_count);
            }

            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                const Style* s = layout[ eid ]->style;
                if ( s->height.type == ui::SizingType::SIZE_EXTEND )
                {
                    layout[ eid ]->rect.height = h;
                    height += h;
                }
            }
        }

        return height;
    }

    void computeChildrenExtent( Layout& layout, NodeIndex pid )
    {
        updateChildrenWidth( layout, pid );
        updateChildrenHeight( layout, pid );

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            computeChildrenExtent( layout, eid );
        }
    }
    
    void alignChildrenNodes( Layout& layout, NodeIndex pid )
    {
        const Style* style = layout[ pid ]->style;
        switch ( style->align.direction )
        {
            case Direction::DIRECTION_LEFT_TO_RIGHT:
            {
                alignLeftToRight( layout, pid );
                break;
            }

            case Direction::DIRECTION_RIGHT_TO_LEFT:
            {
                alignRightToLeft( layout, pid );
                break;
            }

            case Direction::DIRECTION_TOP_TO_BOTTOM:
            {
                alignTopToBottom( layout, pid );
                break;
            }

            case Direction::DIRECTION_BOTTOM_TO_TOP:
            {
                alignBottomToTop( layout, pid );
                break;
            }

            default:
            {
                break;
            }
        }
        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            alignChildrenNodes( layout, eid );
        }
    }

    void Aligner::align( Layout& layout, NodeIndex pid )
    {
        computeChildrenExtent( layout, pid );
        alignChildrenNodes( layout, pid );
    }

    Aligner::Aligner()
    {}
}
