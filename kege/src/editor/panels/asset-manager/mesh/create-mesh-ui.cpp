//
//  create-mesh-ui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/14/26.
//

#include "../../../editor-layer.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "create-mesh-ui.hpp"
//namespace kege::ui{
//
//    typedef void (*generateMeshFnPtr)(kege::GUI* gui);
//
//    struct MeshShapeGenerator : public kege::RefCounter
//    {
//        enum GenState{EDITING, GENERATE, COMPLETE};
//        virtual void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
//        {
//            gui->push({.style = &gui->theme().row, .single_click = ui::ClickTrigger::Continuous});
//            gui->put({.text = text, .rect = {.width = 100, .height = 20} });
//            gui->put({.style = &gui->theme().x_seperator});
//            gui->put({.color = 0x00FF00FF, .rect = {.width = 10, .height = 10} });
//            gui->put({.color = 0xFF0000FF, .rect = {.width = 10, .height = 10} });
//            gui->pop();
//        }
//        MeshShapeGenerator(){}
//
//        ui::Text text;
//        GenState gen_state;
//    };
//
//    struct ShapeSelectGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
//        {}
//        ShapeSelectGenerator()
//        {
//        }
//    };
//
//    struct ShapeCubeGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity)
//        {
//            kege::Renderable* comp = ecs->get< kege::Renderable >( entity );
//
//            if ( gen_state == EDITING )
//            {
//                gui->push({.layer = layer, .style = &gui->theme().column});
//                gui->scrubber(layer, gui->getAddressAsInt(lengths.x), text_width,  lengths.x);
//                gui->scrubber(layer, gui->getAddressAsInt(lengths.y), text_height, lengths.y);
//                gui->scrubber(layer, gui->getAddressAsInt(lengths.z), text_depth,  lengths.z);
//                if( gui->button(layer, butn_id, text_butn) )
//                {
//                    gen_state = GENERATE;
//                }
//                gui->pop();
//            }
//
//            if ( gen_state == GENERATE )
//            {
//                gen_state = COMPLETE;
//                kege::Ref<kege::MeshPrimitive> primative = new CuboidMesh(center, lengths);
//                am->add< kege::Ref<kege::MeshPrimitive> >("cube", primative);
//                uint32_t instance_count = 1;
//                uint32_t first_instance = 0;
//                uint32_t first_index    = 0;
//                uint32_t index_count    = 36;
//                int32_t  material_index = 1;
//                kege::Ref<kege::Mesh> mesh = new kege::Mesh
//                (
//                    primative,
//                    instance_count,
//                    first_instance,
//                    first_index,
//                    index_count,
//                    material_index
//                );
//                comp->mesh_handle = am->add< kege::Ref<kege::Mesh> >("cube", mesh);
//            }
//        }
//
//        ShapeCubeGenerator()
//        {
//            lengths = {0.f, 0.f, 0.f};
//            center = {0.f, 0.f, 0.f};
//
//            text.ptr = "Cuboid";
//            text.width = 100;
//            text.font_size = 20;
//            text.height = 15;
//            text.color = 0xFFFFFFFF;
//
//            text_butn.ptr = "Regenerate:";
//            text_butn.width = 40;
//
//            text_width = text;
//            text_height = text;
//            text_depth = text;
//
//            text_width.ptr = "Width:";
//            text_width.width = 35;
//            text_height.ptr = "Height:";
//            text_height.width = 40;
//            text_depth.ptr = "Depth:";
//            text_depth.width = 35;
//            gen_state = EDITING;
//        }
//
//        ui::Text text_butn;
//        ui::Text text_width;
//        ui::Text text_height;
//        ui::Text text_depth;
//        kege::vec3 lengths;
//        kege::vec3 center;
//        ui::WidgetHandle butn_id;
//    };
//
//    struct ShapeEllipsoidGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
//        ShapeEllipsoidGenerator()
//        {
//            text.ptr = "Ellipsoid";
//            text.width = 100;
//            text.font_size = 20;
//            text.height = 15;
//            text.color = 0xFFFFFFFF;
//        }
//    };
//
//    struct ShapeCylinderGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
//        ShapeCylinderGenerator()
//        {
//            text.ptr = "Cylinder";
//            text.width = 100;
//            text.font_size = 20;
//            text.height = 15;
//            text.color = 0xFFFFFFFF;
//        }
//    };
//
//    struct ShapeConeGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
//        ShapeConeGenerator()
//        {
//            text.ptr = "Cone";
//            text.width = 100;
//            text.font_size = 20;
//            text.height = 15;
//            text.color = 0xFFFFFFFF;
//        }
//    };
//
//    struct ShapePlaneGenerator : public ui::MeshShapeGenerator
//    {
//        void update(kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity){}
//        ShapePlaneGenerator()
//        {
//            text.ptr = "Plane";
//            text.width = 100;
//            text.font_size = 20;
//            text.height = 15;
//            text.color = 0xFFFFFFFF;
//        }
//    };
//
//}
//

