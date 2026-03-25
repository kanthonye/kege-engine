//
//  ui-dock-split.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/13/26.
//

#include "ui-dock-split.hpp"

namespace kege::ui{

    DockSplit::DockSplit(Dock& dock, float slit_ratio, ui::Dock::SplitDirection direction)
    :   slit_ratio( slit_ratio )
    ,   direction( direction )
    ,   gap( 6 )
    ,   owner(&dock)
    {
        nodes[0]._parent = &dock;
        nodes[1]._parent = &dock;
        nodes[0]._editor = dock._editor;
        nodes[1]._editor = dock._editor;
        nodes[0]._gui = dock._gui;
        nodes[1]._gui = dock._gui;
        onReshape();
    }

    void DockSplit::updateHorizontalDragRect()
    {
        drag_rect.x = nodes[0]._rect.x + nodes[0]._rect.width;
        drag_rect.y = nodes[0]._rect.y;
        drag_rect.width = nodes[1]._rect.x - drag_rect.x;
        drag_rect.height = nodes[0]._rect.height;
    }

    void DockSplit::updateVerticalDragRect()
    {
        drag_rect.y = nodes[0]._rect.y + nodes[0]._rect.height;
        drag_rect.x = nodes[0]._rect.x;
        drag_rect.width = nodes[0]._rect.width;
        drag_rect.height = nodes[1]._rect.y - drag_rect.y;
    }

    void DockSplit::onReshape()
    {
        if (this->direction == ui::Dock::SplitDirection::VERTICAL)
        {
            float height = owner->_rect.height - (gap + owner->_padding.above + owner->_padding.below);

            nodes[0]._rect.height = height * slit_ratio;
            nodes[0]._rect.width  = owner->_rect.width - (owner->_padding.left + owner->_padding.above);
            nodes[0]._rect.x      = owner->_rect.x + owner->_padding.left;
            nodes[0]._rect.y      = owner->_rect.y + owner->_padding.above;

            nodes[1]._rect.height = height - nodes[0]._rect.height;
            nodes[1]._rect.width  = nodes[0]._rect.width;
            nodes[1]._rect.x      = nodes[0]._rect.x;
            nodes[1]._rect.y      = nodes[0]._rect.y + nodes[0]._rect.height + gap;

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_rect.width - (gap + owner->_padding.left + owner->_padding.right);

            nodes[0]._rect.width  = width * slit_ratio;
            nodes[0]._rect.height = owner->_rect.height - (owner->_padding.above + owner->_padding.below);
            nodes[0]._rect.x      = owner->_rect.x + owner->_padding.left;
            nodes[0]._rect.y      = owner->_rect.y + owner->_padding.above;

            nodes[1]._rect.width  = width - nodes[0]._rect.width;
            nodes[1]._rect.height = nodes[0]._rect.height;
            nodes[1]._rect.x      = nodes[0]._rect.x + nodes[0]._rect.width + gap;
            nodes[1]._rect.y      = nodes[0]._rect.y;

            updateHorizontalDragRect();
        }
        if ( nodes[0]._split ) nodes[0]._split->onReshape();
        if ( nodes[1]._split ) nodes[1]._split->onReshape();
    }

