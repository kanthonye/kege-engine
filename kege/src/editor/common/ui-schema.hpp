//
//  ui-schema.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/19/26.
//

#ifndef ui_schema_hpp
#define ui_schema_hpp

#include "ui.hpp"

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
        Quat,
        Mat22,
        Mat33,
        Mat44,
        Asset
    };

    struct PropertyHint {
        ui::UIHint hint = ui::UIHint::Default;

        // optional blocks
        bool clamp = false;
        double min = 0.0f;
        double max = 1.0f;
    };

    struct PropertyMeta {
        PropertyType type;
        PropertyHint hint;
        kege::ui::Text text;
    };

    template<typename Component>
    struct Property
    {
        PropertyMeta meta;
        void* (*get)(Component&);
    };

    constexpr inline const kege::ui::Text& text(const kege::ui::Text& t){ return t; }
    constexpr inline const kege::ui::UIHint& hint(const kege::ui::UIHint& h){ return h; }
}


#define UI_META(ui_type, min, max) kege::ui::PropertyMeta{ ui_type, min, max }

#define UI_TEXT(str, x, y, w, h) kege::ui::Text{ str, x, y, w, h }

#define GET_MEMBER(classname, member) [](classname& p) -> void* { return &p.member; }

#define UI_PROP(classname, member, ...) \
    kege::ui::Property<classname>{ \
        .get = [](classname& p) -> void* { return &p.member; },\
        .meta = __VA_ARGS__\
    }


namespace kege::ui{

    template<typename DataType>
    bool drawSlider
    (
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        DataType& value,
        const PropertyMeta& meta
    )
    {
        ui::ID id[2];
        id[0] = uid[ id_offset++ ];
        id[1] = uid[ id_offset++ ];
        ui->push({.style = &ui->theme().column});
        ui->label(meta.text);
        bool active = ui->slider< DataType >(id, value, meta.hint.min, meta.hint.max);
        ui->pop();
        return active;
    }

    template<typename DataType>
    bool drawSlidebar
    (   ScrubberState::Type type,
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        DataType& value,
        const PropertyMeta& meta
    )
    {
        ui::ID id[2];
        id[0] = uid[ id_offset++ ];
        id[1] = uid[ id_offset++ ];
        ui->push({.style = &ui->theme().column});
        ui->label(meta.text);
        bool active = ui->slidebar< DataType >(id, value, meta.hint.min, meta.hint.max);
        ui->pop();
        return active;
    }

    template<typename DataType>
    bool drawDragNum
    (   ScrubberState::Type type,
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        DataType& value,
        const PropertyMeta& meta
    )
    {
        ui->push({.style = &ui->theme().row});
        ui->put
        ({
            .rect = {meta.text.x, meta.text.y, meta.text.width, meta.text.height},
            .text = meta.text.ptr,
            .color = 0xFFFFFF00,
            .mouseover = false
        });
        bool active = ui->scrubber< DataType >(type, uid[ id_offset++ ], value, meta.hint.min, meta.hint.max);
        ui->pop();
        return active;
    }

    template<typename DataType>
    bool drawNumericUI
    (
        ScrubberState::Type type,
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        DataType& value,
        const PropertyMeta& meta
    )
    {
        bool active = false;
        switch (meta.hint.hint)
        {
            case ui::UIHint::Slider:
                active = drawSlider
                (
                    ui,
                    uid,
                    id_offset,
                    value,
                    meta
                );
                break;

            case ui::UIHint::DragBar:
                active = drawSlidebar
                (
                    type,
                    ui,
                    uid,
                    id_offset,
                    value,
                    meta
                );
                break;

            case ui::UIHint::DragNum:
                active = drawDragNum
                (
                    type,
                    ui,
                    uid,
                    id_offset,
                    value,
                    meta
                );
                break;

            case ui::UIHint::Input:
                break;

            default:
                break;
        }
        return active;
    }

}


namespace kege::ui{

    extern const kege::ui::Property<kege::vec2> Vec2Properties[2];
    extern const kege::ui::Property<kege::vec3> Vec3Properties[3];
    extern const kege::ui::Property<kege::vec4> Vec4Properties[4];
    extern const kege::ui::Property<kege::quat> QuatProperties[2];

