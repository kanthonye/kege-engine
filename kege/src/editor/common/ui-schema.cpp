//
//  ui-schema.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/19/26.
//

#include "ui-schema.hpp"
namespace kege::ui{

    const kege::ui::Property<kege::vec2> Vec2Properties[2] =
    {
        UI_PROP
        (
            kege::vec2, x, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "X:" },
            }
        ),
        UI_PROP
        (
            kege::vec2, y, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Y:", },
            }
        )
    };

    const kege::ui::Property<kege::vec3> Vec3Properties[3] =
    {
        UI_PROP
        (
            kege::vec3, x, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "X:",  },
            }
        ),
        UI_PROP
        (
            kege::vec3, y, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Y:",  },
            }
        ),
        UI_PROP
        (
            kege::vec3, z, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Z:",  },
            }
        )
    };

    const kege::ui::Property<kege::vec4> Vec4Properties[4] =
    {
        UI_PROP
        (
            kege::vec4, x, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "X:"},
            }
        ),
        UI_PROP
        (
            kege::vec4, y, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Y:", },
            }
        ),
        UI_PROP
        (
            kege::vec4, z, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Z:", },
            }
        ),
        UI_PROP
        (
            kege::vec4, w, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "W:", },
            }
        )
    };

    const kege::ui::Property<kege::quat> QuatProperties[2] =
    {
        UI_PROP
        (
            kege::quat, w, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 60.f, 20, 0xFFFFFFFF, "Rotation:"},
            }
        ),
        UI_PROP
        (
            kege::quat, xyz, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {0.f, 0.f, 20.f, 20, 0xFFFFFFFF, "Axis:"},
            }
        ),
    };

}