    void DockSplit::onResizeGrandChild(ui::Dock::SplitDirection align_direction, const Rect& drag_rect_resizer)
    {
        float closest_1, closest_2, length;

        if (align_direction == ui::Dock::SplitDirection::VERTICAL && direction == align_direction)
        {
            closest_1 = drag_rect_resizer.y - nodes[0]._rect.y;
            closest_2 = drag_rect_resizer.y - nodes[1]._rect.y;
            if (closest_1 < closest_2)
            {
                nodes[0]._rect.y = owner->_rect.y + owner->_padding.above;
                nodes[0]._rect.height = owner->_rect.height - nodes[1]._rect.height - gap - owner->_padding.above;
                length = nodes[1]._rect.height;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_rect_resizer);
            }
            else
            {
                nodes[1]._rect.y = nodes[0]._rect.y + gap + nodes[0]._rect.height;
                nodes[1]._rect.height = owner->_rect.height - nodes[0]._rect.height - gap - owner->_padding.below;
                length = nodes[0]._rect.height;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_rect_resizer);
            }
            slit_ratio = length / (owner->_rect.height - owner->_padding.above - owner->_padding.below - gap);
        }

        else if (align_direction == ui::Dock::SplitDirection::HORIZONTAL && direction == align_direction)
        {
            closest_1 = drag_rect_resizer.x - nodes[0]._rect.x;
            closest_2 = drag_rect_resizer.x - nodes[1]._rect.x;

            if (closest_1 < closest_2)
            {
                nodes[0]._rect.x = owner->_rect.x + owner->_padding.left;
                nodes[0]._rect.width = owner->_rect.width - nodes[1]._rect.width - gap - owner->_padding.left;
                length = nodes[1]._rect.width;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_rect_resizer);
            }
            else
            {
                nodes[1]._rect.x = nodes[0]._rect.x + nodes[0]._rect.width + gap;
                nodes[1]._rect.width = owner->_rect.width - nodes[0]._rect.width - gap - owner->_padding.right;
                length = nodes[0]._rect.width;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_rect_resizer);
            }
            slit_ratio = length / (owner->_rect.width - owner->_padding.left - owner->_padding.right - gap);
        }

        else
        {
            onResize(align_direction, drag_rect_resizer);
        }
    }

    void DockSplit::onResize(ui::Dock::SplitDirection align_direction, const Rect& drag_rect)
    {
        if (direction == ui::Dock::SplitDirection::VERTICAL)
        {
            float height = owner->_rect.height
                         - gap
                         - owner->_padding.above
                         - owner->_padding.below;

            float h0 = height * slit_ratio;
            float h1 = height - h0;

            nodes[0]._rect = {
                owner->_rect.x + owner->_padding.left,
                owner->_rect.y + owner->_padding.above,
                owner->_rect.width - owner->_padding.left - owner->_padding.right,
                h0
            };

            nodes[1]._rect = {
                nodes[0]._rect.x,
                nodes[0]._rect.y + h0 + gap,
                nodes[0]._rect.width,
                h1
            };

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_rect.width
                        - gap
                        - owner->_padding.left
                        - owner->_padding.right;

            float w0 = width * slit_ratio;
            float w1 = width - w0;

            nodes[0]._rect = {
                owner->_rect.x + owner->_padding.left,
                owner->_rect.y + owner->_padding.above,
                w0,
                owner->_rect.height - owner->_padding.above - owner->_padding.below
            };

            nodes[1]._rect = {
                nodes[0]._rect.x + w0 + gap,
                nodes[0]._rect.y,
                w1,
                nodes[0]._rect.height
            };

            updateHorizontalDragRect();
        }

        if (nodes[0]._split)
        {
            nodes[0]._split->onResizeGrandChild(align_direction, drag_rect);
        }
        if (nodes[1]._split)
        {
            nodes[1]._split->onResizeGrandChild(align_direction, drag_rect);
        }
    }

    bool DockSplit::update(GUI* gui)
    {
        if (!gui->layout()->inputManager()->getMouse()->isDown(MouseButtonCode::Left))
            show_drag_knob = gui->testPointVsRect(gui->pointer(), drag_rect);

        else if (show_drag_knob)
        {
            if( gui->pointerDragging() )
            {
                float amount, available, delta_ratio;
                if( direction == Dock::SplitDirection::VERTICAL )
                {
                    amount = gui->deltaPointer().y;
                    available = owner->_rect.height - gap - owner->_padding.above - owner->_padding.below;
                    delta_ratio = amount / available;
                }
                else
                {
                    amount = gui->deltaPointer().x;
                    available = owner->_rect.width - gap - owner->_padding.left - owner->_padding.right;
                    delta_ratio = amount / available;
                }
                slit_ratio = clamp(slit_ratio + delta_ratio, 0.05f, 0.95f);

                onResize(direction, drag_rect);
            }
        }
        return show_drag_knob;
    }

    DockSplit::~DockSplit()
    {
    }
}