    template<typename Component, size_t N>
    inline bool drawProperties
    (
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        Component& instance,
        const Property<Component>(&props)[N]
    );

    template<typename Component>
    inline bool drawProperty
    (
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        Component& instance,
        const Property<Component>& p
    )
    {
        bool modified = false;
        void* data = p.get(instance);

        switch (p.meta.type)
        {
            case PropertyType::Float:
            {
                modified = drawNumericUI< float >( ScrubberState::F32, ui, uid, id_offset, *(float*)data, p.meta );
                break;
            }

            case PropertyType::Double:
            {
                modified = drawNumericUI< double >( ScrubberState::F64, ui, uid, id_offset, *(double*)data, p.meta );
                break;
            }

            case PropertyType::Int:
            {
                modified = drawNumericUI< int >( ScrubberState::I32, ui, uid, id_offset, *(int*)data, p.meta );
                break;
            }

            case PropertyType::Int64:
            {
                modified = drawNumericUI< int64_t >( ScrubberState::I64, ui, uid, id_offset, *(int64_t*)data, p.meta );
                break;
            }

            case PropertyType::Bool:
            {
                //modified = drawUI< bool >( ui, uid, id_offset, *(bool*)data, p.meta, p.label );
                break;
            }

            case PropertyType::String:
            {
                //const std::string& str = *(std::string*)data;
                //p.label.ptr = str.c_str();
                //ui->label(p.label);
                break;
            }

            case PropertyType::Vec2:
            {
                ui->push({.style = &ui->theme().group});
                ui->label(p.meta.text);
                modified = drawProperties
                (
                    ui,
                    uid,
                    id_offset,
                    *((kege::vec2*)(data)),
                    Vec2Properties
                );
                ui->pop();
                break;
            }

            case PropertyType::Vec3:
            {
                //ui->push({.style = &ui->theme().group});
                ui->label(p.meta.text);
                modified = drawProperties
                (
                    ui,
                    uid,
                    id_offset,
                    *((kege::vec3*)(data)),
                    Vec3Properties
                );
                //ui->pop();
                break;
            }

            case PropertyType::Vec4:
            {
                //ui->push({.style = &ui->theme().group});
                ui->label(p.meta.text);
                modified = drawProperties
                (
                    ui,
                    uid,
                    id_offset,
                    *((kege::vec4*)(data)),
                    Vec4Properties
                );
                //ui->pop();
                break;
            }

            case PropertyType::Quat:
            {
                //ui->push({.style = &ui->theme().group});
                ui->label(p.meta.text);
                modified = drawProperties
                (
                    ui,
                    uid,
                    id_offset,
                    *((kege::quat*)(data)),
                    QuatProperties
                );
                //ui->pop();
                break;
            }

            case PropertyType::Mat22:
            {
                //modified = drawUI< kege::mat22 >( ui, uid, id_offset, *(kege::mat22*)data, p.meta, p.label );
                break;
            }

            case PropertyType::Mat33:
            {
                //modified = drawUI< kege::mat33 >( ui, uid, id_offset, *(kege::mat33*)data, p.meta, p.label );
                break;
            }

            case PropertyType::Mat44:
            {
                //modified = drawUI< kege::mat44 >( ui, uid, id_offset, *(kege::mat44*)data, p.meta, p.label );
                break;
            }

            case PropertyType::Asset:
            {
                //AssetHandle& v = *(AssetHandle*)data;

                //if (DrawAsset(ui, label, v, p.meta.assetType))
                //    modified = true;

                break;
            }
        }
        return modified;
    }

    template<typename Component, size_t N>
    inline bool drawProperties
    (
        kege::UI* ui,
        const ui::UID& uid,
        uint32_t& id_offset,
        Component& instance,
        const Property<Component>(&props)[N]
    )
    {
        ui->push({.style = &ui->theme().group});
        bool modified = false;
        for ( auto& p : props)
        {
            if(drawProperty< Component >(
                ui,
                uid,
                id_offset,
                instance,
                p
            ))
            {
                modified = true;
            }
        }
        ui->pop();
        return modified;
    }

}

#endif /* ui_schema_hpp */
