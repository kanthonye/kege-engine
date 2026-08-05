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

    vec2 getAlignmentOrigin( const Node* node, const ui::Elem* elem )
    {
        vec2 p;
        switch ( elem->alignment.origin.x )
        {
            case AlignX::LEFT:   p.x = node->quad.x + elem->padding.left; break;
            case AlignX::CENTER: p.x = node->quad.x + node->quad.width * 0.5f; break;
            case AlignX::RIGHT:  p.x = node->quad.x + node->quad.width - elem->padding.right; break;
        }

        switch ( elem->alignment.origin.y )
        {
            case AlignY::TOP:    p.y = node->quad.y + elem->padding.above; break;
            case AlignY::CENTER: p.y = node->quad.y + node->quad.height * 0.5f; break;
            case AlignY::BOTTOM: p.y = node->quad.y + node->quad.height - elem->padding.below; break;
        }
        return p;
    }

//    void alignTopY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.y += origin.y;
//        }
//    }
//
//    void alignBottomY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            //Widget* w = layout[begin];
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.y += origin.y + extent - n->quad.height;
//        }
//    }
//
//    void alignCenterY(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            //Widget* w = layout[begin];
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.y += origin.y + (extent - n->quad.height) * 0.5;
//        }
//    }
//
//    void alignLeftX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            //Widget* w = layout[begin];
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.x += origin.x;
//        }
//    }
//
//    void alignRightX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            //Widget* w = layout[begin];
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.x += origin.x + extent - n->quad.width;
//        }
//    }
//
//    void alignCenterX(int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        for (; begin != end ; begin = layout.next( begin ) )
//        {
//            //Widget* w = layout[begin];
//            kege::ui::Node* n = layout[begin];
//            kege::ui::Elem* e = layout.elem( n );
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                //w->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }
//            n->quad.x += origin.x + (extent - n->quad.width) * 0.5;
//        }
//    }
//
//    void alignX(const Alignment& alignment, int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        vec2 offset = origin;
//        switch ( alignment.direction.x )
//        {
//            case AlignDir::LEFT: offset.x -= extent; break;
//            default:break;
//        }
//
//        switch ( alignment.items )
//        {
//            case ui::AlignItem::START:  alignLeftX(begin, end, layout, offset, extent); break;
//            case ui::AlignItem::END:    alignRightX(begin, end, layout, offset, extent); break;
//            case ui::AlignItem::CENTER: alignCenterX(begin, end, layout, offset, extent); break;
//        }
//    }
//    
//    void alignY(const Alignment& alignment, int begin, int end, Layout& layout, const vec2& origin, const float& extent)
//    {
//        vec2 offset = origin;
//
//        switch ( alignment.direction.y )
//        {
//            case AlignDir::DOWN: offset.y += extent; break;
//            case AlignDir::UP: offset.y -= extent; break;
//            default:break;
//        }
//
//        switch ( alignment.items )
//        {
//            case ui::AlignItem::START:  alignTopY(begin, end, layout, offset, extent); break;
//            case ui::AlignItem::END:    alignBottomY(begin, end, layout, offset, extent); break;
//            case ui::AlignItem::CENTER: alignCenterY(begin, end, layout, offset, extent); break;
//            default:break;
//        }
//    }
//
//    void Aligner::advanceDownward( Layout& layout, const Node* parent )
//    {
//        const kege::ui::Elem* elem = layout.elem( parent );
//        vec2 origin = getAlignmentOrigin( parent, elem );
//        vec2 position = origin;
//
//        float MAX_LENGTH = parent->quad.height - (elem->padding.above + elem->padding.below);
//        float height = 0;
//        float width = 0;
//        float h;
//
//        uint32_t end, begin = parent->head;
//        for (end = begin; end != 0 ; end = layout.next( end ) )
//        {
//            //Widget* child = layout[end];
//            kege::ui::Node* n = layout[end];
//            kege::ui::Elem* e = layout.elem( n );
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                n->quad.x += position.x;
//                n->quad.y += position.y;
//                continue;
//            }
//
//            h = height + n->quad.height;
//
//            if (MAX_LENGTH <= h && elem->alignment.wrap.enable)
//            {
//                if (width == 0) width = n->quad.width;
//                alignX(elem->alignment, begin, end, layout, position, width);
//                begin = end;
//
//                switch ( elem->alignment.wrap.direction )
//                {
//                    case AlignDir::LEFT:  position.x -= width + elem->alignment.gap.height; break;
//                    case AlignDir::RIGHT: position.x += width + elem->alignment.gap.height; break;
//                    default:break;
//                }
//                
//                height = 0;
//                h = height + n->quad.height;
//            }
//            n->quad.y += position.y + height;
//            height = h + elem->alignment.gap.height;
//
//            if (width < n->quad.width) width = n->quad.width;
//        }
//        alignX( elem->alignment, begin, end, layout, position, width );
//    }
//
//    void Aligner::advanceUpward( Layout& layout, const Node* node )
//    {
//        const kege::ui::Elem* elem = layout.elem( node );
//        vec2 origin = getAlignmentOrigin( node, elem );
//        vec2 position = origin;
//
//        uint32_t end, begin = node->head;
//        float height = 0;
//        float width = 0;

