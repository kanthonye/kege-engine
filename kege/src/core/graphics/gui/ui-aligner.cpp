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

    vec2 calcPos( const Style* s, const Rect& rect, const ui::Extent& extent )
    {
        vec2 p;
        switch ( s->align.content.x )
        {
            case AlignPosX::LEFT:   p.x = rect.x + s->padding.left; break;
            case AlignPosX::CENTER: p.x = rect.x + (rect.width - extent.width) * 0.5f; break;
            case AlignPosX::RIGHT:  p.x = rect.x + rect.width - extent.width - s->padding.right; break;
        }

        switch ( s->align.content.y )
        {
            case AlignPosY::TOP:    p.y = rect.y + s->padding.above; break;
            case AlignPosY::CENTER: p.y = rect.y + (rect.height - extent.height) * 0.5f; break;
            case AlignPosY::BOTTOM: p.y = rect.y + rect.height - extent.height - s->padding.below; break;
        }
        return p;
    }

    vec2 calcPos( const Style* s, const vec2& pos, const ui::Extent& extent )
    {
        vec2 p;
        switch ( s->align.origin.x )
        {
            case AlignPosX::LEFT:   p.x = pos.x /*+ s->padding.left*/; break;
            case AlignPosX::CENTER: p.x = pos.x + extent.width * 0.5; break;
            case AlignPosX::RIGHT:  p.x = pos.x + extent.width /*+ s->padding.right*/; break;
        }

        switch ( s->align.origin.y )
        {
            case AlignPosY::TOP:    p.y = pos.y /*+ s->padding.above*/; break;
            case AlignPosY::CENTER: p.y = pos.y + extent.height * 0.5f; break;
            case AlignPosY::BOTTOM: p.y = pos.y + extent.height /*+ s->padding.below*/; break;
        }
        return p;
    }

    struct Boundary
    {
        std::vector< ui::Extent > rects;
        Extent max;
    };

    Boundary computeInnerBoundaries( Layout& layout, NodeIndex pid, const Rect& outer_boundary )
    {
        Boundary boundary;
        boundary.max = {};
        boundary.rects.reserve(16);

        Widget* p = layout[ pid ];
        Rect rect = {};
        //rect.x = p->style->padding.left;
        //rect.y = p->style->padding.above;
        rect.height = 0.f;
        rect.width = 0.f;

        float x = 0.f;
        float curr = 0.f;

        float max_width = p->rect.width;// - p->style->padding.left - p->style->padding.right;
        const float max_height = p->rect.height;// - p->style->padding.above - p->style->padding.below;

        Extent extent = {};
        if( p->style->align.direction == ui::AlignDir::HORIZONTAL )
        {
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                Widget* e = layout[ eid ];
                extent.height = kege::max(extent.height, e->rect.height);

                if ( e->style->position == Positioning::Independent || e->style->position == Positioning::Absolute )
                {
                    continue;
                }

                extent.width = curr;
                curr += e->rect.width + p->style->gap.width;
                if (curr > max_width && p->style->align.wrap_around)
                {
                    x += extent.height;
                    boundary.rects.push_back(extent);
                    curr = e->rect.width + p->style->gap.width;
                }
                boundary.max.height = kege::max(boundary.max.height, x);
                boundary.max.width = kege::max(boundary.max.width, curr);
            }
            boundary.max.height += extent.height;
            boundary.rects.push_back(extent);
        }
        else
        {
            for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                Widget* e = layout[ eid ];
                extent.width = kege::max(extent.width, e->rect.width);

                if ( e->style->position == Positioning::Independent || e->style->position == Positioning::Absolute )
                {
                    continue;
                }

                extent.height = curr;
                curr += e->rect.height + p->style->gap.height;
                if (curr > max_height && p->style->align.wrap_around)
                {
                    x += extent.width;
                    boundary.rects.push_back(extent);
                    curr = e->rect.height + p->style->gap.height;
                }
                boundary.max.height = kege::max(boundary.max.height, curr);
                boundary.max.width = kege::max(boundary.max.width, x);
            }
            boundary.max.width += extent.width;
            boundary.rects.push_back(extent);
        }

        return boundary;
    }

    void alignVertically( Layout& layout, NodeIndex pid )
    {
        const Widget* p = layout[ pid ];
        Boundary boundaries = computeInnerBoundaries( layout, pid, p->rect );
        vec2 min = calcPos( p->style, p->rect, boundaries.max);
        vec2 start = calcPos( p->style, min, boundaries.max );
        vec2 pos = start;

        int count = 0;
        float threshold = 0;
        if (p->style->align.flow.x == AlignDirX::ETW)
        {
            pos.x -= boundaries.rects[0].width;
            threshold = start.y - boundaries.max.height;
        }
        else
        {
            threshold = min.y + boundaries.max.height;

        }

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            Widget* e = layout[ eid ];
            const ui::Extent* rect = &boundaries.rects[ count ];

            if ( e->style->position == Positioning::Independent )
            {
                e->rect.x = e->offset.x;
                e->rect.y = e->offset.y;
            }
            else if ( e->style->position == Positioning::Absolute )
            {
                e->rect.x = pos.x + e->offset.x;
                e->rect.y = pos.y + e->offset.y;
            }
            else
            {
                if (p->style->align.flow.y == AlignDirY::NTS)
                {
                    if (pos.y >= threshold && p->style->align.wrap_around)
                    {
                        if (p->style->align.flow.x == AlignDirX::ETW)
                        {
                            pos.x -= rect->width;
                        }
                        else
                        {
                            pos.x += rect->width;
                        }
                        pos.y = start.y;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x = pos.x + e->offset.x;
                    e->rect.y = pos.y + e->offset.y;
                    pos.y += e->rect.height + p->style->gap.height;
                }
                else //p->style->align_y == AlignDirY::SouthToNorth
                {
                    pos.y -= e->rect.height;
                    if (pos.y < threshold && p->style->align.wrap_around)
                    {
                        if (p->style->align.flow.x == AlignDirX::ETW)
                        {
                            pos.x -= rect->width;
                        }
                        else
                        {
                            pos.x += rect->width;
                        }
                        pos.y = start.y - e->rect.height - p->style->gap.height;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x = pos.x + e->offset.x;
                    e->rect.y = pos.y + e->offset.y;
                    pos.y -= p->style->gap.height;
                }
            }
        }
    }

    void alignHorizontally( Layout& layout, NodeIndex pid )
    {
        const Widget* p = layout[ pid ];
        Boundary boundaries = computeInnerBoundaries( layout, pid, p->rect );
        vec2 min = calcPos( p->style, p->rect, boundaries.max);
        vec2 start = calcPos( p->style, min, boundaries.max );
        vec2 pos = start;

        int count = 0;
        float threshold = 0;
        if (p->style->align.flow.x == AlignDirX::ETW)
        {
            pos.x = start.x;
            threshold = start.x - boundaries.max.width;
        }
        else
        {
            threshold = min.x + boundaries.max.width;

        }

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            Widget* e = layout[ eid ];
            const ui::Extent* rect = &boundaries.rects[ count ];

            if ( e->style->position == Positioning::Independent )
            {
                e->rect.x = e->offset.x;
                e->rect.y = e->offset.y;
            }
            else if ( e->style->position == Positioning::Absolute )
            {
                e->rect.x = pos.x + e->offset.x;
                e->rect.y = pos.y + e->offset.y;
            }
            else
            {
                if (p->style->align.flow.x == AlignDirX::WTE)
                {
                    if (pos.x >= threshold && p->style->align.wrap_around)
                    {
                        if (p->style->align.flow.y == AlignDirY::STN)
                        {
                            pos.y -= rect->height;
                        }
                        else
                        {
                            pos.y += rect->height;
                        }
                        pos.x = start.x;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x = pos.x + e->offset.x;
                    e->rect.y = pos.y + e->offset.y;
                    pos.x += e->rect.width + p->style->gap.width;
                }
                else //p->style->align_y == AlignDirY::SouthToNorth
                {
                    pos.x -= e->rect.width;
                    if (pos.x < threshold && p->style->align.wrap_around)
                    {
                        if (p->style->align.flow.y == AlignDirY::STN)
                        {
                            pos.y -= rect->height;
                        }
                        else
                        {
                            pos.y += rect->height;
                        }
                        pos.x = start.x - e->rect.width;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x = pos.x + e->offset.x;
                    e->rect.y = pos.y + e->offset.y;
                    pos.x -= p->style->gap.width;
                }
            }
        }
    }

    void alignChildren( Layout& layout, NodeIndex pid )
    {
        Widget* p = layout[ pid ];
        switch ( p->style->align.direction )
        {
            case AlignDir::HORIZONTAL:
            {
                alignHorizontally( layout, pid );
                break;
            }
            case AlignDir::VERTICAL:
            {
                alignVertically( layout, pid );
                break;
            }
            default:
            {
                break;
            }
        }

        for (NodeIndex eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            alignChildren( layout, eid );
        }
    }

    void align( Layout& layout, NodeIndex pid )
    {
        Widget* e = layout[ pid ];
        e->rect.x = e->offset.x;
        e->rect.y = e->offset.y;
        switch ( e->style->align.direction )
        {
            case AlignDir::HORIZONTAL:
            {
                alignHorizontally( layout, pid );
                break;
            }
            case AlignDir::VERTICAL:
            {
                alignVertically( layout, pid );
                break;
            }
            default:
            {
                break;
            }
        }
        alignChildren( layout, pid );
    }

    void Aligner::align( Layout& layout )
    {
        for (int root_index = 0; root_index < layout._root_count; ++root_index)
        {
            ui::align( layout, layout._roots[ root_index ] );
        }
    }
}
