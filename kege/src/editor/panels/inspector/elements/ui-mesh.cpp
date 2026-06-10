//
//  ui-mesh.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#include "ui-mesh.hpp"

namespace kege::ui{
//
//    bool mesh( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
//    {
//        uint64_t component_id = ecs->getCompId< kege::Renderable >( entity );
//
//        /*
//            FeatureMask feature;
//
//            uint64_t mesh_handle     = 0;
//            uint64_t material_handle = 0;
//            uint64_t pipeline_handle = 0;
//
//            uint32_t render_layer = 0;
//            uint32_t render_pass = 0;
//
//            bool is_visible         = true;
//            bool is_static          = true;
//
//            char fname[64] = {0};
//            size_t size = 0;
//         */
//        ui::Text text
//        {
//            .ptr = "Renderable",
//            .width = 64,
//            .font_size = 20,
//            .height = 15,
//            .color = 0xFFFFFFFF
//        };
//
//        ui->push({.style = &ui->theme()->padded_list});
//        {
//            static std::vector< kege::ui::Text > list;
//            if (list.empty())
//            {
//                list.resize(2);
//                list[0] = text;
//                list[1] = text;
//                //list[2].text = text;
//
//                list[0].ptr = "Asset";
//                list[0].width = 100;
//
//                list[1].ptr = "Procedural";
//                list[1].width = 150;
//            }
//
//            ui->push({.style = &ui->theme()->column});
//            {
//                text.width = 150;
//                text.ptr = "Geometry:";
//                ui->put({.text = text, .rect = {.width = 55, .height = 20} });
//
//                int mesh_option;
//                text.width = 150;
//                text.ptr = "Source:";
//                ui->push({.style = &ui->theme()->row});
//                ui->put({.text = text, .rect = {.width = 45, .height = 20} });
//                ui->options(uid, layer, list, mesh_option);
//                ui->pop();
//                switch (mesh_option)
//                {
//                    case 0: // Loaded Asset
//                    {
//                        kege::Renderable* comp = ecs->get< kege::Renderable >( entity );
//                        text.ptr = "Target:";
//                        text.width = 55;
//                        ui->push({.style = &ui->theme()->row});
//                        ui->put({.text = text, .rect = {.width = 45, .height = 20} });
//                        if(ui->textField( uid[10], layer, comp->fname, 64, comp->size ))
//                        {
//                            comp->mesh_handle = am->getId< kege::Ref<MeshPrimitive> >(comp->fname);
//                        }
//                        ui->pop();
//                        break;
//                    }
//
//                    case 1: // Procedural
//                    {
////                        static std::vector<Ref<MeshShapeGenerator>> generators;
////                        static std::vector<ListElem> mesh_types;
////                        if (mesh_types.empty())
////                        {
////                            generators.resize(6);
////                            generators[0] = new ShapeSelectGenerator();
////                            generators[1] = new ShapeCubeGenerator();
////                            generators[2] = new ShapeEllipsoidGenerator();
////                            generators[3] = new ShapeCylinderGenerator();
////                            generators[4] = new ShapeConeGenerator();
////                            generators[5] = new ShapePlaneGenerator();
////
////                            mesh_types.resize(6);
////                            mesh_types[0].text = generators[0]->text;
////                            mesh_types[1].text = generators[1]->text;
////                            mesh_types[2].text = generators[2]->text;
////                            mesh_types[3].text = generators[3]->text;
////                            mesh_types[4].text = generators[4]->text;
////                            mesh_types[5].text = generators[5]->text;
////                        }
////
////                        text.ptr = "Target:";
////                        text.width = 55;
////                        ui->push({.style = &ui->theme()->row});
////                        ui->put({.text = text, .rect = {.width = 45, .height = 20} });
////                        ui->options(layer, ui->getAddressAsInt(generators[mesh_option]), mesh_types, mesh_option);
////                        ui->pop();
////
////                        if( mesh_option )
////                        {
////                            generators[mesh_option]->update(am, layer, ui, ecs, entity);
////                        }
//                        break;
//                    }
//                    case 2: // Procedural Dynamic
//                    {
//                        break;
//                    }
//
//                    default: break;
//                }
//            }
//            ui->pop();
//        }
//        ui->pop();
//        return false;
//    }

}