//        for (end = begin; end != 0 ; end = layout.next( end ) )
//        {
//            kege::ui::Node* n = layout[end];
//            kege::ui::Elem* e = layout.elem( n );
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                n->quad.x += position.x;
//                n->quad.y += position.y;
//                continue;
//            }
//
//            if (width < n->quad.width) width = n->quad.width;
//            height += n->quad.height;
//
//            if (node->quad.height <= height && elem->alignment.wrap.enable)
//            {
//                alignX(elem->alignment, begin, end, layout, position, width);
//                begin = end;
//
//                switch ( elem->alignment.wrap.direction )
//                {
//                    case AlignDir::LEFT:  position.x -= width + elem->alignment.gap.width; break;
//                    case AlignDir::RIGHT: position.x += width + elem->alignment.gap.width; break;
//                    default:break;
//                }
//                height = n->quad.height + elem->padding.below;
//            }
//            n->quad.y += position.y - height;
//            height += elem->alignment.gap.height;
//        }
//        alignX(elem->alignment, begin, end, layout, position, width);
//    }
//
//    void Aligner::advanceToTheRight( Layout& layout, const Node* node )
//    {
//        const kege::ui::Elem* elem = layout.elem( node );
//        vec2 origin = getAlignmentOrigin( node, elem );
//
//        float MAX_LENGTH = node->quad.width - (elem->padding.left + elem->padding.right);
//        vec2 position = origin;
//
//        uint32_t end, begin = node->head;
//        float height = 0;
//        float width = 0;
//        float w;

//        for (end = begin; end != 0 ; end = layout.next( end ) )
//        {
//            //Widget* child = layout[end];
//            kege::ui::Node* n = layout[end];
//            kege::ui::Elem* e = layout.elem( layout[end] );
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                n->quad.x += position.x;
//                n->quad.y += position.y;
//                continue;
//            }
//
//            w = width + n->quad.width;
//
//            if (MAX_LENGTH < w)
//            {
//                if (height == 0) height = n->quad.height;
//                alignY(elem->alignment, begin, end, layout, position, height);
//                begin = end;
//
//                if (elem->alignment.wrap.enable)
//                {
//                    switch ( elem->alignment.wrap.direction )
//                    {
//                        case AlignDir::UP:   position.y -= height + elem->alignment.gap.height; break;
//                        case AlignDir::DOWN: position.y += height + elem->alignment.gap.height; break;
//                        default:break;
//                    }
//                }
//
//                width = 0;
//                w = width + n->quad.width;
//            }
//            
//            if (height < n->quad.height) height = n->quad.height;
//            n->quad.x += position.x + width;
//            width = w + elem->alignment.gap.width;
//        }
//        alignY(elem->alignment, begin, end, layout, position, height);
//    }
//
//    void Aligner::advanceToTheLeft( Layout& layout, const Node* node )
//    {
//        const kege::ui::Elem* elem = layout.elem( node );
//        vec2 origin = getAlignmentOrigin( node, elem );
//
//        float MAX_LENGTH = node->quad.width - (elem->padding.left + elem->padding.right);
//        vec2 position = origin;
//
//        uint32_t end, begin = node->head;
//        float height = 0;
//        float width = 0;

