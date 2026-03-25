//
//  ui-schema.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/19/26.
//

#include "ui-schema.hpp"
namespace kege::ui{

    bool drawFloat
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        float& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                //active = gui->slider(layer, &value, meta.min, meta.max);
                break;

            case ui::UIHint::DragBar:
                ///active = gui->slidebar(layer, &value, meta.min, meta.max);
                break;

            case ui::UIHint::DragNum:
                //active = gui->scrubber(id, layer, value);
                break;

            case ui::UIHint::Input:
                //active = gui->scrubber(id, layer, value);
                break;

            default:
                break;
        }
        return active;
    }

    bool drawDouble
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        double& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawInt
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        int& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawInt64
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        int64_t& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawBool
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        bool& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Press:
                break;

            case ui::UIHint::Click:
                break;

            case ui::UIHint::Toggle:
                break;

            case ui::UIHint::CheckBox:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawString
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        std::string& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawVec2
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec2& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawVec3
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec3& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawVec4
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec4& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawMat22
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat22& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawMat33
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat33& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

    bool drawMat44
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat44& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    )
    {
        std::cout <<label.ptr << ": " << value <<"\n";
        bool active = false;
        switch (meta.hint)
        {
            case ui::UIHint::Slider:
                break;

            case ui::UIHint::DragBar:
                break;

            case ui::UIHint::DragNum:
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }
    
}
