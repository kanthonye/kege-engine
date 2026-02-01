//
//  ui-rigidbody.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-rigidbody.hpp"

namespace kege::ui
{
    bool rigidbody( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        kege::Rigidbody* rigidbody = ecs->get< kege::Rigidbody >( entity );
        uint64_t component_id = ecs->getCompId< kege::Rigidbody >( entity );

        ui::Text main_label
        {
            .ptr = "Rigidbody",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->push({ .layer = layer, .style = &gui->_theme.card });
        switch (gui->removableHeader(layer, component_id, main_label))
        {
            case 1:
            {
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
                    layer,
                    gui->getAddressAsInt(rigidbody->friction),
                    label_friction,
                    rigidbody->friction
                );

                gui->scrubber
                (
                    layer,
                    gui->getAddressAsInt(rigidbody->cor),
                    label_restitution,
                    rigidbody->cor
                );

                gui->scrubber3
                (
                    layer,
                    gui->getAddressAsInt(rigidbody->linear.velocity),
                    label_linear_velocity,
                    rigidbody->linear.velocity.x,
                    rigidbody->linear.velocity.y,
                    rigidbody->linear.velocity.z
                );

                gui->scrubber
                (
                    layer,
                    gui->getAddressAsInt(rigidbody->linear.damping),
                    label_linear_damping,
                    rigidbody->linear.damping
                );

                gui->scrubber3
                (
                    layer,
                    gui->getAddressAsInt(rigidbody->linear.velocity),
                    label_angular_velocity,
                    rigidbody->angular.velocity.x,
                    rigidbody->angular.velocity.y,
                    rigidbody->angular.velocity.z
                );

                gui->scrubber
                (
                    layer,
                    gui->getAddressAsInt(rigidbody->angular.damping),
                    label_angular_damping,
                    rigidbody->angular.damping
                );
                gui->pop();
            }
            break;

            case 2:
            {
                ecs->remove< kege::Rigidbody >( entity );
            }
            break;

            default: break;
        }
        gui->pop();

//        gui->push({.layer = layer, .style = &gui->_theme.card});
//        if ( gui->removableHeader(layer, component_id, main_label) )
//        {
//            gui->push({.layer = layer, .style = &gui->_theme.padded_list});
//            ui::Text label_friction
//            {
//                .ptr = "Friction:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//            ui::Text label_restitution
//            {
//                .ptr = "Restitution:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//            ui::Text label_linear_damping
//            {
//                .ptr = "Linear Damping:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//            ui::Text label_angular_damping
//            {
//                .ptr = "Angular Damping:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//            ui::Text label_linear_velocity
//            {
//                .ptr = "Linear Velocity:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//            ui::Text label_angular_velocity
//            {
//                .ptr = "Angular Velocity:",
//                .width = 100,
//                .font_size = 20,
//                .height = 15,
//                .color = 0xFFFFFFFF
//            };
//
//            gui->scrubber
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->friction),
//                label_friction,
//                rigidbody->friction
//            );
//
//            gui->scrubber
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->cor),
//                label_restitution,
//                rigidbody->cor
//            );
//
//            gui->scrubber3
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->linear.velocity),
//                label_linear_velocity,
//                rigidbody->linear.velocity.x,
//                rigidbody->linear.velocity.y,
//                rigidbody->linear.velocity.z
//            );
//
//            gui->scrubber
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->linear.damping),
//                label_linear_damping,
//                rigidbody->linear.damping
//            );
//
//            gui->scrubber3
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->linear.velocity),
//                label_angular_velocity,
//                rigidbody->angular.velocity.x,
//                rigidbody->angular.velocity.y,
//                rigidbody->angular.velocity.z
//            );
//
//            gui->scrubber
//            (
//                layer,
//                gui->getAddressAsInt(rigidbody->angular.damping),
//                label_angular_damping,
//                rigidbody->angular.damping
//            );
//            gui->pop();
//        }
//        gui->pop();
        return false;
    }
}
