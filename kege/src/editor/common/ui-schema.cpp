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

    
//    bool drawFloat
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        float& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
////        switch (meta.hint)
////        {
////            case ui::UIHint::Slider:
////                //active = gui->slider(layer, &value, meta.min, meta.max);
////                break;
////
////            case ui::UIHint::DragBar:
////                ///active = gui->slidebar(layer, &value, meta.min, meta.max);
////                break;
////
////            case ui::UIHint::DragNum:
////                //active = gui->scrubber(id, layer, value);
////                break;
////
////            case ui::UIHint::Input:
////                //active = gui->scrubber(id, layer, value);
////                break;
////
////            default:
////                break;
////        }
//        return active;
//    }
//
//    bool drawDouble
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        double& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        //std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//            {
//                ui::ID id[2];
//                id[0] = uid[ id_offset++ ];
//                id[1] = uid[ id_offset++ ];
//                gui->push({.layer = layer, .style = &gui->theme().column});
//                gui->label(layer, label);
//                active = gui->slider<double>(id, layer, value, meta.min, meta.max);
//                gui->pop();
//                break;
//            }
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                gui->push({.layer = layer, .style = &gui->theme().row});
//                gui->label(layer, label);
//                active = gui->scrubber(uid[ id_offset++ ], layer, value, meta.min, meta.max);
//                gui->pop();
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawInt
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        int& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawInt64
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        int64_t& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawBool
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        bool& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Press:
//                break;
//
//            case ui::UIHint::Click:
//                break;
//
//            case ui::UIHint::Toggle:
//                break;
//
//            case ui::UIHint::CheckBox:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawString
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        std::string& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawVec2
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        kege::vec2& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawVec3
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        kege::vec3& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//
//    bool drawMat22
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        kege::mat22& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawMat33
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        kege::mat33& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//
//    bool drawMat44
//    (
//        kege::GUI* gui,
//        const ui::UID& uid,
//        uint32_t& id_offset,
//        int16_t layer,
//        kege::mat44& value,
//        const PropertyMeta& meta,
//        const kege::ui::Text& label
//    )
//    {
//        std::cout <<label.ptr << ": " << value <<"\n";
//        bool active = false;
//        switch (meta.hint)
//        {
//            case ui::UIHint::Slider:
//                break;
//
//            case ui::UIHint::DragBar:
//                break;
//
//            case ui::UIHint::DragNum:
//                break;
//
//            case ui::UIHint::Input:
//                break;
//
//            default:
//                break;
//        }
//        return active;
//    }
//    
}
