//
//  ui-camera.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-camera.hpp"

namespace kege::ui
{

    bool camera( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        kege::Camera* camera = ecs->get< kege::Camera >( entity );
        uint64_t component_id = ecs->getCompId< kege::Camera >( entity );

        ui::Text main_label
        {
            .ptr = "Camera",
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
                if ( camera->projection->type == Projection::PERSPECTIVE )
                {
                    Perspective* projection = (Perspective*) camera->projection.ref();
                    ui::Text text;
                    text.ptr = "Field Of View";
                    text.width = 100;
                    if( gui->sliderUI(layer, text, gui->getAddressAsInt(projection->fov), &projection->fov, 10.0, 160.0)) camera->modified = true;

                    ui::Text label_znear{ .ptr = "Near Clip:", .width = 60, .font_size = 20, .height = 15, .color = 0xFFFFFFFF };
                    if( gui->scrubber(layer, gui->getAddressAsInt(projection->znear), label_znear, projection->znear) ) camera->modified = true;

                    ui::Text label_zfar{ .ptr = "Far Clip:", .width = 60, .font_size = 20, .height = 15, .color = 0xFFFFFFFF };
                    if( gui->scrubber(layer, gui->getAddressAsInt(projection->zfar), label_zfar, projection->zfar) ) camera->modified = true;
                }
                else if ( camera->projection->type == Projection::ORTHOGRAPHIC )
                {
                    Orthographic* projection = (Orthographic*) camera->projection.ref();
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->left), projection->left)) camera->modified = true;
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->right), projection->right)) camera->modified = true;
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->above), projection->above)) camera->modified = true;
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->below), projection->below)) camera->modified = true;
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->znear), projection->znear)) camera->modified = true;
                    if( gui->numeric(layer, gui->getAddressAsInt(projection->zfar), projection->zfar)) camera->modified = true;
                }
                gui->pop();
            }
            break;

            case 2:
            {
                ecs->remove< kege::Camera >( entity );
            }
            break;

            default: break;
        }
        gui->pop();
        return false;
    }

}