//        for (end = begin; end != 0 ; end = layout.next( end ) )
//        {
//            //Widget* child = layout[end];
//            kege::ui::Node* n = layout[end];
//            kege::ui::Elem* e = layout.elem( n );
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                n->quad.x += position.x;// + e->offset.x;
//                n->quad.y += position.y;// + e->offset.y;
//                continue;
//            }
//
//            width += n->quad.width;
//
//            if (MAX_LENGTH <= width && elem->alignment.wrap.enable)
//            {
//                if (height == 0) height = n->quad.height;
//                alignY(elem->alignment, begin, end, layout, position, height);
//                begin = end;
//
//                switch ( elem->alignment.wrap.direction )
//                {
//                    case AlignDir::UP:   position.y -= height + elem->alignment.gap.height; break;
//                    case AlignDir::DOWN: position.y += height + elem->alignment.gap.height; break;
//                    default:break;
//                }
//                width = n->quad.width;
//            }
//            
//            if (height < n->quad.height) height = n->quad.height;
//            n->quad.x += position.x - width;
//            width += elem->alignment.gap.width;
//        }
//        alignY( elem->alignment, begin, end, layout, position, height );
//    }

//    void Aligner::centerAll( Layout& layout, const Node* node )
//    {
//        const kege::ui::Elem* elem = layout.elem( node );
//        vec2 origin = getAlignmentOrigin( node, elem );
//
//        uint32_t end, begin = node->head;
//
//        for (end = begin; end != 0 ; end = layout.next( end ) )
//        {
//            //Widget* child = layout[end];
//            kege::ui::Node* n = layout[end];
//            kege::ui::Elem* e = layout.elem( n );
//
//            if ( e->position == Positioning::Independent )
//            {
//                continue;
//            }
//            else if ( e->position == Positioning::Absolute )
//            {
//                n->quad.x += origin.x;// + e->offset.x;
//                n->quad.y += origin.y;// + e->offset.y;
//                continue;
//            }

