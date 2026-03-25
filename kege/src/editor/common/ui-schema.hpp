//
//  ui-schema.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/19/26.
//

#ifndef ui_schema_hpp
#define ui_schema_hpp

#include "gui.hpp"

namespace kege::ui{

    enum class UIHint {
        Default,

        Press,
        Click,
        Toggle,
        CheckBox,

        Slider,
        DragBar,
        DragNum,

        Input,     // numeric text input
    };

    enum class PropertyType {
        Float,
        Double,
        Int,
        Int64,
        Bool,
        String,
        Vec2,
        Vec3,
        Vec4,
        Mat22,
        Mat33,
        Mat44,
        Asset
    };

    struct PropertyMeta {
        ui::UIHint hint = ui::UIHint::Default;

        // optional blocks
        float min = 0.0f;
        float max = 1.0f;

        //AssetType assetType;0
    };

    template<typename Component>
    struct Property
    {
        kege::ui::Text label;
        PropertyType type;

        // raw offset instead of member pointer (important)
        size_t offset;

        PropertyMeta meta;
    };

    constexpr inline const kege::ui::Text& text(const kege::ui::Text& t){ return t; }
    constexpr inline const kege::ui::UIHint& hint(const kege::ui::UIHint& h){ return h; }

}


namespace kege::ui{

    bool drawFloat
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        float& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawDouble
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        double& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawInt
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        int& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawInt64
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        int64_t& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawBool
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        bool& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawString
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        std::string& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );


    bool drawVec2
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec2& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawVec3
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec3& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawVec4
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::vec4& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawMat22
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat22& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawMat33
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat33& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );

    bool drawMat44
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        kege::mat44& value,
        const PropertyMeta& meta,
        const kege::ui::Text& label
    );
    
}


namespace kege::ui{

    template<typename Component, size_t N>
    inline bool drawProperties
    (
        kege::GUI* gui,
        UserId user_id,
        int16_t layer,
        Component& instance,
        const Property<Component>(&props)[N]
    )
    {
        bool modified = false;
        for (const auto& p : props)
        {
            void* data = (char*)&instance + p.offset;

            switch (p.type)
            {
                case PropertyType::Float:
                {
                    float& v = *(float*)data;
                    modified = drawFloat(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Double:
                {
                    double& v = *(double*)data;
                    modified = drawDouble(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Int:
                {
                    int& v = *(int*)data;
                    modified = drawInt(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Int64:
                {
                    int64_t& v = *(int64_t*)data;
                    modified = drawInt64(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Bool:
                {
                    bool& v = *(bool*)data;
                    modified = drawBool(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::String:
                {
                    std::string& v = *(std::string*)data;
                    modified = drawString(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Vec2:
                {
                    kege::vec2& v = *(kege::vec2*)data;
                    modified = drawVec2(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Vec3:
                {
                    kege::vec3& v = *(kege::vec3*)data;
                    modified = drawVec3(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Vec4:
                {
                    kege::vec4& v = *(kege::vec4*)data;
                    modified = drawVec4(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Mat22:
                {
                    kege::mat22& v = *(kege::mat22*)data;
                    modified = drawMat22(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Mat33:
                {
                    kege::mat33& v = *(kege::mat33*)data;
                    modified = drawMat33(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Mat44:
                {
                    kege::mat44& v = *(kege::mat44*)data;
                    modified = drawMat44(gui, user_id, layer, v, p.meta, p.label);
                    break;
                }
                case PropertyType::Asset:
                {
                    //AssetHandle& v = *(AssetHandle*)data;

                    //if (DrawAsset(gui, layer, label, v, p.meta.assetType))
                    //    modified = true;

                    break;
                }
            }
        }
        return modified;
    }

}
#endif /* ui_schema_hpp */
