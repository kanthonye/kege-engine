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

    /*
    inline float getTop( const Rect& rect, const Padding& padding )
    {
        return rect.y + padding.above;
    }
    inline float getBottom( const Rect& rect, const Padding& padding )
    {
        return rect.y + rect.height - padding.below;
    }

    inline float getLeft( const Rect& rect, const Padding& padding )
    {
        return rect.x + padding.left;
    }
    inline float getCenterX( const Rect& rect, const Padding& padding )
    {
        return rect.x - (padding.left + padding.right) + rect.width * 0.5f;
    }
    inline float getCenterY( const Rect& rect, const Padding& padding )
    {
        return rect.y - (padding.above + padding.below) + rect.height * 0.5f;
    }
    inline float getRight( const Rect& rect, const Padding& padding )
    {
        return rect.x - padding.right + rect.width;
    }


    inline vec2 getTopLeftOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getLeft( rect, padding ), getTop( rect, padding ) };
    }
    inline vec2 getTopCenterOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getCenterX(rect, padding), getTop( rect, padding ) };
    }
    inline vec2 getTopRightOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getRight( rect, padding ), getTop( rect, padding ) };
    }

    inline vec2 getCenterLeftOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getLeft( rect, padding ), getCenterY( rect, padding ) };
    }
    inline vec2 getCenterCenterOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getCenterX(rect, padding), getCenterY( rect, padding ) };
    }
    inline vec2 getCenterRightOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getRight( rect, padding ), getCenterY( rect, padding ) };
    }

    inline vec2 getBottonLeftOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getLeft( rect, padding ), getBottom( rect, padding ) };
    }
    inline vec2 getBottomCenterOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getCenterX(rect, padding), getBottom( rect, padding ) };
    }
    inline vec2 getBottomRightOffset( const Rect& rect, const Padding& padding )
    {
        return vec2{ getRight( rect, padding ), getBottom( rect, padding ) };
    }


    inline void advanceNorthToSouth( vec2& curr_pos, Rect& rect )
    {
        curr_pos.y += rect.height;
    }
    inline void advanceSouthToNorth( vec2& curr_pos, Rect& rect )
    {
        curr_pos.y -= rect.height;
    }
    inline void advanceWestToEast( vec2& curr_pos, Rect& rect )
    {
        curr_pos.x += rect.width;
    }
    inline void advanceEastToWest( vec2& curr_pos, Rect& rect )
    {
        curr_pos.x -= rect.width;
    }
    */