//            switch ( elem->alignment.direction )
//            {
//                case AlignDir::CENTER:
//                    n->quad.x = origin.x - n->quad.width * 0.5;
//                    n->quad.y = origin.y - n->quad.height * 0.5;
//                    break;
//
//                case AlignDir::CENTER_X:
//                    n->quad.x = origin.x - n->quad.width * 0.5;
//                    break;
//
//                case AlignDir::CENTER_Y:
//                    n->quad.y = origin.y - n->quad.height * 0.5;
//                    break;
//
//                default:break;
//            }
//        }
//    }

    void handleOverflow
    (
        kege::ui::Alignment alignment,
        kege::vec2& origin,
        kege::vec2& p,
        float max_l
    )
    {
        switch ( alignment.direction.x )
        {
            default:
            case AlignDir::LEFT:
            case AlignDir::RIGHT:
                p.x = origin.x;
                switch ( alignment.direction.y )
                {
                    default:
                    case AlignDir::DOWN: p.y += max_l + alignment.gap.height; break;
                    case AlignDir::UP:   p.y -= max_l + alignment.gap.height; break;
                }
                break;

            case AlignDir::UP:
            case AlignDir::DOWN:
                p.y = origin.y;
                switch ( alignment.direction.y )
                {
                    default:
                    case AlignDir::RIGHT: p.x += max_l + alignment.gap.width; break;
                    case AlignDir::LEFT:  p.x -= max_l + alignment.gap.width; break;
                }
                break;
        }
    }


    bool advanceRight
    (
        kege::ui::Alignment alignment,
        const kege::vec2& origin,
        Extent& extent,
        kege::vec2& p,
        float& max_l,
        float& stretch,
        Node* n
    )
    {
        float w = stretch + n->quad.width;
        if (extent.width < w) return false;
        max_l = kege::max(max_l, n->quad.height);
        n->quad.x += p.x + stretch;
        n->quad.y += p.y;
        stretch = w + alignment.gap.width;
        return true;
    }

    bool advanceLeft
    (
        kege::ui::Alignment alignment,
        const kege::vec2& origin,
        Extent& extent,
        kege::vec2& p,
        float& max_l,
        float& stretch,
        Node* n
    )
    {
        float w = stretch + n->quad.width;
        if (extent.width - w < 0) return false;
        max_l = kege::max(max_l, n->quad.height);
        n->quad.x += p.x - w;
        n->quad.y += p.y;
        stretch = w + alignment.gap.width;
        return true;
    }

    bool advanceDown
    (
        kege::ui::Alignment alignment,
        const kege::vec2& origin,
        Extent& extent,
        kege::vec2& p,
        float& max_l,
        float& stretch,
        Node* n
    )
    {
        float h = stretch + n->quad.height;
        if (extent.height < h) return false;

        n->quad.x += p.x - ((alignment.origin.x == AlignX::RIGHT)? n->quad.width : 0);
        n->quad.y += p.y + stretch;

        stretch = h + alignment.gap.height;
        max_l = kege::max(max_l, n->quad.width);
        return true;
    }

    bool advanceUp
    (
        kege::ui::Alignment alignment,
        const kege::vec2& origin,
        Extent& extent,
        kege::vec2& p,
        float& max_l,
        float& stretch,
        Node* n
    )
    {
        float h = stretch + n->quad.height;
        if (extent.height - h < 0) return false;

        n->quad.x += p.x - ((alignment.origin.x == AlignX::RIGHT)? n->quad.width : 0);
        n->quad.y += p.y - h;

        stretch = h + alignment.gap.height;
        max_l = kege::max(max_l, n->quad.width);
        return true;
    }

    void offsetContent
    (
        kege::ui::Layout& layout,
        const kege::ui::Node* node,
        kege::ui::Extent& extent,
        kege::ui::Extent& min,
        kege::ui::Extent& max
    )
    {
        const kege::ui::Elem* elem = layout.elem( node );

        kege::vec2 p;
        p.x = node->quad.x + elem->padding.left;
        p.y = node->quad.y + elem->padding.above;

        Extent dim;
        dim.width = (max.width - min.width);
        dim.height = (max.height - min.height);

        kege::vec2 offset = {0,0};
        switch ( elem->alignment.content.x )
        {
            case AlignX::LEFT:
                if (elem->alignment.content.x != elem->alignment.origin.x)
                {
                    offset.x = elem->padding.left - min.width;
                }
                break;

            case AlignX::CENTER: offset.x = (extent.width - dim.width) * 0.5;
                break;

            case AlignX::RIGHT:
                if (elem->alignment.content.x != elem->alignment.origin.x)
                {
                    offset.x = (node->quad.width - dim.width) - elem->padding.left - elem->padding.right;
                }
                break;

            default: break;
        }

        switch ( elem->alignment.content.y )
        {
            case AlignY::TOP:
                if (elem->alignment.content.y != elem->alignment.origin.y)
                {
                    offset.y = -min.height;
                }
                break;

            case AlignY::CENTER: offset.y = (extent.height - dim.height) * 0.5;
                break;

            case AlignY::BOTTOM:
                if (elem->alignment.content.y != elem->alignment.origin.y)
                {
                    offset.y = (node->quad.height - dim.height) - elem->padding.above - elem->padding.below;
                }
                break;

            default: break;
        }

        for (uint32_t index = node->head; index != 0 ; index = layout.next( index ) )
        {
            kege::ui::Node* n = layout[index];
            kege::ui::Elem* e = layout.elem( n );
            if ( e->position == Positioning::Independent || e->position == Positioning::Absolute )
            {
                continue;
            }
            n->quad.x += offset.x;
            n->quad.y += offset.y;
        }
    }

    void Aligner::alignerize
    (
        Layout& layout,
        const Node* node,
        kege::ui::Extent& extent,
        kege::ui::Extent& min,
        kege::ui::Extent& max
    )
    {
        const kege::ui::Elem* elem = layout.elem( node );
        kege::vec2 origin = getAlignmentOrigin( node, elem );
        kege::vec2 p = origin;

        extent.width = node->quad.width - (elem->padding.left + elem->padding.right);
        extent.height = node->quad.height - (elem->padding.above + elem->padding.below);
        min = {65536,65536};
        max = {0,0};

        float max_l = 0;
        float stretch = 0;

        uint32_t index, begin = node->head;
        for (index = begin; index != 0 ; index = layout.next( index ) )
        {
            kege::ui::Node* n = layout[index];
            kege::ui::Elem* e = layout.elem( n );

            if ( e->position == Positioning::Independent )
            {
                continue;
            }
            else if ( e->position == Positioning::Absolute )
            {
                n->quad.x += origin.x;
                n->quad.y += origin.y;
                continue;
            }
            switch ( elem->alignment.direction.x )
            {
                case AlignDir::UP:
                    if( !advanceUp( elem->alignment, origin, extent, p, max_l, stretch, n ) )
                    {
                        stretch = 0;
                        handleOverflow( elem->alignment, origin, p, n->quad.height );
                        advanceUp( elem->alignment, origin, extent, p, max_l, stretch, n );
                    }
                    break;

                case AlignDir::DOWN:
                    if( !advanceDown( elem->alignment, origin, extent, p, max_l, stretch, n ) )
                    {
                        stretch = 0;
                        handleOverflow( elem->alignment, origin, p, n->quad.height );
                        advanceDown( elem->alignment, origin, extent, p, max_l, stretch, n );
                    }
                    break;

                case AlignDir::LEFT:
                    if( !advanceLeft( elem->alignment, origin, extent, p, max_l, stretch, n ) )
                    {
                        stretch = 0;
                        handleOverflow( elem->alignment, origin, p, n->quad.height );
                        advanceLeft( elem->alignment, origin, extent, p, max_l, stretch, n );
                    }
                    break;

                case AlignDir::RIGHT:
                    if( !advanceRight( elem->alignment, origin, extent, p, max_l, stretch, n ) )
                    {
                        stretch = 0;
                        handleOverflow( elem->alignment, origin, p, n->quad.height );
                        advanceRight( elem->alignment, origin, extent, p, max_l, stretch, n );
                    }
                    break;

                default:
                {
                    n->quad.x = origin.x - n->quad.width * 0.5;
                    n->quad.y = origin.y - n->quad.height * 0.5;
                    break;
                }
            }
            max.width = kege::max(max.width, p.x + n->quad.width);
            max.height = kege::max(max.height, p.y + n->quad.height);
            min.width = kege::min(min.width, p.x);
            min.height = kege::min(min.height, p.y);
        }
    }

    void Aligner::align( Layout& layout, uint32_t pid )
    {
        kege::ui::Node* n = layout[pid];
        kege::ui::Extent extent = {};
        kege::ui::Extent min = {};
        kege::ui::Extent max = {};

        alignerize( layout, n, extent, min, max );
        //offsetContent( layout, n, extent, min, max );

        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            if (layout[eid]->count != 0)
            {
                align( layout, eid );
            }
        }
    }

}
