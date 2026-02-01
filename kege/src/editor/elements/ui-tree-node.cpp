//
//  tree-node.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "ui-tree-node.hpp"



namespace kege::ui{

    struct Renderable
    {
        int mesh_type;
        int mesh_source_id;
    };


    typedef void (*generateMeshFnPtr)(kege::GUI* gui);

    struct MeshShapeGenerator : public kege::RefCounter
    {
        enum GenState{EDITING, GENERATE, COMPLETE};
        virtual void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
        {
            gui->push({.style = &gui->_theme.row, .single_click = ui::ClickTrigger::Continuous});
            gui->put({.text = text, .rect = {.width = 100, .height = 20} });
            gui->put({.style = &gui->_theme.x_seperator});
            gui->put({.color = 0x00FF00FF, .rect = {.width = 10, .height = 10} });
            gui->put({.color = 0xFF0000FF, .rect = {.width = 10, .height = 10} });
            gui->pop();
        }
        MeshShapeGenerator(){}

        ui::Text text;
        GenState gen_state;
    };

    struct ShapeSelectGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
        {}
        ShapeSelectGenerator()
        {
        }
    };
    struct ShapeCubeGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
        {
            kege::Renderable* comp = ecs->get< kege::Renderable >( entity );

            if ( gen_state == EDITING )
            {
                gui->push({.layer = layer, .style = &gui->_theme.column});
                gui->scrubber(layer, gui->getAddressAsInt(lengths.x), text_width,  lengths.x);
                gui->scrubber(layer, gui->getAddressAsInt(lengths.y), text_height, lengths.y);
                gui->scrubber(layer, gui->getAddressAsInt(lengths.z), text_depth,  lengths.z);
                if( gui->button(layer, butn_id, text_butn) )
                {
                    gen_state = GENERATE;
                }
                gui->pop();
            }

            if ( gen_state == GENERATE )
            {
                gen_state = COMPLETE;
                kege::Ref<kege::MeshPrimitive> primative = new CuboidMesh(center, lengths);
                am->add< kege::Ref<kege::MeshPrimitive> >("cube", primative);
                uint32_t instance_count = 1;
                uint32_t first_instance = 0;
                uint32_t first_index    = 0;
                uint32_t index_count    = 36;
                int32_t  material_index = 1;
                kege::Ref<kege::Mesh> mesh = new kege::Mesh
                (
                    primative,
                    instance_count,
                    first_instance,
                    first_index,
                    index_count,
                    material_index
                );
                comp->mesh_handle = am->add< kege::Ref<kege::Mesh> >("cube", mesh);
            }
        }
        
        ShapeCubeGenerator()
        {
            lengths = {0.f, 0.f, 0.f};
            center = {0.f, 0.f, 0.f};

            text.ptr = "Cuboid";
            text.width = 100;
            text.font_size = 20;
            text.height = 15;
            text.color = 0xFFFFFFFF;

            text_butn.ptr = "Commit:";
            text_butn.width = 40;

            text_width = text;
            text_height = text;
            text_depth = text;

            text_width.ptr = "Width:";
            text_width.width = 35;
            text_height.ptr = "Height:";
            text_height.width = 40;
            text_depth.ptr = "Depth:";
            text_depth.width = 35;
            gen_state = EDITING;
        }

        ui::Text text_butn;
        ui::Text text_width;
        ui::Text text_height;
        ui::Text text_depth;
        kege::vec3 lengths;
        kege::vec3 center;
        ui::UID butn_id;
    };
    struct ShapeEllipsoidGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
        ShapeEllipsoidGenerator()
        {
            text.ptr = "Ellipsoid";
            text.width = 100;
            text.font_size = 20;
            text.height = 15;
            text.color = 0xFFFFFFFF;
        }
    };
    struct ShapeCylinderGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
        ShapeCylinderGenerator()
        {
            text.ptr = "Cylinder";
            text.width = 100;
            text.font_size = 20;
            text.height = 15;
            text.color = 0xFFFFFFFF;
        }
    };
    struct ShapeConeGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
        ShapeConeGenerator()
        {
            text.ptr = "Cone";
            text.width = 100;
            text.font_size = 20;
            text.height = 15;
            text.color = 0xFFFFFFFF;
        }
    };
    struct ShapePlaneGenerator : public ui::MeshShapeGenerator
    {
        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
        ShapePlaneGenerator()
        {
            text.ptr = "Plane";
            text.width = 100;
            text.font_size = 20;
            text.height = 15;
            text.color = 0xFFFFFFFF;
        }
    };

    bool mesh( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint64_t component_id = ecs->getCompId< kege::Renderable >( entity );

        ui::Text text
        {
            .ptr = "Mesh",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->push({ .layer = layer, .style = &gui->_theme.card });
        switch (gui->removableHeader(layer, component_id, text))
        {
            case 1:
            {
                gui->push({.layer = layer, .style = &gui->_theme.padded_list});
                {
                    static std::vector<ListElem> list;
                    if (list.empty())
                    {
                        list.resize(3);
                        list[0].text = text;
                        list[1].text = text;
                        list[2].text = text;

                        list[0].text.ptr = "Loaded Asset";
                        list[0].text.width = 100;

                        list[1].text.ptr = "Procedural Static";
                        list[1].text.width = 150;

                        list[2].text.ptr = "Procedural Dynamic";
                        list[2].text.width = 150;
                    }

                    int selection;
                    text.ptr = "Source:";
                    gui->push({.layer = layer, .style = &gui->_theme.row});
                    gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
                    gui->options(layer, component_id, list, selection);
                    gui->pop();

                    switch (selection)
                    {
                        case 0: // Loaded Asset
                        {
                            kege::Renderable* comp = ecs->get< kege::Renderable >( entity );
                            text.ptr = "Filename:";
                            text.width = 55;
                            gui->push({.layer = layer, .style = &gui->_theme.column});
                            gui->put({.layer = layer, .text = text, .rect = {.width = 50, .height = 20} });
                            if(gui->textField( layer, component_id, comp->fname, 64, comp->size ))
                            {
                                // load asset
                            }
                            gui->pop();
                            break;
                        }
                        case 1: // Procedural Static
                        {
                            static std::vector<Ref<MeshShapeGenerator>> generators;
                            static std::vector<ListElem> mesh_types;
                            if (mesh_types.empty())
                            {
                                generators.resize(6);
                                generators[0] = new ShapeSelectGenerator();
                                generators[1] = new ShapeCubeGenerator();
                                generators[2] = new ShapeEllipsoidGenerator();
                                generators[3] = new ShapeCylinderGenerator();
                                generators[4] = new ShapeConeGenerator();
                                generators[5] = new ShapePlaneGenerator();

                                mesh_types.resize(6);
                                mesh_types[0].text = generators[0]->text;
                                mesh_types[1].text = generators[1]->text;
                                mesh_types[2].text = generators[2]->text;
                                mesh_types[3].text = generators[3]->text;
                                mesh_types[4].text = generators[4]->text;
                                mesh_types[5].text = generators[5]->text;
                            }

                            text.ptr = "Source:";
                            text.width = 55;
                            gui->push({.layer = layer, .style = &gui->_theme.row});
                            gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
                            gui->options(layer, gui->getAddressAsInt(generators[selection]), mesh_types, selection);
                            gui->pop();

                            if( selection )
                            {
                                generators[selection]->update(am, layer, gui, ecs, entity);
                            }
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
            break;

            case 2:
            {
                ecs->remove< kege::ref::Mesh >( entity );
            }
            break;

            default: break;
        }
        gui->pop();

        return false;
    }

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

        gui->push({ .layer = layer, .style = &gui->_theme.card });

        switch (gui->removableHeader(layer, component_id, main_label))
        {
            case 1:
            {
                gui->push({.layer = layer, .style = &gui->_theme.padded_list});

                ui::Text text;
                text.font_size = 20;
                text.height = 15;
                text.color = 0xFFFFFFFF;

                uint64_t id = gui->getAddressAsInt(light);
                std::vector<ListElem> &list = gui->getListState(id);
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

                gui->options(layer, id, list, light->type);

                if ( light->type == Light::POINT )
                {
                    text.ptr = "Color:";
                    text.width = 100;
                    gui->scrubber3
                    (
                        layer,
                        gui->getAddressAsInt(light->color),
                        text,
                        light->color.x,
                        light->color.y,
                        light->color.z
                    );

                    text.ptr = "Linear Attenuation:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->linear_attenuation), text, light->spot_exponent );

                    text.ptr = "Quadratic Attenuation:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->quadratic_attenuation), text, light->spot_exponent );
                }
                else if ( light->type == Light::DIRECTIONAL )
                {
                    text.ptr = "Direction:";
                    text.width = 100;
                    gui->scrubber3
                    (
                        layer,
                        gui->getAddressAsInt(light->color),
                        text,
                        light->color.x,
                        light->color.y,
                        light->color.z
                    );

                    text.ptr = "Color:";
                    text.width = 100;
                    gui->scrubber3
                    (
                        layer,
                        gui->getAddressAsInt(light->color),
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
                        layer,
                        gui->getAddressAsInt(light->color),
                        text,
                        light->color.x,
                        light->color.y,
                        light->color.z
                    );

                    text.ptr = "Color:";
                    text.width = 100;
                    gui->scrubber3
                    (
                        layer,
                        gui->getAddressAsInt(light->color),
                        text,
                        light->color.x,
                        light->color.y,
                        light->color.z
                    );

                    text.ptr = "Spot Exponent:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->color), text, light->spot_exponent );

                    text.ptr = "Spot Cutoff:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->color), text, light->spot_cutoff );

                    text.ptr = "Linear Attenuation:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->linear_attenuation), text, light->spot_exponent );

                    text.ptr = "Quadratic Attenuation:";
                    text.width = 100;
                    gui->scrubber( layer, gui->getAddressAsInt(light->quadratic_attenuation), text, light->spot_exponent );
                }
                gui->pop();
            }
            break;

            case 2:
            {
                ecs->remove< kege::Light >( entity );
            }
            break;

            default: break;
        }
        gui->pop();

        return false;
    }



    bool particleEffect( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint64_t component_id = ecs->getCompId< kege::ParticleEffect >( entity );
        kege::ParticleEffect* effect = ecs->get< kege::ParticleEffect >( entity );

        ui::Text text
        {
            .ptr = "Particle Effect",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        gui->push({ .layer = layer, .style = &gui->_theme.card });

        switch (gui->removableHeader(layer, component_id, text))
        {
            case 1:
            {
                static std::vector<ListElem> list;
                if (list.empty())
                {
                    list.resize(12);

                    list[0].text.ptr = "Circular Area";
                    list[0].text.width = 100;

                    list[1].text.ptr = "Circular Line";
                    list[1].text.width = 100;

                    list[2].text.ptr = "Cone";
                    list[2].text.width = 150;

                    list[3].text.ptr = "Cube";
                    list[3].text.width = 150;

                    list[4].text.ptr = "Cylindrical Surface";
                    list[4].text.width = 150;

                    list[5].text.ptr = "Cylindrical Area";
                    list[5].text.width = 150;

                    list[6].text.ptr = "Line";
                    list[6].text.width = 150;

                    list[7].text.ptr = "Plane";
                    list[7].text.width = 150;

                    list[8].text.ptr = "Pyrmid";
                    list[8].text.width = 150;

                    list[9].text.ptr = "Spherical Surface";
                    list[9].text.width = 150;

                    list[10].text.ptr = "Spherical Area";
                    list[10].text.width = 150;

                    list[11].text.ptr = "Triangle";
                    list[11].text.width = 150;
                }

                int selection;
                text.ptr = "Emitters:";
                gui->push({.layer = layer, .style = &gui->_theme.row});
                gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
                gui->options(layer, component_id, list, selection);
                gui->pop();

                //float emissions_per_second, bool burst
                //max particle quantity
                //rate_of_deterioration
                gui->push({.layer = layer, .style = &gui->_theme.padded_list});
                gui->pop();
            }
            break;

            case 2:
            {
                ecs->remove< kege::Light >( entity );
            }
            break;

            default: break;
        }
        gui->pop();

        return false;
    }
}
