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
                .text = {.ptr = "X:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec2, y, kege::ui::PropertyMeta{
                .text = {.ptr = "Y:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        )
    };

    const kege::ui::Property<kege::vec3> Vec3Properties[3] =
    {
        UI_PROP
        (
            kege::vec3, x, kege::ui::PropertyMeta{
                .text = {.ptr = "X:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec3, y, kege::ui::PropertyMeta{
                .text = {.ptr = "Y:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec3, z, kege::ui::PropertyMeta{
                .text = {.ptr = "Z:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        )
    };

    const kege::ui::Property<kege::vec4> Vec4Properties[4] =
    {
        UI_PROP
        (
            kege::vec4, x, kege::ui::PropertyMeta{
                .text = {.ptr = "X:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec4, y, kege::ui::PropertyMeta{
                .text = {.ptr = "Y:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec4, z, kege::ui::PropertyMeta{
                .text = {.ptr = "Z:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::vec4, w, kege::ui::PropertyMeta{
                .text = {.ptr = "W:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        )
    };

    const kege::ui::Property<kege::quat> QuatProperties[2] =
    {
        UI_PROP
        (
            kege::quat, w, kege::ui::PropertyMeta{
                .text = {.ptr = "Rotation:", 0.f, 0.f, 60.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Float
            }
        ),
        UI_PROP
        (
            kege::quat, xyz, kege::ui::PropertyMeta{
                .text = {.ptr = "Axis:", 0.f, 0.f, 20.f, 20.f },
                .hint = {kege::ui::UIHint::DragNum, false},
                .type = PropertyType::Vec3
            }
        ),
    };

}
