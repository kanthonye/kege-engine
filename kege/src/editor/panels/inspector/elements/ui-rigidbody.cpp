//
//  ui-rigidbody.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-rigidbody.hpp"

namespace kege::ui{
    
    bool rigidbody( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        kege::Rigidbody* rigidbody = ecs->get< kege::Rigidbody >( entity );

        gui->push({.layer = layer, .style = &gui->_theme.padded_list});
        ui::Text label_friction
        {
            .ptr = "Friction:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        ui::Text label_restitution
        {
            .ptr = "Restitution:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        ui::Text label_linear_damping
        {
            .ptr = "Linear Damping:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        ui::Text label_angular_damping
        {
            .ptr = "Angular Damping:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        ui::Text label_linear_velocity
        {
            .ptr = "Linear Velocity:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };
        ui::Text label_angular_velocity
        {
            .ptr = "Angular Velocity:",
            .width = 100,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->scrubber
        (
            UI_BASE_ID(),
            layer,
            label_friction,
            rigidbody->friction
        );

        gui->scrubber
        (
            UI_BASE_ID(),
            layer,
            label_restitution,
            rigidbody->cor
        );

        gui->scrubber3
        (
            UI_BASE_ID(),
            layer,
            label_linear_velocity,
            rigidbody->linear.velocity.x,
            rigidbody->linear.velocity.y,
            rigidbody->linear.velocity.z
        );

        gui->scrubber
        (
            UI_BASE_ID(),
            layer,
            label_linear_damping,
            rigidbody->linear.damping
        );

        gui->scrubber3
        (
            UI_BASE_ID(),
            layer,
            label_angular_velocity,
            rigidbody->angular.velocity.x,
            rigidbody->angular.velocity.y,
            rigidbody->angular.velocity.z
        );

        gui->scrubber
        (
            UI_BASE_ID(),
            layer,
            label_angular_damping,
            rigidbody->angular.damping
        );
        gui->pop();
        return false;
    }
}