//    inline vec2 getTopCenterPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getCenterX(rect, padding), getTop( rect, padding ) };
//    }
//    inline vec2 getTopRightPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getRight( rect, padding ), getTop( rect, padding ) };
//    }
//
//    inline vec2 getCenterLeftPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getLeft( rect, padding ), getCenterY( rect, padding ) };
//    }
//    inline vec2 getCenterCenterPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getCenterX(rect, padding), getCenterY( rect, padding ) };
//    }
//    inline vec2 getCenterRightPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getRight( rect, padding ), getCenterY( rect, padding ) };
//    }
//
//    inline vec2 getBottonLeftPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getLeft( rect, padding ), getBottom( rect, padding ) };
//    }
//    inline vec2 getBottomCenterPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getCenterX(rect, padding), getBottom( rect, padding ) };
//    }
//    inline vec2 getBottomRightPos( const Rect& rect, const Padding& padding )
//    {
//        return vec2{ getRight( rect, padding ), getBottom( rect, padding ) };
//    }

    vec2 getAlignmentOrigin( const Widget& widget )
    {
        vec2 p;
        switch ( widget.alignment.origin.x )
        {
            case AlignX::LEFT:   p.x = widget.rect.x + widget.padding.left; break;
            case AlignX::CENTER: p.x = widget.rect.x + widget.rect.width * 0.5f; break;
            case AlignX::RIGHT:  p.x = widget.rect.x + widget.rect.width - widget.padding.right; break;
        }

        switch ( widget.alignment.origin.y )
        {
            case AlignY::TOP:    p.y = widget.rect.y + widget.padding.above; break;
            case AlignY::CENTER: p.y = widget.rect.y + widget.rect.height * 0.5f; break;
            case AlignY::BOTTOM: p.y = widget.rect.y + widget.rect.height - widget.padding.below; break;
        }
        return p;
    }

    void alignTopY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            layout[begin]->rect.y += origin.y;
        }
    }

    void alignBottomY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            w->rect.y += origin.y + extent - w->rect.height;
        }
    }

    void alignCenterY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            w->rect.y += origin.y + (extent - w->rect.height) * 0.5;
        }
    }

    void alignLeftX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            w->rect.x += origin.x;
        }
    }

    void alignRightX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            w->rect.x += origin.x + extent - w->rect.width;
        }
    }

    void alignCenterX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        for (; begin != end ; begin = layout.next( begin ) )
        {
            Widget* w = layout[begin];
            if ( w->position == Positioning::Independent )
            {
                continue;
            }
            else if ( w->position == Positioning::Absolute )
            {
                //w->rect.y += origin.y;// + e->offset.y;
                continue;
            }
            w->rect.x += origin.x + (extent - w->rect.width) * 0.5;
        }
    }

    void alignX(const Alignment& alignment, int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        vec2 offset = origin;
        if ( alignment.wrap.enable )
        {
            switch ( alignment.wrap.direction )
            {
                case AlignDir::LEFT: offset.x -= extent; break;
                default:break;
            }
        }

        switch ( alignment.items )
        {
            case ui::AlignItem::START:  alignLeftX(begin, end, layout, offset, extent); break;
            case ui::AlignItem::END:    alignRightX(begin, end, layout, offset, extent); break;
            case ui::AlignItem::CENTER: alignCenterX(begin, end, layout, offset, extent); break;
        }
    }
    
    void alignY(const Alignment& alignment, int begin, int end, Layout& layout, const vec2& origin, const float& extent)
    {
        vec2 offset = origin;
        if ( alignment.wrap.enable )
        {
            switch ( alignment.wrap.direction )
            {
                //case AlignDir::DOWN: offset.y += extent.height; break;
                case AlignDir::UP: offset.y -= extent; break;
                default:break;
            }
        }

        switch ( alignment.items )
        {
            case ui::AlignItem::START:  alignTopY(begin, end, layout, offset, extent); break;
            case ui::AlignItem::END:    alignBottomY(begin, end, layout, offset, extent); break;
            case ui::AlignItem::CENTER: alignCenterY(begin, end, layout, offset, extent); break;
            default:break;
        }
    }

    void advanceDownward( Layout& layout, const Widget& parent )
    {
        vec2 origin = getAlignmentOrigin(parent);
        vec2 position = origin;

        float MAX_LENGTH = parent.rect.height - (parent.padding.above + parent.padding.below);
        float height = 0;
        float width = 0;
        float h;

        uint32_t end, begin = parent.head;
        for (end = begin; end != 0 ; end = layout.next( end ) )
        {
            Widget* child = layout[end];

            if ( child->position == Positioning::Independent )
            {
                continue;
            }
            else if ( child->position == Positioning::Absolute )
            {
                child->rect.x += position.x;
                child->rect.y += position.y;
                continue;
            }

            h = height + child->rect.height;

            if (MAX_LENGTH <= h && parent.alignment.wrap.enable)
            {
                if (width == 0) width = child->rect.width;
                alignX(parent.alignment, begin, end, layout, position, width);
                begin = end;

                switch ( parent.alignment.wrap.direction )
                {
                    case AlignDir::LEFT:  position.x -= width + parent.gap.height; break;
                    case AlignDir::RIGHT: position.x += width + parent.gap.height; break;
                    default:break;
                }
                
                height = 0;
                h = height + child->rect.height;
            }
            child->rect.y += position.y + height;
            height = h + parent.gap.height;

            if (width < child->rect.width) width = child->rect.width;
        }
        alignX(parent.alignment, begin, end, layout, position, width);
    }

    void advanceUpward( Layout& layout, const Widget& parent )
    {
        vec2 origin = getAlignmentOrigin(parent);
        vec2 position = origin;

        uint32_t end, begin = parent.head;
        float height = 0;
        float width = 0;

        for (end = begin; end != 0 ; end = layout.next( end ) )
        {
            Widget* child = layout[end];

            if ( child->position == Positioning::Independent )
            {
                continue;
            }
            else if ( child->position == Positioning::Absolute )
            {
                child->rect.x += position.x;
                child->rect.y += position.y;
                continue;
            }

            if (width < child->rect.width) width = child->rect.width;
            height += child->rect.height;

            if (parent.rect.height <= height && parent.alignment.wrap.enable)
            {
                alignX(parent.alignment, begin, end, layout, position, width);
                begin = end;

                switch ( parent.alignment.wrap.direction )
                {
                    case AlignDir::LEFT:  position.x -= width + parent.gap.width; break;
                    case AlignDir::RIGHT: position.x += width + parent.gap.width; break;
                    default:break;
                }
                height = child->rect.height + parent.padding.below;
            }
            child->rect.y += position.y - height;
            height += parent.gap.height;
        }
        alignX(parent.alignment, begin, end, layout, position, width);
    }

    void advanceToTheRight( Layout& layout, const Widget& parent )
    {
        float MAX_LENGTH = parent.rect.width - (parent.padding.left + parent.padding.right);
        vec2 origin = getAlignmentOrigin(parent);
        vec2 position = origin;

        uint32_t end, begin = parent.head;
        float height = 0;
        float width = 0;
        float w;

        for (end = begin; end != 0 ; end = layout.next( end ) )
        {
            Widget* child = layout[end];

            if ( child->position == Positioning::Independent )
            {
                continue;
            }
            else if ( child->position == Positioning::Absolute )
            {
                child->rect.x += position.x;
                child->rect.y += position.y;
                continue;
            }

            w = width + child->rect.width;

            if (MAX_LENGTH < w)
            {
                if (height == 0) height = child->rect.height;
                alignY(parent.alignment, begin, end, layout, position, height);
                begin = end;

                if (parent.alignment.wrap.enable)
                {
                    switch ( parent.alignment.wrap.direction )
                    {
                        case AlignDir::UP:   position.y -= height + parent.gap.height; break;
                        case AlignDir::DOWN: position.y += height + parent.gap.height; break;
                        default:break;
                    }
                }

                width = 0;
                w = width + child->rect.width;
            }
            
            if (height < child->rect.height) height = child->rect.height;
            child->rect.x += position.x + width;
            width = w + parent.gap.width;
        }
        alignY(parent.alignment, begin, end, layout, position, height);
    }

    void advanceToTheLeft( Layout& layout, const Widget& parent )
    {
        float MAX_LENGTH = parent.rect.width - (parent.padding.left + parent.padding.right);
        vec2 origin = getAlignmentOrigin(parent);
        vec2 position = origin;

        uint32_t end, begin = parent.head;
        float height = 0;
        float width = 0;

        for (end = begin; end != 0 ; end = layout.next( end ) )
        {
            Widget* child = layout[end];

            if ( child->position == Positioning::Independent )
            {
                continue;
            }
            else if ( child->position == Positioning::Absolute )
            {
                child->rect.x += position.x;// + e->offset.x;
                child->rect.y += position.y;// + e->offset.y;
                continue;
            }

            width += child->rect.width;

            if (MAX_LENGTH <= width && parent.alignment.wrap.enable)
            {
                if (height == 0) height = child->rect.height;
                alignY(parent.alignment, begin, end, layout, position, height);
                begin = end;

                switch ( parent.alignment.wrap.direction )
                {
                    case AlignDir::UP:   position.y -= height + parent.gap.height; break;
                    case AlignDir::DOWN: position.y += height + parent.gap.height; break;
                    default:break;
                }
                width = child->rect.width;
            }
            
            if (height < child->rect.height) height = child->rect.height;
            child->rect.x += position.x - width;
            width += parent.gap.width;
        }
        alignY(parent.alignment, begin, end, layout, position, height);
    }


    void centerAll( Layout& layout, const Widget& parent )
    {
        vec2 origin = getAlignmentOrigin(parent);
        uint32_t end, begin = parent.head;

        for (end = begin; end != 0 ; end = layout.next( end ) )
        {
            Widget* child = layout[end];

            if ( child->position == Positioning::Independent )
            {
                continue;
            }
            else if ( child->position == Positioning::Absolute )
            {
                child->rect.x += origin.x;// + e->offset.x;
                child->rect.y += origin.y;// + e->offset.y;
                continue;
            }

            switch ( parent.alignment.direction )
            {
                case AlignDir::CENTER:
                    child->rect.x = origin.x - child->rect.width * 0.5;
                    child->rect.y = origin.y - child->rect.height * 0.5;
                    break;

                case AlignDir::CENTER_X:
                    child->rect.x = origin.x - child->rect.width * 0.5;
                    break;

                case AlignDir::CENTER_Y:
                    child->rect.y = origin.y - child->rect.height * 0.5;
                    break;

                default:break;
            }
        }
    }


