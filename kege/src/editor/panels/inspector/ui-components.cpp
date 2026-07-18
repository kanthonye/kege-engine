//
//  ui-components.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/29/26.
//

#include "ui-components.hpp"

namespace kege::ui{
    PropertyMeta k;
    const kege::ui::Property<kege::Perspective> PerspectiveProperties[3] =
    {
        UI_PROP
        (
            kege::Perspective, fov, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = { kege::ui::UIHint::DragNum, false, 10.0, 160.0 },
                .text = { .ptr = "FOV:", .x = 0.f, .y = 0.f, .width = 90.0, .height = 20.0 }
            }
        ),
        UI_PROP
        (
            kege::Perspective, znear, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 0.001, 999'999'999.0},
                .text = { "Near:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::Perspective, zfar, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 0.001, 999'999'999.0},
                .text = {"Far:", 0.f, 0.f, 90.f, 20.f },
            }
        ),
    };

    const kege::ui::Property<kege::Orthographic> OrthographicProperties[] =
    {
        UI_PROP
        (
            kege::Orthographic, left, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 10.0, 160.0},
                .text = {"Left:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::Orthographic, right, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 10.0, 160.0},
                .text = {"Right:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::Orthographic, above, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 10.0, 160.0},
                .text = {"Top:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::Orthographic, below, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 10.0, 160.0},
                .text = {"Bottom:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::Orthographic, znear, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 0.001, 999'999'999.0},
                .text = {"Near:", 0.f, 0.f, 90.0f, 20.0f },
            }
        ),
        UI_PROP
        (
            kege::Orthographic, zfar, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, false, 0.001, 999'999'999.0},
                .text = {"Far:", 0.f, 0.f, 90.f, 20.f },
            }
        ),
    };

    const kege::ui::Property<kege::Rigidbody> RigidbodyProperties[6] =
    {
        UI_PROP
        (
            kege::Rigidbody, linear.velocity, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {"Velocity:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Rigidbody, linear.invmass, kege::ui::PropertyMeta{
                .type = PropertyType::Double,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 3'999'999'999 },
                .text = {"Linear InvMass:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Rigidbody, linear.damping, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Linear Damping:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Rigidbody, cor, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 3'999'999'999 },
                .text = {"COR:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Rigidbody, friction, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Friction:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Rigidbody, immovable, kege::ui::PropertyMeta{
                .type = PropertyType::Bool,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Immovable:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
    };

    const kege::ui::Property<kege::Transform> TransformProperties[3] =
    {
        UI_PROP
        (
            kege::Transform, position, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, false},
                .text = {"Position:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Transform, scale, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 3'999'999'999 },
                .text = {"Scale:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
        UI_PROP
        (
            kege::Transform, orientation, kege::ui::PropertyMeta{
                .type = PropertyType::Quat,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Orientation:", 0.f, 0.f, 100.f, 20.f },
            }
        ),
    };

    const kege::ui::Property<kege::DirectionalLight> DirectionalLightProperties[3] =
    {
        UI_PROP
        (
            kege::DirectionalLight, color, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Color:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::DirectionalLight, intensity, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Intensity:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::DirectionalLight, direction, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Direction:", 0.f, 0.f, 90.0, 20.0 },
            }
        )
    };


    const kege::ui::Property<kege::PointLight> PointLightProperties[5] =
    {
        UI_PROP
        (
            kege::PointLight, color, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Color:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::PointLight, intensity, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Intensity:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::PointLight, position, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Position:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::PointLight, linear_attenuation, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Linear Attenuation:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::PointLight, quadratic_attenuation, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Quadratic Attenuation:", 0.f, 0.f, 90.0, 20.0 },
            }
        )
    };


    const kege::ui::Property<kege::SpotLight> SpotLightProperties[8] =
    {
        UI_PROP
        (
            kege::SpotLight, color, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Color:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, intensity, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Intensity:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, position, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Position:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, direction, kege::ui::PropertyMeta{
                .type = PropertyType::Vec3,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Direction:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, spot_exponent, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Spot Exponent:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, spot_cutoff, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Spot Cutoff:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, linear_attenuation, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Linear Attenuation:", 0.f, 0.f, 90.0, 20.0 },
            }
        ),
        UI_PROP
        (
            kege::SpotLight, quadratic_attenuation, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0},
                .text = {"Quadratic Attenuation:", 0.f, 0.f, 90.0, 20.0 },
            }
        )
    };


    bool perspectiveCamera( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        kege::Perspective* projection = ecs->get< kege::Perspective >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *projection, PerspectiveProperties);
        ui->pop();
        return false;
    }

    bool orthographicCamera( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        kege::Orthographic* projection = ecs->get< kege::Orthographic >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *projection, OrthographicProperties);
        ui->pop();
        return false;
    }

    bool rigidbody( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        kege::Rigidbody* rigidbody = ecs->get< kege::Rigidbody >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *rigidbody, RigidbodyProperties);
        ui->pop();
        return false;
    }

    bool transform( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        Transform* transform = ecs->get< kege::Transform >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *transform, TransformProperties);
        ui->pop();
        return false;
    }

    bool directionalLight( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        DirectionalLight* light = ecs->get< kege::DirectionalLight >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *light, DirectionalLightProperties);
        ui->pop();
        return false;
    }

    bool pointLight( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        PointLight* light = ecs->get< kege::PointLight >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *light, PointLightProperties);
        ui->pop();
        return false;
    }

    bool spotLight( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint32_t id_offset = 0;
        SpotLight* light = ecs->get< kege::SpotLight >( entity );
        ui->push({.style = &ui->theme()->padded_list});
        drawProperties(ui, uid, id_offset, *light, SpotLightProperties);
        ui->pop();
        return false;
    }

    bool renderable( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        //Renderable* component = ecs->get< kege::Renderable >( entity );
        ui::Text text
        {
            .width = 64,
            .height = 15,
            .font_size = 20,
            .color = 0xFFFFFFFF
        };

        ui->push({.style = &ui->theme()->padded_list});
        {
            text.width = 150;
            text.ptr = "Geometry:";
            ui->put({.rect = {.width = 55, .height = 20}, .text = text});

            //text.ptr = component->fname;
            //ui->put({.rect = {.width = 55, .height = 20}, .text = text});
        }
        ui->pop();
        return false;
    }

    bool particleEffect( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
    {
        ui::Text text
        {
            .ptr = "Particle Effect",
            .width = 64,
            .height = 15,
            .font_size = 20,
            .color = 0xFFFFFFFF
        };

        static std::vector< kege::ui::Text > list;
        if (list.empty())
        {
            list.resize(12);

            list[0].ptr = "Circular Area";
            list[0].width = 100;

            list[1].ptr = "Circular Line";
            list[1].width = 100;

            list[2].ptr = "Cone";
            list[2].width = 150;

            list[3].ptr = "Cube";
            list[3].width = 150;

            list[4].ptr = "Cylindrical Surface";
            list[4].width = 150;

            list[5].ptr = "Cylindrical Area";
            list[5].width = 150;

            list[6].ptr = "Line";
            list[6].width = 150;

            list[7].ptr = "Plane";
            list[7].width = 150;

            list[8].ptr = "Pyrmid";
            list[8].width = 150;

            list[9].ptr = "Spherical Surface";
            list[9].width = 150;

            list[10].ptr = "Spherical Area";
            list[10].width = 150;

            list[11].ptr = "Triangle";
            list[11].width = 150;
        }

        int selection;
        text.ptr = "Emitters:";
        ui->push({.style = &ui->theme()->row});
        ui->put({.rect = {.width = 45, .height = 20}, .text = text});
        ui->options(uid, list, selection);
        ui->pop();

        //float emissions_per_second, bool burst
        //max particle quantity
        //rate_of_deterioration
        ui->push({.style = &ui->theme()->padded_list});
        ui->pop();

        return false;
    }
//    bool particleEffect( const ui::UID& uid, kege::AssetManager* am, kege::UI* ui, kege::ECS* ecs, ecs::Entity& entity )
//    {
//        uint32_t id_offset = 0;
//        Transform* transform = ecs->get< kege::Transform >( entity );
//        ui->push({.style = &ui->theme()->padded_list});
//        drawProperties(ui, uid, id_offset, *transform, TransformProperties);
//        ui->pop();
//        return false;
//    }


}
