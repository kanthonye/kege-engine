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

    vec2 calcPos( const Alignment& alignment, const Padding& padding, const Rect& rect, const ui::Extent& extent )
    {
        vec2 p;
        switch ( alignment.content.x )
        {
            case AlignPosX::LEFT:   p.x = rect.x + padding.left; break;
            case AlignPosX::CENTER: p.x = rect.x + (rect.width - extent.width) * 0.5f; break;
            case AlignPosX::RIGHT:  p.x = rect.x + rect.width - extent.width - padding.right; break;
        }

        switch ( alignment.content.y )
        {
            case AlignPosY::TOP:    p.y = rect.y + padding.above; break;
            case AlignPosY::CENTER: p.y = rect.y + (rect.height - extent.height) * 0.5f; break;
            case AlignPosY::BOTTOM: p.y = rect.y + rect.height - extent.height - padding.below; break;
        }
        return p;
    }

    vec2 calcPos( const Alignment& alignment, const vec2& pos, const ui::Extent& extent )
    {
        vec2 p;
        switch ( alignment.origin.x )
        {
            case AlignPosX::LEFT:   p.x = pos.x /*+ s->padding.left*/; break;
            case AlignPosX::CENTER: p.x = pos.x + extent.width * 0.5; break;
            case AlignPosX::RIGHT:  p.x = pos.x + extent.width /*+ s->padding.right*/; break;
        }

        switch ( alignment.origin.y )
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

    Boundary computeInnerBoundaries( Layout& layout, uint32_t pid, const Alignment& alignment, const Extent& gap, const Rect& outer_boundary )
    {
        Boundary boundary;
        boundary.max = {};
        boundary.rects.reserve(16);

        Widget* p = layout[ pid ];
        Rect rect = {};

        rect.height = 0.f;
        rect.width = 0.f;

        float x = 0.f;
        float curr = 0.f;

        float max_width = p->rect.width;// - p->style->padding.left - p->style->padding.right;
        const float max_height = p->rect.height;// - p->style->padding.above - p->style->padding.below;

        Extent extent = {};
        if( alignment.direction == ui::AlignDir::HORIZONTAL )
        {
            for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                Widget* e = layout[ eid ];
                Positioning positioning = (e->style)? e->style->position: Positioning::Relative;
                extent.height = kege::max(extent.height, e->rect.height);

                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
                {
                    continue;
                }

                extent.width = curr;
                curr += e->rect.width + gap.width;
                if (curr > max_width && alignment.wrap_around)
                {
                    x += extent.height;
                    boundary.rects.push_back(extent);
                    curr = e->rect.width + gap.width;
                }
                boundary.max.height = kege::max(boundary.max.height, x);
                boundary.max.width = kege::max(boundary.max.width, curr);
            }
            boundary.max.height += extent.height;
            boundary.rects.push_back(extent);
        }
        else
        {
            for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
            {
                Widget* e = layout[ eid ];
                Positioning positioning = (e->style)? e->style->position: Positioning::Relative;
                extent.width = kege::max(extent.width, e->rect.width);

                if ( positioning == Positioning::Independent || positioning == Positioning::Absolute )
                {
                    continue;
                }

                extent.height = curr;
                curr += e->rect.height + gap.height;
                if (curr > max_height && alignment.wrap_around)
                {
                    x += extent.width;
                    boundary.rects.push_back(extent);
                    curr = e->rect.height + gap.height;
                }
                boundary.max.height = kege::max(boundary.max.height, curr);
                boundary.max.width = kege::max(boundary.max.width, x);
            }
            boundary.max.width += extent.width;
            boundary.rects.push_back(extent);
        }

        return boundary;
    }

    void alignVertically( Layout& layout, uint32_t pid, const Alignment& alignment )
    {
        const Widget* p = layout[ pid ];

        Extent gap = (p->style != nullptr)? p->style->gap : Extent{};
        const Padding& padding = (p->style != nullptr) ? p->style->padding : p->padding;
        Boundary boundaries = computeInnerBoundaries( layout, pid, alignment, gap, p->rect );

        vec2 min = calcPos( alignment, padding, p->rect, boundaries.max);
        vec2 start = calcPos( alignment, min, boundaries.max );
        vec2 pos = start;

        int count = 0;
        float threshold = 0;
        if (alignment.flow.x == AlignDirX::ETW)
        {
            pos.x -= boundaries.rects[0].width;
            threshold = start.y - boundaries.max.height;
        }
        else
        {
            threshold = min.y + boundaries.max.height;
        }

        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            Widget* e = layout[ eid ];
            const ui::Extent* rect = &boundaries.rects[ count ];

            if ( e->position == Positioning::Independent )
            {
                continue;
            }
            else if ( e->position == Positioning::Absolute )
            {
                e->rect.x = pos.x;// + e->offset.x;
                e->rect.y = pos.y;// + e->offset.y;
            }
            else
            {
                if (alignment.flow.y == AlignDirY::NTS)
                {
                    if (pos.y >= threshold && alignment.wrap_around)
                    {
                        if (alignment.flow.x == AlignDirX::ETW)
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
                    e->rect.x += pos.x;// + e->offset.x;
                    e->rect.y += pos.y;// + e->offset.y;
                    pos.y += e->rect.height + gap.height;
                }
                else //p->style->align_y == AlignDirY::SouthToNorth
                {
                    pos.y -= e->rect.height;
                    if (pos.y < threshold && alignment.wrap_around)
                    {
                        if (alignment.flow.x == AlignDirX::ETW)
                        {
                            pos.x -= rect->width + gap.width;
                        }
                        else
                        {
                            pos.x += rect->width + gap.width;
                        }
                        pos.y = start.y - e->rect.height - gap.height;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x += pos.x;// + e->offset.x;
                    e->rect.y += pos.y;// + e->offset.y;
                    pos.y -= gap.height;
                }
            }
        }
    }

    void alignHorizontally( Layout& layout, uint32_t pid, const Alignment& alignment )
    {
        const Widget* p = layout[ pid ];

        Extent gap = (p->style != nullptr)? p->style->gap : Extent{};
        const Padding& padding = (p->style != nullptr) ? p->style->padding : p->padding;
        Boundary boundaries = computeInnerBoundaries( layout, pid, alignment, gap, p->rect );

        vec2 min = calcPos( alignment, padding, p->rect, boundaries.max);
        vec2 start = calcPos( alignment, min, boundaries.max );
        vec2 pos = start;

        int count = 0;
        float threshold = 0;
        if (alignment.flow.x == AlignDirX::ETW)
        {
            pos.x = start.x;
            threshold = start.x - boundaries.max.width;
        }
        else
        {
            threshold = min.x + boundaries.max.width;
        }

        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            Widget* e = layout[ eid ];
            const ui::Extent* rect = &boundaries.rects[ count ];

            if ( e->position == Positioning::Independent )
            {
                continue;
            }
            else if ( e->position == Positioning::Absolute )
            {
                e->rect.x += pos.x;// + e->offset.x;
                e->rect.y += pos.y;// + e->offset.y;
            }
            else
            {
                if (alignment.flow.x == AlignDirX::WTE)
                {
                    if (pos.x >= threshold && alignment.wrap_around)
                    {
                        if (alignment.flow.y == AlignDirY::STN)
                        {
                            pos.y -= rect->height + gap.height;
                        }
                        else
                        {
                            pos.y += rect->height + gap.height;
                        }
                        pos.x = start.x;

                        if (count + 1 < boundaries.rects.size())
                        {
                            count += 1;
                            if (count < boundaries.rects.size()) rect = &boundaries.rects[ count ];
                        }
                    }
                    e->rect.x += pos.x;// + e->offset.x;
                    e->rect.y += pos.y;// + e->offset.y;
                    pos.x += e->rect.width + gap.width;
                }
                else //p->style->align_y == AlignDirY::SouthToNorth
                {
                    pos.x -= e->rect.width;
                    if (pos.x < threshold && alignment.wrap_around)
                    {
                        if (alignment.flow.y == AlignDirY::STN)
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
                    e->rect.x += pos.x;// + e->offset.x;
                    e->rect.y += pos.y;// + e->offset.y;
                    pos.x -= gap.width;
                }
            }
        }
    }

    void alignChildren( Layout& layout, uint32_t pid )
    {
        Widget* p = layout[ pid ];
        Alignment alignment = getAlignment(p);

        switch ( alignment.direction )
        {
            case AlignDir::HORIZONTAL:
            {
                alignHorizontally( layout, pid, alignment );
                break;
            }
            case AlignDir::VERTICAL:
            {
                alignVertically( layout, pid, alignment );
                break;
            }
            default:
            {
                break;
            }
        }

        for (uint32_t eid = layout.head( pid ); eid != 0 ; eid = layout.next( eid ) )
        {
            alignChildren( layout, eid );
        }
    }

    void Aligner::align( Layout& layout, uint32_t pid )
    {
        Widget* p = layout[ pid ];
        Alignment alignment = getAlignment(p);

        if( p->style != nullptr )
        {
            switch ( p->style->align.direction )
            {
                case AlignDir::HORIZONTAL:
                {
                    alignHorizontally( layout, pid, alignment );
                    break;
                }
                case AlignDir::VERTICAL:
                {
                    alignVertically( layout, pid, alignment );
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        alignChildren( layout, pid );
    }

}
