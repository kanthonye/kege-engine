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
        drag_quad = {0,0,0,0,0xFFFFFF20};
        nodes[0]._parent = &dock;
        nodes[1]._parent = &dock;
        nodes[0]._manager = dock._manager;
        nodes[1]._manager = dock._manager;
        nodes[0]._ui = dock._ui;
        nodes[1]._ui = dock._ui;
        onReshape();
    }

    void DockSplit::updateHorizontalDragRect()
    {
        drag_quad.x = nodes[0]._quad.x + nodes[0]._quad.width;
        drag_quad.y = nodes[0]._quad.y;
        drag_quad.width = nodes[1]._quad.x - drag_quad.x;
        drag_quad.height = nodes[0]._quad.height;
    }

    void DockSplit::updateVerticalDragRect()
    {
        drag_quad.y = nodes[0]._quad.y + nodes[0]._quad.height;
        drag_quad.x = nodes[0]._quad.x;
        drag_quad.width = nodes[0]._quad.width;
        drag_quad.height = nodes[1]._quad.y - drag_quad.y;
    }

    void DockSplit::onReshape()
    {
        if (this->direction == ui::Dock::SplitDirection::VERTICAL)
        {
            float height = owner->_quad.height - (gap + owner->_padding.above + owner->_padding.below);

            nodes[0]._quad.height = height * slit_ratio;
            nodes[0]._quad.width  = owner->_quad.width - (owner->_padding.left + owner->_padding.above);
            nodes[0]._quad.x      = owner->_quad.x + owner->_padding.left;
            nodes[0]._quad.y      = owner->_quad.y + owner->_padding.above;

            nodes[1]._quad.height = height - nodes[0]._quad.height;
            nodes[1]._quad.width  = nodes[0]._quad.width;
            nodes[1]._quad.x      = nodes[0]._quad.x;
            nodes[1]._quad.y      = nodes[0]._quad.y + nodes[0]._quad.height + gap;

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_quad.width - (gap + owner->_padding.left + owner->_padding.right);

            nodes[0]._quad.width  = width * slit_ratio;
            nodes[0]._quad.height = owner->_quad.height - (owner->_padding.above + owner->_padding.below);
            nodes[0]._quad.x      = owner->_quad.x + owner->_padding.left;
            nodes[0]._quad.y      = owner->_quad.y + owner->_padding.above;

            nodes[1]._quad.width  = width - nodes[0]._quad.width;
            nodes[1]._quad.height = nodes[0]._quad.height;
            nodes[1]._quad.x      = nodes[0]._quad.x + nodes[0]._quad.width + gap;
            nodes[1]._quad.y      = nodes[0]._quad.y;

            updateHorizontalDragRect();
        }
        if ( nodes[0]._split ) nodes[0]._split->onReshape();
        if ( nodes[1]._split ) nodes[1]._split->onReshape();
    }

    void DockSplit::onResizeGrandChild(ui::Dock::SplitDirection align_direction, const Quad& drag_quad_resizer)
    {
        float closest_1, closest_2, length;

        if (align_direction == ui::Dock::SplitDirection::VERTICAL && direction == align_direction)
        {
            closest_1 = drag_quad_resizer.y - nodes[0]._quad.y;
            closest_2 = drag_quad_resizer.y - nodes[1]._quad.y;
            if (closest_1 < closest_2)
            {
                nodes[0]._quad.y = owner->_quad.y + owner->_padding.above;
                nodes[0]._quad.height = owner->_quad.height - nodes[1]._quad.height - gap - owner->_padding.above;
                length = nodes[1]._quad.height;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_quad_resizer);
            }
            else
            {
                nodes[1]._quad.y = nodes[0]._quad.y + gap + nodes[0]._quad.height;
                nodes[1]._quad.height = owner->_quad.height - nodes[0]._quad.height - gap - owner->_padding.below;
                length = nodes[0]._quad.height;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_quad_resizer);
            }
            slit_ratio = length / (owner->_quad.height - owner->_padding.above - owner->_padding.below - gap);
        }

        else if (align_direction == ui::Dock::SplitDirection::HORIZONTAL && direction == align_direction)
        {
            closest_1 = drag_quad_resizer.x - nodes[0]._quad.x;
            closest_2 = drag_quad_resizer.x - nodes[1]._quad.x;

            if (closest_1 < closest_2)
            {
                nodes[0]._quad.x = owner->_quad.x + owner->_padding.left;
                nodes[0]._quad.width = owner->_quad.width - nodes[1]._quad.width - gap - owner->_padding.left;
                length = nodes[1]._quad.width;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_quad_resizer);
            }
            else
            {
                nodes[1]._quad.x = nodes[0]._quad.x + nodes[0]._quad.width + gap;
                nodes[1]._quad.width = owner->_quad.width - nodes[0]._quad.width - gap - owner->_padding.right;
                length = nodes[0]._quad.width;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_quad_resizer);
            }
            slit_ratio = length / (owner->_quad.width - owner->_padding.left - owner->_padding.right - gap);
        }

        else
        {
            onResize(align_direction, drag_quad_resizer);
        }
    }

    void DockSplit::onResize(ui::Dock::SplitDirection align_direction, const Quad& drag_quad)
    {
        if (direction == ui::Dock::SplitDirection::VERTICAL)
        {
            float height = owner->_quad.height
                         - gap
                         - owner->_padding.above
                         - owner->_padding.below;

            float h0 = height * slit_ratio;
            float h1 = height - h0;

            nodes[0]._quad = {
                owner->_quad.x + owner->_padding.left,
                owner->_quad.y + owner->_padding.above,
                owner->_quad.width - owner->_padding.left - owner->_padding.right,
                h0
            };

            nodes[1]._quad = {
                nodes[0]._quad.x,
                nodes[0]._quad.y + h0 + gap,
                nodes[0]._quad.width,
                h1
            };

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_quad.width
                        - gap
                        - owner->_padding.left
                        - owner->_padding.right;

            float w0 = width * slit_ratio;
            float w1 = width - w0;

            nodes[0]._quad = {
                owner->_quad.x + owner->_padding.left,
                owner->_quad.y + owner->_padding.above,
                w0,
                owner->_quad.height - owner->_padding.above - owner->_padding.below
            };

            nodes[1]._quad = {
                nodes[0]._quad.x + w0 + gap,
                nodes[0]._quad.y,
                w1,
                nodes[0]._quad.height
            };

            updateHorizontalDragRect();
        }

        if (nodes[0]._split)
        {
            nodes[0]._split->onResizeGrandChild(align_direction, drag_quad);
        }
        if (nodes[1]._split)
        {
            nodes[1]._split->onResizeGrandChild(align_direction, drag_quad);
        }
    }

    bool DockSplit::update(UI* ui)
    {
        if (!ui->gui()->getInputManager()->getMouse()->isDown(MouseButtonCode::Left))
            show_drag_knob = kege::ui::testPointVsRect(ui->pointer(), drag_quad);

        else if (show_drag_knob)
        {
            if( ui->pointerDragging() )
            {
                float amount, available, delta_ratio;
                if( direction == Dock::SplitDirection::VERTICAL )
                {
                    amount = ui->deltaPointer().y;
                    available = owner->_quad.height - gap - owner->_padding.above - owner->_padding.below;
                    delta_ratio = amount / available;
                }
                else
                {
                    amount = ui->deltaPointer().x;
                    available = owner->_quad.width - gap - owner->_padding.left - owner->_padding.right;
                    delta_ratio = amount / available;
                }
                slit_ratio = clamp(slit_ratio + delta_ratio, 0.05f, 0.95f);

                onResize(direction, drag_quad);
            }
        }
        return show_drag_knob;
    }

    DockSplit::~DockSplit()
    {
    }
}
