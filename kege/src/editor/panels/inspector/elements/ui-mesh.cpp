//
//  ui-mesh.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#include "ui-mesh.hpp"

namespace kege::ui{

    bool mesh( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint64_t component_id = ecs->getCompId< kege::Renderable >( entity );

        /*
            FeatureMask feature;

            uint64_t mesh_handle     = 0;
            uint64_t material_handle = 0;
            uint64_t pipeline_handle = 0;

            uint32_t render_layer = 0;
            uint32_t render_pass = 0;

            bool is_visible         = true;
            bool is_static          = true;

            char fname[64] = {0};
            size_t size = 0;
         */
        ui::Text text
        {
            .ptr = "Renderable",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->push({.layer = layer, .style = &gui->_theme.padded_list});
        {
            static std::vector<ListElem> list;
            if (list.empty())
            {
                list.resize(2);
                list[0].text = text;
                list[1].text = text;
                //list[2].text = text;

                list[0].text.ptr = "Asset";
                list[0].text.width = 100;

                list[1].text.ptr = "Procedural";
                list[1].text.width = 150;
            }

            gui->push({.layer = layer, .style = &gui->_theme.column});
            {
                text.width = 150;
                text.ptr = "Geometry:";
                gui->put({.layer = layer, .text = text, .rect = {.width = 55, .height = 20} });

                int mesh_option;
                text.width = 150;
                text.ptr = "Source:";
                gui->push({.layer = layer, .style = &gui->_theme.row});
                gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
                gui->options(layer, component_id, list, mesh_option);
                gui->pop();
                switch (mesh_option)
                {
                    case 0: // Loaded Asset
                    {
                        kege::Renderable* comp = ecs->get< kege::Renderable >( entity );
                        text.ptr = "Target:";
                        text.width = 55;
                        gui->push({.layer = layer, .style = &gui->_theme.row});
                        gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
                        if(gui->textField( layer, component_id, comp->fname, 64, comp->size ))
                        {
                            comp->mesh_handle = am->getId< kege::Ref<MeshPrimitive> >(comp->fname);
                        }
                        gui->pop();
                        break;
                    }

                    case 1: // Procedural
                    {
//                        static std::vector<Ref<MeshShapeGenerator>> generators;
//                        static std::vector<ListElem> mesh_types;
//                        if (mesh_types.empty())
//                        {
//                            generators.resize(6);
//                            generators[0] = new ShapeSelectGenerator();
//                            generators[1] = new ShapeCubeGenerator();
//                            generators[2] = new ShapeEllipsoidGenerator();
//                            generators[3] = new ShapeCylinderGenerator();
//                            generators[4] = new ShapeConeGenerator();
//                            generators[5] = new ShapePlaneGenerator();
//
//                            mesh_types.resize(6);
//                            mesh_types[0].text = generators[0]->text;
//                            mesh_types[1].text = generators[1]->text;
//                            mesh_types[2].text = generators[2]->text;
//                            mesh_types[3].text = generators[3]->text;
//                            mesh_types[4].text = generators[4]->text;
//                            mesh_types[5].text = generators[5]->text;
//                        }
//
//                        text.ptr = "Target:";
//                        text.width = 55;
//                        gui->push({.layer = layer, .style = &gui->_theme.row});
//                        gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
//                        gui->options(layer, gui->getAddressAsInt(generators[mesh_option]), mesh_types, mesh_option);
//                        gui->pop();
//
//                        if( mesh_option )
//                        {
//                            generators[mesh_option]->update(am, layer, gui, ecs, entity);
//                        }
                        break;
                    }
                    case 2: // Procedural Dynamic
                    {
                        break;
                    }

                    default: break;
                }
            }
            gui->pop();
        }
        gui->pop();
        return false;
    }

}
