//
//  ui-transform.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-transform.hpp"


namespace kege::ui
{
    bool transform( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        Transform* transform = ecs->get< kege::Transform >( entity );

        gui->push({ .layer = layer, .style = &gui->_theme.padded_list });
        ui::Text position_label
        {
            .ptr = "Position:",
            .width = 50,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        gui->scrubber3
        (
            UI_BASE_ID(),
            layer,
            position_label,
            transform->position.x,
            transform->position.y,
            transform->position.z
        );

        ui::Text scale_label
        {
            .ptr = "Scale:",
            .width = 50,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        gui->scrubber3
        (
            UI_BASE_ID(),
            layer,
            scale_label,
            transform->scale.x,
            transform->scale.y,
            transform->scale.z
        );

        ui::Text orientation_label
        {
            .ptr = "Orientation:",
            .width = 50,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        gui->scrubber4
        (
            UI_BASE_ID(),
            layer,
            orientation_label,
            transform->orientation.x,
            transform->orientation.y,
            transform->orientation.z,
            transform->orientation.w
        );
        gui->pop();
        return false;
    }
}
