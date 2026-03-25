//
//  ui-description.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/19/26.
//

#ifndef ui_description_hpp
#define ui_description_hpp

#include "dependencies.hpp"
#include "ui-schema.hpp"

namespace kege::ui{
    
#define UI_META(ui_type, min, max) kege::ui::PropertyMeta{ ui_type, min, max }

#define UI_TEXT(str, x, y, w, h) kege::ui::Text{ str, x, y, w, h }

#define UI_PROP(classname, member, ptype, text, meta) \
    kege::ui::Property<classname>{ \
        text, \
        kege::ui::PropertyType::ptype, \
        offsetof(classname, member), \
        meta \
    }

    inline static const kege::ui::Property<kege::Perspective> PerspectivePropertie[] = {
        UI_PROP(
            kege::Perspective, fov, Double,
            UI_TEXT("Field Of View", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::Slider, 10.0f, 160.0f)
        ),
        UI_PROP(
            kege::Perspective, znear, Double,
            UI_TEXT("Z Near", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 0.00005, 999'999'999.0)
        ),
        UI_PROP(
            kege::Perspective, zfar, Double,
            UI_TEXT("Z Far", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 0.00005, 999'999'999.0)
        ),
    };

    inline static const kege::ui::Property<kege::Orthographic> OrthographicPropertie[] = {
        UI_PROP(
            kege::Orthographic, left, Double,
            UI_TEXT("Left", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, 160.0)
        ),
        UI_PROP(
            kege::Orthographic, right, Double,
            UI_TEXT("Right", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, 160.0)
        ),
        UI_PROP(
            kege::Orthographic, above, Double,
            UI_TEXT("Top", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, 160.0)
        ),
        UI_PROP(
            kege::Orthographic, below, Double,
            UI_TEXT("Bottom", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, 160.0f)
        ),
        UI_PROP(
            kege::Orthographic, znear, Double,
            UI_TEXT("ZNear", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, -999'999'999.0)
        ),
        UI_PROP(
            kege::Orthographic, zfar, Double,
            UI_TEXT("ZFar", 0.f, 0.f, 100.f, 20.f),
            UI_META(kege::ui::UIHint::DragNum, 10.0, 999'999'999.0)
        ),
    };

}
#endif /* ui_description_hpp */
