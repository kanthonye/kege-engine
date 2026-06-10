//
//  ui-light.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#include "ui-light.hpp"

namespace kege::ui{

//    bool light( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
//    {
//        uint64_t component_id = ecs->getCompId< kege::Light >( entity );
//        kege::Light* light = ecs->get< kege::Light >( entity );
//
//        ui::Text main_label
//        {
//            .ptr = "Light",
//            .width = 64,
//            .font_size = 20,
//            .height = 15,
//            .color = 0xFFFFFFFF
//        };
//
//        ui->push({.layer = layer, .style = &ui->theme()->padded_list});
//
//        ui::Text text;
//        text.font_size = 20;
//        text.height = 15;
//        text.color = 0xFFFFFFFF;
//
//        static std::vector< kege::ui::Text > list;
//        if ( list.empty() )
//        {
//            list.push_back
//            ({
//                .align = ui::AlignText::Left,
//                .ptr = "Point Light",
//                .color = 0xFFFFFFFF,
//                .font_size = 20,
//                .width = 200,
//            });
//            list.push_back
//            ({
//                .align = ui::AlignText::Left,
//                .ptr = "Directional Light",
//                .color = 0xFFFFFFFF,
//                .font_size = 20,
//                .width = 200,
//            });
//            list.push_back
//            ({
//                .align = ui::AlignText::Left,
//                .ptr = "Spot Light",
//                .color = 0xFFFFFFFF,
//                .font_size = 20,
//                .width = 200,
//            });
//        }
//
//        ui->options(uid, layer, list, light->type);
//
//        if ( light->type == Light::POINT )
//        {
//            ui::ID id[3] = {uid[10],uid[11],uid[12]};
//            text.ptr = "Color:";
//            text.width = 100;
////            ui->scrubber3
////            (
////                id,
////                layer,
////                text,
////                light->color.x,
////                light->color.y,
////                light->color.z
////            );
////
////            text.ptr = "Linear Attenuation:";
////            text.width = 100;
////            ui->scrubber( uid[13], layer, text, light->spot_exponent );
////
////            text.ptr = "Quadratic Attenuation:";
////            text.width = 100;
////            ui->scrubber( uid[14], layer, text, light->spot_exponent );
//        }
//        else if ( light->type == Light::DIRECTIONAL )
//        {
//            ui::ID id_dir[3] = {uid[10],uid[11],uid[12]};
//            ui::ID id_col[3] = {uid[13],uid[14],uid[15]};
//
//            text.ptr = "Direction:";
//            text.width = 100;
////            ui->scrubber3
////            (
////                id_dir,
////                layer,
////                text,
////                light->color.x,
////                light->color.y,
////                light->color.z
////            );
//
//            text.ptr = "Color:";
//            text.width = 100;
////            ui->scrubber3
////            (
////                id_col,
////                layer,
////                text,
////                light->color.x,
////                light->color.y,
////                light->color.z
////            );
//        }
//        else if ( light->type == Light::SPOT )
//        {
//            ui::ID id_dir[3] = {uid[10],uid[11],uid[12]};
//            ui::ID id_col[3] = {uid[13],uid[14],uid[15]};
//
//            text.ptr = "Direction:";
//            text.width = 100;
//            drawProperties
//            (
//                ui,
//                uid,
//                layer,
//                light->direction,
//                Vec3Properties
//            );
//
//
//            text.ptr = "Color:";
//            text.width = 100;
//            drawProperties
//            (
//                ui,
//                uid,
//                layer,
//                light->color,
//                Vec3Properties
//            );
//
////            text.ptr = "Spot Exponent:";
////            text.width = 100;
////            ui->scrubber( uid[16], layer, text, light->spot_exponent );
////
////            text.ptr = "Spot Cutoff:";
////            text.width = 100;
////            ui->scrubber( uid[17], layer, text, light->spot_cutoff );
////
////            text.ptr = "Linear Attenuation:";
////            text.width = 100;
////            ui->scrubber( uid[18], layer, text, light->spot_exponent );
////
////            text.ptr = "Quadratic Attenuation:";
////            text.width = 100;
////            ui->scrubber( uid[19], layer, text, light->spot_exponent );
//        }
//        ui->pop();
//        return false;
//    }

}
