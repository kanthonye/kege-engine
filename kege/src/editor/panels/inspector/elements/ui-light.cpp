//
//  ui-light.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#include "ui-light.hpp"

namespace kege::ui{

    bool light( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint64_t component_id = ecs->getCompId< kege::Light >( entity );
        kege::Light* light = ecs->get< kege::Light >( entity );

        ui::Text main_label
        {
            .ptr = "Light",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->push({.layer = layer, .style = &gui->_theme.padded_list});

        ui::Text text;
        text.font_size = 20;
        text.height = 15;
        text.color = 0xFFFFFFFF;

        UserId user_id = gui->getAddressAsInt(light);
        std::vector<ListElem> &list = gui->getListState(user_id);
        if ( list.empty() )
        {
            list.push_back
            ({
                .text = ui::Text
                {
                    .align = ui::AlignText::Left,
                    .ptr = "Point Light",
                    .color = 0xFFFFFFFF,
                    .font_size = 20,
                    .width = 200,
                }
            });
            list.push_back
            ({
                .text = ui::Text
                {
                    .align = ui::AlignText::Left,
                    .ptr = "Directional Light",
                    .color = 0xFFFFFFFF,
                    .font_size = 20,
                    .width = 200,
                }
            });
            list.push_back
            ({
                .text = ui::Text
                {
                    .align = ui::AlignText::Left,
                    .ptr = "Spot Light",
                    .color = 0xFFFFFFFF,
                    .font_size = 20,
                    .width = 200,
                }
            });
        }

        gui->options(UI_BASE_ID(), layer, list, light->type);

        if ( light->type == Light::POINT )
        {
            text.ptr = "Color:";
            text.width = 100;
            gui->scrubber3
            (
                UI_BASE_ID(),
                layer,
                text,
                light->color.x,
                light->color.y,
                light->color.z
            );

            text.ptr = "Linear Attenuation:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_exponent );

            text.ptr = "Quadratic Attenuation:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_exponent );
        }
        else if ( light->type == Light::DIRECTIONAL )
        {
            text.ptr = "Direction:";
            text.width = 100;
            gui->scrubber3
            (
                UI_BASE_ID(),
                layer,
                text,
                light->color.x,
                light->color.y,
                light->color.z
            );

            text.ptr = "Color:";
            text.width = 100;
            gui->scrubber3
            (
                UI_BASE_ID(),
                layer,
                text,
                light->color.x,
                light->color.y,
                light->color.z
            );
        }
        else if ( light->type == Light::SPOT )
        {
            text.ptr = "Direction:";
            text.width = 100;
            gui->scrubber3
            (
                UI_BASE_ID(),
                layer,
                text,
                light->color.x,
                light->color.y,
                light->color.z
            );

            text.ptr = "Color:";
            text.width = 100;
            gui->scrubber3
            (
                UI_BASE_ID(),
                layer,
                text,
                light->color.x,
                light->color.y,
                light->color.z
            );

            text.ptr = "Spot Exponent:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_exponent );

            text.ptr = "Spot Cutoff:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_cutoff );

            text.ptr = "Linear Attenuation:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_exponent );

            text.ptr = "Quadratic Attenuation:";
            text.width = 100;
            gui->scrubber( UI_BASE_ID(), layer, text, light->spot_exponent );
        }
        gui->pop();
        return false;
    }

}