//    vec2 calcPos( const Alignment& alignment, const Padding& padding, const Rect& rect, const ui::Extent& extent )
//    {
//        vec2 p;
//        switch ( alignment.content.x )
//        {
//            case AlignX::LEFT:   p.x = rect.x + padding.left; break;
//            case AlignX::CENTER: p.x = rect.x + (rect.width - extent.width) * 0.5f; break;
//            case AlignX::RIGHT:  p.x = rect.x + rect.width - extent.width - padding.right; break;
//        }
//
//        switch ( alignment.content.y )
//        {
//            case AlignY::TOP:    p.y = rect.y + padding.above; break;
//            case AlignY::CENTER: p.y = rect.y + (rect.height - extent.height) * 0.5f; break;
//            case AlignY::BOTTOM: p.y = rect.y + rect.height - extent.height - padding.below; break;
//        }
//        return p;
//    }
//
//    vec2 calcPos( const Alignment& alignment, const vec2& pos, const ui::Extent& extent )
//    {
//        vec2 p;
//        switch ( alignment.origin.x )
//        {
//            case AlignX::LEFT:   p.x = pos.x /*+ s->padding.left*/; break;
//            case AlignX::CENTER: p.x = pos.x + extent.width * 0.5; break;
//            case AlignX::RIGHT:  p.x = pos.x + extent.width /*+ s->padding.right*/; break;
//        }
//
//        switch ( alignment.origin.y )
//        {
//            case AlignY::TOP:    p.y = pos.y /*+ s->padding.above*/; break;
//            case AlignY::CENTER: p.y = pos.y + extent.height * 0.5f; break;
//            case AlignY::BOTTOM: p.y = pos.y + extent.height /*+ s->padding.below*/; break;
//        }
//        return p;
//    }
//
//    struct Boundary
//    {
//        std::vector< ui::Extent > rects;
//        Extent max;
//    };
//
//    Boundary computeInnerBoundaries( Layout& layout, uint32_t pid, const Alignment& alignment, const Gap& gap, const Rect& outer_boundary )
//    {
//        Boundary boundary;
//        boundary.max = {};
//        boundary.rects.reserve(16);
//
//        Widget* p = layout[ pid ];
//        Rect rect = {};
//
//        rect.height = 0.f;
//        rect.width = 0.f;
//
//        float x = 0.f;
//        float curr = 0.f;
//
//        float max_width = p->rect.width;// - p->style->padding.left - p->style->padding.right;
//        const float max_height = p->rect.height;// - p->style->padding.above - p->style->padding.below;
//
//        Extent extent = {};
//        if( alignment.direction == ui::AlignDir::RIGHT )
//        {
//            for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
//            {
//                Widget* e = layout[ eid ];
//                Positioning positioning = (e->style)? e->style->position: Positioning::Relative;
//                extent.height = kege::max(extent.height, e->rect.height);
//
//                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
//                {
//                    continue;
//                }
//
//                extent.width = curr;
//                curr += e->rect.width + gap.width;
//                if (curr > max_width && alignment.wrap_around)
//                {
//                    x += extent.height;
//                    boundary.rects.push_back(extent);
//                    curr = e->rect.width + gap.width;
//                }
//                boundary.max.height = kege::max(boundary.max.height, x);
//                boundary.max.width = kege::max(boundary.max.width, curr);
//            }
//            boundary.max.height += extent.height;
//            boundary.rects.push_back(extent);
//        }
//        else
//        {
//            for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
//            {
//                Widget* e = layout[ eid ];
//                Positioning positioning = (e->style)? e->style->position: Positioning::Relative;
//                extent.width = kege::max(extent.width, e->rect.width);
//
//                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
//                {
//                    continue;
//                }
//
//                extent.height = curr;
//                curr += e->rect.height + gap.height;
//                if (curr > max_height && alignment.wrap_around)
//                {
//                    x += extent.width;
//                    boundary.rects.push_back(extent);
//                    curr = e->rect.height + gap.height;
//                }
//                boundary.max.height = kege::max(boundary.max.height, curr);
//                boundary.max.width = kege::max(boundary.max.width, x);
//            }
//            boundary.max.width += extent.width;
//            boundary.rects.push_back(extent);
//        }
//
//        return boundary;
//    }
//
//    void alignVertically( Layout& layout, uint32_t pid, const Alignment& alignment )
//    {
//        const Widget* p = layout[ pid ];
//
//        Boundary boundaries = computeInnerBoundaries( layout, pid, alignment, p->gap, p->rect );
//
//        vec2 min = calcPos( alignment, p->padding, p->rect, boundaries.max);
//        vec2 start = calcPos( alignment, min, boundaries.max );
//        vec2 pos = start;
//
//        int count = 0;
//        float threshold = 0;
//        if (alignment.flow.x == AlignDirX::ETW)
//        {
//            pos.x -= boundaries.rects[0].width;
//            threshold = start.y - boundaries.max.height;
//        }
//        else
//        {
//            threshold = min.y + boundaries.max.height;
//        }
//
//        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
//        {
//            Widget* e = layout[ eid ];
//            const ui::Extent* rect = &boundaries.rects[ count ];
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                e->rect.x += pos.x;// + e->offset.x;
//                e->rect.y += pos.y;// + e->offset.y;
//            }
//            else
//            {
//                if (alignment.flow.y == AlignDirY::NTS)
//                {
//                    if (pos.y >= threshold && alignment.wrap_around)
//                    {
//                        if (alignment.flow.x == AlignDirX::ETW)
//                        {
//                            pos.x -= rect->width;
//                        }
//                        else
//                        {
//                            pos.x += rect->width;
//                        }
//                        pos.y = start.y;
//
//                        if (count + 1 < boundaries.rects.size())
//                        {
//                            count += 1;
//                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
//                        }
//                    }
//                    e->rect.x += pos.x;// + e->offset.x;
//                    e->rect.y += pos.y;// + e->offset.y;
//                    pos.y += e->rect.height + p->gap.height;
//                }
//                else //p->style->align_y == AlignDirY::SouthToNorth
//                {
//                    pos.y -= e->rect.height;
//                    if (pos.y < threshold && alignment.wrap_around)
//                    {
//                        if (alignment.flow.x == AlignDirX::ETW)
//                        {
//                            pos.x -= rect->width + p->gap.width;
//                        }
//                        else
//                        {
//                            pos.x += rect->width + p->gap.width;
//                        }
//                        pos.y = start.y - e->rect.height - p->gap.height;
//
//                        if (count + 1 < boundaries.rects.size())
//                        {
//                            count += 1;
//                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
//                        }
//                    }
//                    e->rect.x += pos.x;// + e->offset.x;
//                    e->rect.y += pos.y;// + e->offset.y;
//                    pos.y -= p->gap.height;
//                }
//            }
//        }
//    }
//
//    void alignHorizontally( Layout& layout, uint32_t pid, const Alignment& alignment )
//    {
//        const Widget* p = layout[ pid ];
//
//        Boundary boundaries = computeInnerBoundaries( layout, pid, alignment, p->gap, p->rect );
//
//        vec2 min = calcPos( alignment, p->padding, p->rect, boundaries.max);
//        vec2 start = calcPos( alignment, min, boundaries.max );
//        vec2 pos = start;
//
//        int count = 0;
//        float threshold = 0;
//        if (alignment.flow.x == AlignDirX::ETW)
//        {
//            pos.x = start.x;
//            threshold = start.x - boundaries.max.width;
//        }
//        else
//        {
//            threshold = min.x + boundaries.max.width;
//        }
//
//        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
//        {
//            Widget* e = layout[ eid ];
//            const ui::Extent* rect = &boundaries.rects[ count ];
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                e->rect.x += pos.x;// + e->offset.x;
//                e->rect.y += pos.y;// + e->offset.y;
//            }
//            else
//            {
//                if (alignment.flow.x == AlignDirX::WTE)
//                {
//                    if (pos.x >= threshold && alignment.wrap_around)
//                    {
//                        if (alignment.flow.y == AlignDirY::STN)
//                        {
//                            pos.y -= rect->height + p->gap.height;
//                        }
//                        else
//                        {
//                            pos.y += rect->height + p->gap.height;
//                        }
//                        pos.x = start.x;
//
//                        if (count + 1 < boundaries.rects.size())
//                        {
//                            count += 1;
//                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
//                        }
//                    }
//                    e->rect.x += pos.x;// + e->offset.x;
//                    e->rect.y += pos.y;// + e->offset.y;
//                    pos.x += e->rect.width + p->gap.width;
//                }
//                else //p->style->align_y == AlignDirY::SouthToNorth
//                {
//                    pos.x -= e->rect.width;
//                    if (pos.x < threshold && alignment.wrap_around)
//                    {
//                        if (alignment.flow.y == AlignDirY::STN)
//                        {
//                            pos.y -= rect->height;
//                        }
//                        else
//                        {
//                            pos.y += rect->height;
//                        }
//                        pos.x = start.x - e->rect.width;
//
//                        if (count + 1 < boundaries.rects.size())
//                        {
//                            count += 1;
//                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
//                        }
//                    }
//                    e->rect.x += pos.x;// + e->offset.x;
//                    e->rect.y += pos.y;// + e->offset.y;
//                    pos.x -= p->gap.width;
//                }
//            }
//        }
//    }

    void Aligner::align( Layout& layout, uint32_t pid )
    {
        Extent2D extent = {};
        Widget* p = layout[ pid ];

        switch ( p->alignment.direction )
        {
            case AlignDir::UP: advanceUpward(layout, *p); break;
            case AlignDir::DOWN: advanceDownward(layout, *p); break;
            case AlignDir::LEFT: advanceToTheLeft(layout, *p); break;
            case AlignDir::RIGHT: advanceToTheRight(layout, *p); break;
            case AlignDir::CENTER_X:
            case AlignDir::CENTER_Y:
            case AlignDir::CENTER: centerAll(layout, *p); break;
        }

        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            if (layout[eid]->count != 0)
            {
                align( layout, eid );
            }
        }
    }

}
