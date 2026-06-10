//
//  scene-loader.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#include "scene-loader.hpp"

namespace kege{
    typedef int32_t uint32;

    SceneLoader::ParticleBehaviorFunctMap SceneLoader::_particle_behavior_creators;
    SceneLoader::ParticleEmitterFunctMap SceneLoader::_particle_emitter_creators;
    SceneLoader::ColliderFunctMap SceneLoader::_collider_funct_map;
    SceneLoader::ResourceParserFunctMap SceneLoader::_resource_parsers;

    SceneLoader::EntityComponentParsers SceneLoader::_entity_component_parsers;


    ecs::Entity SceneLoader::parseEntity( kege::ECS& ecs, kege::Json& entities, kege::Json& components, int entity_index )
    {
        kege::Json entity_json = entities[ entity_index ];

        const char* entity_name      = entity_json[ "name" ].toStr();
        kege::Json entity_components = entity_json[ "components" ];
        kege::Json entity_children   = entity_json[ "children" ];

        ecs::Entity entity = ecs.create();
        *ecs.add< Tag >( entity ) = entity_name;

        for (int comp_index = 0; comp_index < entity_components.count(); ++comp_index )
        {
            kege::Json component = components[ comp_index ];
            //int comp_type = component[ "type" ].toInt();
            //kege::Json component_type = component_types[ comp_type ];
            std::string type = component[ "type" ].toStr();

            auto itr_funct = _entity_component_parsers.find( type );
            if( itr_funct != _entity_component_parsers.end() )
            {
                itr_funct->second( ecs, entity, component[ "data" ] );
            }
        }

        for (int child_index = 0; child_index < entity_children.count(); ++child_index )
        {
            int entity_index = entity_children[ child_index ].toInt();
            ecs::Entity child = parseEntity( ecs, entities, components, entity_index );
            ecs.attach( entity, child );
        }
        return entity;
    }

    kege::Ref< kege::Scene > SceneLoader::load( kege::ECS& ecs, kege::RenderGraph* rg, kege::AssetManager* am, const std::string& filename )
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            kege::Log::error << "Failed to open scene file: " << filename;
            return {};
        }

        kege::Ref< kege::Scene > scene = new kege::Scene( "", *ecs.getEntityManager(), am );

        Params params;
//        params.scene = scene.ref();
//        params.assets = &scene->getResourceManager();


        int scene_index = json[ "scene" ].toInt();
        kege::Json main_scene = json[ "scenes" ][ scene_index ];

        kege::Json main_scene_name = main_scene[ "name" ];
        kege::Json main_scene_entities = main_scene[ "entities" ];

        kege::Json entities = json[ "entities" ];
        kege::Json components = json[ "components" ];
        //kege::Json component_types = json[ "ComponentTypes" ];

        for (int i = 0; i < main_scene_entities.count(); ++i )
        {
            int entity_index = main_scene_entities[i].toInt();
            ecs::Entity entity = parseEntity( ecs, entities, components, entity_index );
            scene->insert( entity );
        }

        return scene;
    }

//    void SceneLoader::getAssetManager( Params* params, ecs::Entity& entity, Json json )
//    {
//        for (int i = 0; i < json.count(); ++i )
//        {
//            Json data = json[ i ][ "data" ];
//            params->id = json[ i ][ "id" ].value();
//            params->type = data[ "type" ].value();
//            ResourceParserFunctMap::iterator funct = _resource_parsers.find( params->type );
//            if ( funct != _resource_parsers.end() )
//            {
//                funct->second( params, nullptr, data[ params->type ] );
//            }
//        }
//    }
//
//    void SceneLoader::atmosphere( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Atmosphere atmosphere = kege::Atmosphere
//        {
//            .inner_radius = json[ "inner_radius" ].getDouble(),
//            .outer_radius = json[ "outer_radius" ].getDouble(),
//
//            .rayleigh.scattering_coefficient = toVec3( json[ "rayleigh" ][ "scattering_coefficient" ] ),
//            .rayleigh.scale_height = json[ "rayleigh" ][ "scale_height" ].toFloat(),
//
//            .mie.scattering_coefficient = toVec3( json[ "mie" ][ "scattering_coefficient" ] ),
//            .mie.scale_height = json[ "mie" ][ "scale_height" ].toFloat(),
//            .mie.anisotropy = json[ "mie" ][ "anisotropy" ].toFloat(),
//
//            .absorption.scattering_coefficient = toVec3( json[ "absorption" ][ "absorption_coefficient" ] ),
//            .absorption.ozone_center_height = json[ "absorption" ][ "ozone_center_height" ].toFloat(),
//
//            .rendering.ground_color = toVec3( json[ "rendering" ][ "ground_color" ] ),
//            .rendering.sky_color = toVec3( json[ "rendering" ][ "sky_color" ] ),
//            .rendering.enable_multiple_scattering = json[ "rendering" ][ "enable_multiple_scattering" ].toBool(),
//            .rendering.samples = json[ "rendering" ][ "samples" ].toInt()
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Atmosphere >( atmosphere );
//        }
//        else
//        {
//            params->assets->add< kege::Atmosphere >( params->id, atmosphere );
//        }
//    }

    float invMass( float mass )
    {
        return ( mass != 0 ) ? (1.0 / mass) : 0.0;
    }
//
//    void SceneLoader::rigidbody( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Rigidbody rigidbody = kege::Rigidbody
//        {
//            .linear = LinearMotion
//            {
//                .forces = {},
//                .acceleration = {},
//                .velocity = toVec3( json[ "linear" ][ "velocity" ] ),
//                .invmass = invMass( json[ "linear" ][ "mass" ].toFloat() ),
//                .damping = json[ "linear" ][ "damping" ].toFloat()
//            },
//            .angular = AngularMotion
//            {
//                .torques = {},
//                .rotation = {},
//                .velocity = toVec3( json[ "angular" ][ "velocity" ] ),
//                .inertia_inverse = mat33( json[ "angular" ][ "inertia" ].toFloat() ),
//                .damping = json[ "angular" ][ "damping" ].toFloat()
//            },
//            .center = toVec3( json[ "center" ] ),
//            .orientation = toQuat( json[ "orientation" ] ),
//            .anti_gravity = json[ "anti_gravity" ].toBool(),
//            .immovable = json[ "immovable" ].toBool(),
//            .sleepable = json[ "sleepable" ].toBool(),
//            .up = vec3(0.f, 1.f, 0.f)
//        };
//
//        Json collider = json[ "collider" ];
//        if ( entity )
//        {
//            entity->add< kege::Rigidbody >( rigidbody );
//            SceneLoader::collider( params, entity, collider );
//        }
//        else
//        {
//            SceneLoader::collider( params, entity, collider );
//            params->assets->add< kege::Rigidbody >
//            (
//                params->id, kege::Rigidbody{ rigidbody }
//            );
//        }
//    }
//
//    void SceneLoader::transform( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Transform transform = kege::Transform
//        {
//            .position = toVec3( json[ "position" ] ),
//            .orientation = toQuat( json[ "orientation" ] ),
//            .scale = toVec3( json[ "scale" ] ),
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Transform >( transform );
//        }
//        else
//        {
//            params->assets->add< kege::Transform >( params->id, transform );
//        }
//    }
//
//    void SceneLoader::particleEffect( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::ParticleEffect effect;
//        effect.rate_of_deterioration = json[ "rate_of_deterioration" ].toFloat();
//
//        Json behaviors = json[ "behaviors" ];
//        if( behaviors )
//        {
//            effect.behaviors.resize( behaviors.count() );
//            for (int i=0; i<json.count(); ++i)
//            {
//                ParticleBehaviorFunct behaviors_creator = _particle_behavior_creators[ behaviors[i][ "type" ].value() ];
//                effect.behaviors[i] = behaviors_creator( behaviors[i] );
//            }
//        }
//
//        Json initial = json[ "initial" ];
//        if( initial )
//        {
//            effect.initails = new ParticleInitails
//            ({
//                .size     = rand1f( initial[ "size" ][ "min" ].toFloat(),     initial[ "size" ][ "max" ].toFloat() ),
//                .speed    = rand1f( initial[ "speed" ][ "min" ].toFloat(),    initial[ "speed" ][ "max" ].toFloat() ),
//                .lifetime = rand1f( initial[ "lifetime" ][ "min" ].toFloat(), initial[ "lifetime" ][ "max" ].toFloat() ),
//                .mass     = rand1f( initial[ "mass" ][ "min" ].toFloat(),     initial[ "mass" ][ "max" ].toFloat() ),
//                .rotation = rand1f( initial[ "rotation" ][ "min" ].toFloat(), initial[ "rotation" ][ "max" ].toFloat() ),
//                .velocity = rand3f
//                (
//                    rand1f( initial[ "start" ][ 0 ].toFloat(), json[ "end" ][ 0 ].toFloat() ),
//                    rand1f( initial[ "start" ][ 1 ].toFloat(), json[ "end" ][ 1 ].toFloat() ),
//                    rand1f( initial[ "start" ][ 2 ].toFloat(), json[ "end" ][ 2 ].toFloat() )
//                ),
//                .color = rand4f
//                (
//                    rand1f( initial[ "start" ][ 0 ].toFloat(), initial[ "end" ][ 0 ].toFloat() ),
//                    rand1f( initial[ "start" ][ 1 ].toFloat(), initial[ "end" ][ 1 ].toFloat() ),
//                    rand1f( initial[ "start" ][ 2 ].toFloat(), initial[ "end" ][ 2 ].toFloat() ),
//                    rand1f( initial[ "start" ][ 3 ].toFloat(), initial[ "end" ][ 3 ].toFloat() )
//                ),
//                .saturation = rand1f( json[ "saturation" ][ "min" ].toFloat(), json[ "saturation" ][ "max" ].toFloat() ),
//            });
//        }
//
//        Json emitter = json[ "emitter" ];
//        if ( emitter )
//        {
//            ParticleEmitterFunct emitter_creator = _particle_emitter_creators[ emitter[ "type" ].value() ];
//            if ( emitter_creator )
//            {
//                if ( entity )
//                {
//                    entity->add< kege::ParticleEmitter >({ emitter_creator( emitter ) });
//                }
//                else
//                {
//                    params->assets->add< kege::ParticleEmitter >( params->id, { emitter_creator( emitter ) });
//                }
//            }
//        }
//
//        if ( entity )
//        {
//            entity->add< kege::ParticleBuffer >({ array< kege::Particle >( json[ "max_particles" ].toInt() ) });
//            entity->add< kege::ParticleEffect >( effect );
//        }
//        else
//        {
//            params->assets->add< kege::ParticleEffect >( params->id, effect );
//        }
//    }
//
//    void SceneLoader::orthographic( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::Projection > projection = new Orthographic
//        {
//            json[ "left"  ].toFloat(),
//            json[ "right" ].toFloat(),
//            json[ "below" ].toFloat(),
//            json[ "above" ].toFloat(),
//            json[ "znear" ].toFloat(),
//            json[ "zfar"  ].toFloat()
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Ref< kege::Projection > >( projection );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Projection > >( params->id, projection );
//        }
//    }
//
//    void SceneLoader::perspective( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::Projection > projection = new Perspective
//        {
//            json[ "aspect_ratio" ].toFloat(),
//            json[ "fov" ].toFloat(),
//            json[ "znear" ].toFloat(),
//            json[ "zfar" ].toFloat()
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Camera >({ projection });
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Projection > >( params->id, projection );
//        }
//    }
//    
//    void SceneLoader::directional( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::Light > light = new Light
//        {
//            kege::DirectionalLight
//            {
//                .color = toVec3( json[ "color" ] ),
//                .direction = toVec3( json[ "direction" ] )
//            }
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Ref< kege::Light > >( light );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
//        }
//    }
//
//    void SceneLoader::point( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::Light > light = new Light
//        {
//            kege::PointLight
//            {
//                .color = toVec3( json[ "color" ] ),
//                .linear_attenuation = json[ "linear_attenuation" ].toFloat(),
//                .quadratic_attenuation = json[ "quadratic_attenuation" ].toFloat()
//            }
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Ref< kege::Light > >( light );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
//        }
//    }
//
//    void SceneLoader::spot( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::Light > light = new Light
//        {
//            kege::SpotLight
//            {
//                .color = toVec3( json[ "color" ] ),
//                .direction = toVec3( json[ "color" ] ),
//                .spot_cutoff = json[ "cutoff" ].toFloat(),
//                .spot_exponent = json[ "exponent" ].toFloat(),
//                .linear_attenuation = json[ "linear_attenuation" ].toFloat(),
//                .quadratic_attenuation = json[ "quadratic_attenuation" ].toFloat()
//            }
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::Ref< kege::Light > >( light );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
//        }
//    }
//
//    void SceneLoader::skeleton( Params* params, ecs::Entity& entity, Json json )
//    {}
//
//    void SceneLoader::pbr( Params* params, ecs::Entity& entity, Json json )
//    {
//        Json properties = json[ "properties" ];
//        Json textures = json[ "textures" ];
//        Json passes = json[ "passes" ];
//    }
//
//    void SceneLoader::mesh( Params* params, ecs::Entity& entity, Json json )
//    {
//        Json json_operation = json[ "draw-operation" ];
//
//        std::vector< float > vertices = json[ "vertices" ][ "data" ].getVector< float >( stof );
//        std::vector< int32_t > indices = json[ "indices" ].getVector< int32_t >( atoi );
//    }
//
//    void SceneLoader::meshPath( Params* params, ecs::Entity& entity, Json json )
//    {
//        std::string mesh_path = json[ "mesh-path" ].value();
//        MeshLoader::load( params->assets, mesh_path );
//    }
//
//    void resolveGeometry( SceneLoader::Params* params, ecs::Entity& entity, kege::Ref< kege::MeshPrimitive > primative )
//    {
//        if ( entity )
//        {
//            entity->add< Geometry >
//            ({
//                .mesh = new Mesh
//                {{
//                    new Mesh
//                    (
//                        primative, // .primative =
//                        1, // .instance_count
//                        0, // .first_instance =
//                        0, // .first_index =
//                        primative->total_indices // .index_count
//                    )
//                }}
//            });
//        }
//        else
//        {
//            params->assets->add< kege::Geometry >( params->id, {
//                .mesh = new Mesh
//                {{
//                    new Mesh
//                    {
//                        primative,  // .primative =
//                        1, // .instance_count
//                        0, // .first_instance =
//                        0, // .first_index =
//                        primative->total_indices // .index_count
//                    }
//                }}
//            });
//        }
//    }
//
//    void SceneLoader::meshBox( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new CuboidMesh( vec3( 0.0 ), toVec3( json[ "extent" ] ) );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshCone( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new ConeMesh
//        (
//            json[ "radius" ].toFloat(),
//            json[ "hegith" ].toFloat(),
//            json[ "columns" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshRect( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new RectMesh
//        (
//            vec3( 0.0 ),
//            json[ "radius" ].toFloat(),
//            json[ "hegith" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshGrid( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new GridMesh
//        (
//            json[ "radius" ].toFloat(),
//            json[ "hegith" ].toFloat(),
//            json[ "columns" ].toFloat(),
//            json[ "rows" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshSphere( Params* params, ecs::Entity& entity, Json json )
//    {
//        float v_radius = 1;
//        float h_radius = 1;
//        float columns = 16;
//        float rows = 8;
//
//        if( json[ "v_radius" ] ) v_radius = json[ "v_radius" ].toFloat();
//        if( json[ "h_radius" ] ) h_radius = json[ "h_radius" ].toFloat();
//        if( json[ "columns" ] ) columns = json[ "columns" ].toFloat();
//        if( json[ "rows" ] ) h_radius = json[ "rows" ].toFloat();
//        if( json[ "radius" ] )
//        {
//            h_radius = json[ "radius" ].toFloat();
//            v_radius = json[ "radius" ].toFloat();
//        }
//
//        kege::Ref< kege::MeshPrimitive > primative = new EllipsoidMesh
//        (
//            v_radius,
//            h_radius,
//            columns,
//            rows
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshCircle( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new CircleMesh
//        (
//            toVec3( json[ "center" ] ),
//            json[ "radius" ].toFloat(),
//            json[ "divs" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::meshCylinder( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new CylinderMesh
//        (
//            json[ "radius" ].toFloat(),
//            json[ "height" ].toFloat(),
//            json[ "columns" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//    
//    void SceneLoader::meshIcosahedron( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::Ref< kege::MeshPrimitive > primative = new IcosahedronMesh
//        (
//            json[ "radius" ].toFloat()
//        );
//        resolveGeometry( params, entity, primative );
//    }
//
//    void SceneLoader::cameraController( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::CameraControls controller = kege::CameraControls
//        {
//            .angles = toVec3( json[ "angles" ] ),
//            .euler = toVec3( json[ "euler" ] ),
//            .sensitivity = json[ "sensitivity" ].toFloat(),
//            .smoothness = json[ "smoothness" ].toFloat()
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::CameraControls >( controller );
//            if ( json[ "player-controled" ].toBool() )
//            {
//                params->scene->setCameraEntity( *entity );
//            }
//        }
//        else
//        {
//            params->assets->add< kege::CameraControls >
//            (
//                params->id,
//                controller
//            );
//        }
//    }
//
//    void SceneLoader::movementController( Params* params, ecs::Entity& entity, Json json )
//    {
//        kege::MovementControl controller = kege::MovementControl
//        {
//            .speed = toVec3( json[ "speed" ] )
//        };
//
//        if ( entity )
//        {
//            entity->add< kege::MovementControl >( controller );
//            if ( json[ "player-controled" ].toBool() )
//            {
//                params->scene->setPlayer( *entity );
//            }
//        }
//        else
//        {
//            params->assets->add< kege::MovementControl >
//            (
//                params->id,
//                controller
//            );
//        }
//    }
//
//    void SceneLoader::components( Params* params, ecs::Entity& entity, Json json )
//    {
//        for (int i = 0; i < json.count(); ++i )
//        {
//            const std::string type = json[i][ "type" ].value();
//            ResourceParserFunctMap::iterator funct = _resource_parsers.find( type );
//            if ( funct != _resource_parsers.end() )
//            {
//                funct->second( params, entity, json[i][ type ] );
//            }
//        }
//    }
//
//    void SceneLoader::entities( Params* params, ecs::Entity& entity, Json json )
//    {
//        for (int i = 0; i < json.count(); ++i )
//        {
//            SceneLoader::entity( params, entity, json[i] );
//        }
//    }
//
//    void SceneLoader::component( Params* params, ecs::Entity& entity, Json json )
//    {
//        const std::string type = json[ "type" ].value();
//        ResourceParserFunctMap::iterator funct = _resource_parsers.find( type );
//        if ( funct != _resource_parsers.end() )
//        {
//            funct->second( params, entity, json[ type ] );
//        }
//    }
//
//    void SceneLoader::entity( Params* params, ecs::Entity& parent, Json json )
//    {
//        const std::string name = json[ "name" ].value();
//        const Json components = json[ "components" ];
//        const Json entities = json[ "entities" ];
//
//        Entity entity = Entity::create();
//        entity.add< EntityTag >({ name.c_str() });
//        SceneLoader::components( params, &entity, components );
//        SceneLoader::entities( params, &entity, entities );
//
//        if ( parent )
//        {
//            parent->attach( entity );
//        }
//        else
//        {
//            params->scene->insert( entity );
//        }
//    }
//
//    void SceneLoader::sourceEffect( Params* params, ecs::Entity& entity, Json json )
//    {
//        ParticleEffect* resource = params->assets->fetch< ParticleEffect >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< ParticleEffect >( *resource );
//        }
//    }
//    void SceneLoader::sourceAtmosphere( Params* params, ecs::Entity& entity, Json json )
//    {
//        Atmosphere* resource = params->assets->fetch< Atmosphere >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Atmosphere >( *resource );
//        }
//    }
//    void SceneLoader::sourceTerrain( Params* params, ecs::Entity& entity, Json json )
//    {
////        Ref< ParticleEffect >* resource = assets->fetch< Ref< Atmosphere > >( json[ "id" ].value() );
////        if ( resource )
////        {
////            entity->add< Ref< ParticleEffect > >( *resource );
////        }
//    }
//    void SceneLoader::sourcePlanet( Params* params, ecs::Entity& entity, Json json )
//    {
////        Ref< ParticleEffect >* resource = assets->fetch< Ref< ParticleEffect > >( json[ "id" ].value() );
////        if ( resource )
////        {
////            entity->add< Ref< ParticleEffect > >( *resource );
////        }
//    }
//    void SceneLoader::sourceLight( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Light >* resource = params->assets->fetch< Ref< Light > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< Light > >( *resource );
//        }
//    }
//    void SceneLoader::sourceTexture( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Light >* resource = params->assets->fetch< Ref< Light > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< Light > >( *resource );
//        }
//    }
//    void SceneLoader::sourceCamera( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Projection >* resource = params->assets->fetch< Ref< Projection > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Camera >({ .projection = (*resource).ref() });
//        }
//    }
//    void SceneLoader::sourceCollider( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Collider >* resource = params->assets->fetch< Ref< Collider > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            if ( !entity->has< Rigidbody >() )
//            {
//                entity->add< Rigidbody >();
//            }
//            entity->get< Rigidbody >()->collider = *resource;
//        }
//    }
//    void SceneLoader::sourceMaterial( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Material >* resource = params->assets->fetch< Ref< Material > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< Material > >( *resource );
//        }
//    }
//    void SceneLoader::sourceSkeleton( Params* params, ecs::Entity& entity, Json json )
//    {
////        Ref< ParticleEffect >* resource = assets->fetch< Ref< ParticleEffect > >( json[ "id" ].value() );
////        if ( resource )
////        {
////            entity->add< Ref< ParticleEffect > >( *resource );
////        }
//    }
//    void SceneLoader::sourceMesh( Params* params, ecs::Entity& entity, Json json )
//    {
//        Ref< Mesh >* resource = params->assets->fetch< Ref< Mesh > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< Mesh > >( *resource );
//        }
//    }
//
//    void SceneLoader::meta( Params* params, ecs::Entity& entity, Json json )
//    {
//    }
//
//    void setRigidbodyCollider( ecs::Entity& entity, kege::Ref< kege::Collider > collider )
//    {
//        kege::Rigidbody* rigidbody = entity->get< kege::Rigidbody >();
//        if ( rigidbody )
//        {
//            rigidbody->collider = collider;
//        }
//        else
//        {
//            entity->add< kege::Ref< kege::Collider > >( collider );
//        }
//    }
//
//    void SceneLoader::colliderCircle( Params* params, ecs::Entity& entity, Json json )
//    {
//        Circle shape;
//        shape.center = toVec3( json[ "center" ] );
//        shape.right  = toVec3( json[ "right" ] );
//        shape.normal = toVec3( json[ "normal" ] );
//        shape.radius = json[ "center" ].toFloat();
//
//        kege::Ref< kege::Collider > collider = new ColliderCircle( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::colliderMesh( Params* params, ecs::Entity& entity, Json json )
//    {
//    }
//
//    void SceneLoader::colliderCone( Params* params, ecs::Entity& entity, Json json )
//    {
//        Cone shape;
//        shape.height = json[ "height" ].toFloat();
//        shape.direction = toVec3( json[ "direction" ] );
//        shape.radius = json[ "center" ].toFloat();
//
//        kege::Ref< kege::Collider > collider = new ColliderCone( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::colliderCylinder( Params* params, ecs::Entity& entity, Json json )
//    {
//        Cylinder shape;
//        shape.height = json[ "height" ].toFloat();
//        shape.center = toVec3( json[ "center" ] );
//        shape.radius = json[ "center" ].toFloat();
//        shape.axes[0] = toVec3( json[ "axes" ][ 0 ] );
//        shape.axes[1] = toVec3( json[ "axes" ][ 1 ] );
//
//        kege::Ref< kege::Collider > collider = new ColliderCylinder( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::colliderPlane( Params* params, ecs::Entity& entity, Json json )
//    {
//        Plane shape;
//        shape.point = toVec3( json[ "point" ] );
//        shape.distance = json[ "distance" ].toFloat();
//        shape.normal = toVec3( json[ "normal" ][ 0 ] );
//        shape.one_sided = json[ "one_sided" ].toBool();
//
//        kege::Ref< kege::Collider > collider = new ColliderPlane( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::colliderSphere( Params* params, ecs::Entity& entity, Json json )
//    {
//        Sphere shape;
//        shape.center = toVec3( json[ "center" ] );
//        shape.radius = json[ "radius" ].toFloat();
//
//        kege::Ref< kege::Collider > collider = new ColliderSphere( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::colliderBox( Params* params, ecs::Entity& entity, Json json )
//    {
//        OBB shape;
//        shape.center = toVec3( json[ "center" ] );
//        shape.extents = toVec3( json[ "extents" ] );
//        shape.axes[0] = toVec3( json[ "axes" ][0] );
//        shape.axes[1] = toVec3( json[ "axes" ][1] );
//        shape.axes[2] = toVec3( json[ "axes" ][2] );
//
//        kege::Ref< kege::Collider > collider = new ColliderBox( shape );
//
//        if ( entity )
//        {
//            setRigidbodyCollider( entity, collider );
//        }
//        else
//        {
//            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
//        }
//    }
//
//    void SceneLoader::collider( Params* params, ecs::Entity& entity, Json json )
//    {
//        Json shape = json[ "shape" ];
//        Json type = shape[ "type" ];
//        ResourceParserFunctMap::iterator funct = _resource_parsers.find( type.value() );
//        if ( funct != _resource_parsers.end() )
//        {
//            funct->second( params, entity, shape[ type.value() ] );
//
//            Ref< Collider > collider;
//            if ( entity )
//            {
//                if ( entity->get< Ref< Collider > >() )
//                {
//                    collider = *entity->get< Ref< Collider > >();
//                }
//                else if ( entity->get< Rigidbody >() )
//                {
//                    collider = entity->get< Rigidbody >()->collider;
//                }
//            }
//            else
//            {
//                Ref< Collider >* res = params->assets->fetch< Ref< Collider > >( params->id );
//                if ( res )
//                {
//                    collider = *res;
//                }
//            }
//
//            if ( collider != nullptr )
//            {
//                collider->friction = json[ "friction" ].toFloat();
//                collider->cor = json[ "restitution" ].toFloat();
//                collider->is_trigger = json[ "is_trigger" ].toBool();
//            }
//        }
//    }
//
//    Ref< Behavior > SceneLoader::airResistance( Json json )
//    {
//        if( !json ) return {};
//        return new AirResistance
//        (
//            json[ "coefficient" ].toFloat(),
//            json[ "fluid_density" ].toFloat(),
//            json[ "use_quadratic_drag" ].toFloat()
//        );
//    }
//    Ref< Behavior > SceneLoader::colorOverLifetime( Json json )
//    {
//        if( !json ) return {};
//        Json json_gradient = json[ "gradient" ];
//        Gradient gradient;
//        for (int i=0; i<json.count(); ++i)
//        {
//            gradient.addKey( json_gradient[ "t" ].toFloat(), toVec4( json_gradient[ "color" ] ) );
//        }
//        return new ColorOverLifetime( gradient );
//    }
//
//    Ref< Behavior > SceneLoader::directionalGravity( Json json )
//    {
//        if( !json ) return {};
//        return new DirectionalGravity( toVec3( json[ "force" ] ) );
//    }
//
//    Ref< Behavior > SceneLoader::centerOfMassGravity( Json json )
//    {
//        if( !json ) return {};
//        return new CenterOfMassGravity
//        (
//            json[ "strength" ].toFloat(),
//            json[ "falloff" ].toFloat()
//        );
//    }
//
//    Ref< Behavior > SceneLoader::sizeOverLifetime( Json json )
//    {
//        if( !json ) return {};
//        Json json_curve = json[ "curve" ];
//
//        array< float > curve( json_curve.count() );
//        for (int i=0; i<json.count(); ++i)
//        {
//            curve[i] = json_curve[ i ].toFloat();
//        }
//        return new SizeOverLifetime( curve );
//    }
//
//    Ref< Behavior > SceneLoader::velocityOverLifetime( Json json )
//    {
//        if( !json ) return {};
//        return new AirResistance
//        (
//            json[ "coefficient" ].toFloat(),
//            json[ "fluid_density" ].toFloat(),
//            json[ "use_quadratic_drag" ].toFloat()
//        );
//    }
//
//    Ref< Behavior > SceneLoader::windBehavior( Json json )
//    {
//        if( !json ) return {};
//        return new AirResistance
//        (
//            json[ "coefficient" ].toFloat(),
//            json[ "fluid_density" ].toFloat(),
//            json[ "use_quadratic_drag" ].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterCone( Json json )
//    {
//        if( !json ) return {};
//
//        float emissions_per_second = json[ "emissions_per_second" ].toFloat();
//        bool burst = json[ "burst" ].toBool();
//        kege::vec3 axes[2];
//        axes[0] = toVec3( json[ "up" ] );
//        axes[1] = toVec3( json[ "right" ] );
//        float height = json[ "height" ].toFloat();
//        float min_radius = json[ "min_radius" ].toFloat();
//        float max_radius = json[ "max_radius" ].toFloat();
//
//        return new EmitterCone
//        (
//            emissions_per_second,
//            burst,
//            axes,
//            height,
//            min_radius,
//            max_radius
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterLine( Json json )
//    {
//        if( !json ) return {};
//
//        float emissions_per_second = json[ "emissions_per_second" ].toFloat();
//        bool burst = json[ "burst" ].toBool();
//        kege::point3 start = toVec3( json[ "start" ] );
//        kege::point3 end = toVec3( json[ "end" ] );
//
//        return new EmitterLine
//        (
//            emissions_per_second,
//            burst,
//            start,
//            end
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterCube( Json json )
//    {
//        if( !json ) return {};
//        return new EmitterCube
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            json[ "width" ][0].toFloat(),
//            json[ "height" ][0].toFloat(),
//            json[ "depth" ][0].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterPlane( Json json )
//    {
//        if( !json ) return {};
//        return new EmitterPlane
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            toVec3( json[ "a" ] ),
//            toVec3( json[ "b" ] ),
//            toVec3( json[ "c" ] ),
//            toVec2( json[ "extents" ] )
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterCircle( Json json )
//    {
//        if( !json ) return {};
//
//        kege::vec3 axis[2] =
//        {
//            toVec3( json[ "up" ] ),
//            toVec3( json[ "right" ] )
//        };
//
//        return new EmitterCircleArea
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            axis,
//            json[ "min-radius" ].toFloat(),
//            json[ "max-radius" ].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterPyrimid( Json json )
//    {
//        if( !json ) return {};
//        return new EmitterPyrimid
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            json[ "width" ].toFloat(),
//            json[ "height" ].toFloat(),
//            json[ "depth" ].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterSphere( Json json )
//    {
//        if( !json ) return {};
//        return new EmitterSphereArea
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            json[ "min-radius" ].toFloat(),
//            json[ "max-radius" ].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterTriangle( Json json )
//    {
//        if( !json ) return {};
//        return new EmitterTriangle
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            toVec3( json[ "a" ] ),
//            toVec3( json[ "b" ] ),
//            toVec3( json[ "c" ] ),
//            json[ "height" ].toFloat(),
//            json[ "base" ].toFloat()
//        );
//    }
//
//    Ref< Emitter > SceneLoader::emitterCylinder( Json json )
//    {
//        if( !json ) return {};
//        kege::vec3 axes[2] =
//        {
//            toVec3( json[ "up" ] ),
//            toVec3( json[ "right" ] )
//        };
//
//        return new EmitterCylinder
//        (
//            json[ "emissions_per_second" ].toFloat(),
//            json[ "burst" ].toBool(),
//            axes,
//            json[ "height" ].toFloat(),
//            json[ "min_radius" ].toFloat(),
//            json[ "max_radius" ].toFloat()
//        );
//    }


    void parseComponentTransform( kege::ECS& ecs, ecs::Entity& entity, Json data )
    {
        Transform* transform = ecs.add< Transform >( entity );
        transform->position = toVec3( data[ "position" ] );
        transform->orientation = toQuat( data[ "rotation" ] );
        transform->scale = toVec3( data[ "scale" ] );
    }

    void parseComponentRigidBody( kege::ECS& ecs, ecs::Entity& entity, Json data )
    {
        kege::Rigidbody* rigidbody = ecs.add< kege::Rigidbody >( entity );
        rigidbody->linear = {};

        rigidbody->linear.forces = {};
        rigidbody->linear.acceleration = {};
        rigidbody->linear.velocity = toVec3( data[ "linear_velocity" ] );
        rigidbody->linear.invmass = invMass( data[ "mass" ].toFloat() );
        rigidbody->linear.damping = data[ "linear_damping" ].toFloat();

        rigidbody->angular.torques = {};
        rigidbody->angular.rotation = {};
        rigidbody->angular.velocity = toVec3( data[ "angular_velocity" ] );
        rigidbody->angular.inertia_inverse = mat33( toVec3( data[ "inertia" ] ) );
        rigidbody->angular.damping = data[ "angular_damping" ].toFloat();

        rigidbody->center = toVec3( data[ "center" ] );
        rigidbody->orientation = toQuat( data[ "orientation" ] );
        rigidbody->anti_gravity = data[ "anti_gravity" ].toBool();
        rigidbody->immovable = data[ "immovable" ].toBool();
        rigidbody->sleepable = data[ "sleepable" ].toBool();
        rigidbody->up = vec3(0.f, 1.f, 0.f);
    }

    void parseComponentOrthographic( kege::ECS& ecs, ecs::Entity& entity, Json json )
    {
        *ecs.add< kege::Orthographic >( entity ) = Orthographic
        {
            json[ "left"  ].toFloat(),
            json[ "right" ].toFloat(),
            json[ "below" ].toFloat(),
            json[ "above" ].toFloat(),
            json[ "znear" ].toFloat(),
            json[ "zfar"  ].toFloat()
        };
    }

    void parseComponentPerspective( kege::ECS& ecs, ecs::Entity& entity, Json json )
    {
        *ecs.add< kege::Perspective >( entity ) = kege::Perspective
        {
            json[ "aspect_ratio" ].toFloat(),
            json[ "fov" ].toFloat(),
            json[ "znear" ].toFloat(),
            json[ "zfar" ].toFloat()
        };
    }

    void parseComponentCameraController( kege::ECS& ecs, ecs::Entity& entity, Json data )
    {
        kege::CameraControls* controller = ecs.add< kege::CameraControls >( entity );
        controller->angles = toVec3( data[ "angles" ] );
        controller->euler = toVec3( data[ "euler" ] );
        controller->sensitivity = data[ "sensitivity" ].toFloat();
        controller->stiffness = data[ "stiffness" ].toFloat();
    }

    void parseComponentMovementController( kege::ECS& ecs, ecs::Entity& entity, Json json )
    {
        kege::MovementControl* controller = ecs.add< kege::MovementControl >( entity );
        controller->speed = toVec3( json[ "speed" ] );
    }

    void parseComponentRenderable( kege::ECS& ecs, ecs::Entity& entity, Json conponent )
    {}

    void parseComponentPlanet( kege::ECS& ecs, ecs::Entity& entity, Json data )
    {
//        "radius": 6371000.0,
//        "mass": 5.972e24,
//        "rotation_speed": 0.0000727,
//        "axial_tilt": 23.5,
//        "ocean_level": 0.5,
//        "has_atmosphere": true,
//        "atmosphere_height": 100000.0,
//        "atmosphere_color": [0.53, 0.81, 0.92, 0.3],
//        "has_clouds": true,
//        "cloud_height": 10000.0
    }
    void parseComponentSphericalTerrain( kege::ECS& ecs, ecs::Entity& entity, Json conponent )
    {
//        "lodLevels": 6,
//        "maxSubdivisions": 8,
//        "heightmapTexture": 0,
//        "normalMapTexture": 1,
//        "detailDistance": 50000.0,
//        "chunkSize": 256
    }

//    void parseComponentTerrainGenerator( Entity& entity, Json data )
//    {
//        TerrainGeneration* generation = entity.add< TerrainGeneration >();
//        generation->name = data[ "name" ].toStr();
//        generation->recompute = true;
//
//        Json layers = data[ "layers" ];
//        generation->layers.resize( layers.count() );
//        for (int k=0; k<layers.count(); ++k)
//        {
//            generation->layers[k].name = layers[ "name" ].toStr();
//            generation->layers[k].operation = layers[ "operation" ].toStr();
//            generation->layers[k].generator = layers[ "generator" ].toInt();
//            generation->layers[k].weight = layers[ "weight" ].toFloat();
//        }
//
//        Json generators = data[ "generators" ];
//        generation->generators.resize( generators.count() );
//        for (int i=0; i<generators.count(); ++i)
//        {
//            Json parameters = generators[ "parameters" ];
//            generation->generators[i].name = generators[ "name" ].toStr();
//            generation->generators[i].type = generators[ "type" ].toStr();
//            generation->generators[i].parameters.resize( parameters.count() );
//
//            std::vector< NoiseParameters >& noise_parameters = generation->generators[i].parameters;
//            for (int k=0; k<parameters.count(); ++k)
//            {
//                noise_parameters[k].seed = parameters[ "seed" ].toInt();
//                noise_parameters[k].octaves = parameters[ "octaves" ].toInt();
//                noise_parameters[k].frequency = parameters[ "frequency" ].toFloat();
//                noise_parameters[k].amplitude = parameters[ "amplitude" ].toFloat();
//                noise_parameters[k].lacunarity = parameters[ "lacunarity" ].toFloat();
//                noise_parameters[k].persistence = parameters[ "persistence" ].toFloat();
//                noise_parameters[k].amplitude = parameters[ "amplitude" ].toFloat();
//                noise_parameters[k].warp_strength = parameters[ "warp_strength" ].toFloat();
//                noise_parameters[k].warp = parameters[ "warp" ].toBool();
//            }
//        }
//    }

    void parseComponentPlanarTerrain( kege::ECS& ecs, ecs::Entity& entity, Json conponent )
    {}
    void parseComponentBiomeMap( kege::ECS& ecs, ecs::Entity& entity, Json conponent )
    {}
    void parseComponentAtmosphere( kege::ECS& ecs, ecs::Entity& entity, Json conponent )
    {}

    SceneLoader::SceneLoader()
    {
        _entity_component_parsers[ "Transform" ] = parseComponentTransform;
        _entity_component_parsers[ "RigidBody" ] = parseComponentRigidBody;
        _entity_component_parsers[ "Perspective" ] = parseComponentPerspective;
        _entity_component_parsers[ "Orthographic" ] = parseComponentOrthographic;
        _entity_component_parsers[ "MovementController" ] = parseComponentMovementController;
        _entity_component_parsers[ "CameraController" ] = parseComponentCameraController;


        _entity_component_parsers[ "Planet" ] = parseComponentPlanet;
        _entity_component_parsers[ "SphericalTerrain" ] = parseComponentSphericalTerrain;
        _entity_component_parsers[ "PlanarTerrain" ] = parseComponentPlanarTerrain;
        _entity_component_parsers[ "BiomeMap" ] = parseComponentBiomeMap;
        _entity_component_parsers[ "Atmosphere" ] = parseComponentAtmosphere;

//        _particle_behavior_creators[ "air-resistance" ] = airResistance;
//        _particle_behavior_creators[ "color_over_lifetime" ] = colorOverLifetime;
//        _particle_behavior_creators[ "directional_gravity" ] = directionalGravity;
//        _particle_behavior_creators[ "center_of_mass_gravity" ] = centerOfMassGravity;
//        _particle_behavior_creators[ "size_over_lifetime" ] = sizeOverLifetime;
//        _particle_behavior_creators[ "velocity_over_lifetime" ] = velocityOverLifetime;
//        _particle_behavior_creators[ "air_behavior" ] = windBehavior;
//
//        _particle_emitter_creators[ "emitter-cone" ] = emitterCone;
//        _particle_emitter_creators[ "emitter-line" ] = emitterLine;
//        _particle_emitter_creators[ "emitter-cube" ] = emitterCube;
//        _particle_emitter_creators[ "emitter-plane" ] = emitterPlane;
//        _particle_emitter_creators[ "emitter-circle" ] = emitterCircle;
//        _particle_emitter_creators[ "emitter-pyrimid" ] = emitterPyrimid;
//        _particle_emitter_creators[ "emitter-sphere" ] = emitterSphere;
//        _particle_emitter_creators[ "emitter-triangle" ] = emitterTriangle;
//        _particle_emitter_creators[ "emitter-cylinder" ] =  emitterCylinder;
//
//        _resource_parsers[ "source-mesh" ] = sourceMesh;
//        _resource_parsers[ "source-collider" ] = sourceCollider;
//        _resource_parsers[ "source-material" ] = sourceMaterial;
//        _resource_parsers[ "source-light" ] = sourceLight;
//        _resource_parsers[ "source-texture" ] = sourceTexture;
//        _resource_parsers[ "source-effect" ] = sourceEffect;
//        _resource_parsers[ "source-atmosphere" ] = sourceAtmosphere;
//        _resource_parsers[ "source-terrain" ] = sourceTerrain;
//        _resource_parsers[ "source-planet" ] = sourcePlanet;
//        _resource_parsers[ "source-camera" ] = sourceCamera;
//        _resource_parsers[ "source-skeleton" ] = sourceSkeleton;
//
//        _resource_parsers[ "collider-mesh" ] = colliderMesh;
//        _resource_parsers[ "collider-cont" ] = colliderCone;
//        _resource_parsers[ "collider-circle" ] = colliderCircle;
//        _resource_parsers[ "collider-cylinder" ] = colliderCylinder;
//        _resource_parsers[ "collider-plane" ] = colliderPlane;
//        _resource_parsers[ "collider-sphere" ] = colliderSphere;
//        _resource_parsers[ "collider-box" ] = colliderBox;
//        _resource_parsers[ "collider" ] = collider;
//
//        _resource_parsers[ "mesh-box" ] = meshBox;
//        _resource_parsers[ "mesh-cone" ] = meshCone;
//        _resource_parsers[ "mesh-rect" ] = meshRect;
//        _resource_parsers[ "mesh-grid" ] = meshGrid;
//        _resource_parsers[ "mesh-sphere" ] = meshSphere;
//        _resource_parsers[ "mesh-circle" ] = meshCircle;
//        _resource_parsers[ "mesh-cylinder" ] = meshCylinder;
//        _resource_parsers[ "mesh-icosahedron" ] = meshIcosahedron;
//        _resource_parsers[ "mesh-path" ] = meshPath;
//
//        _resource_parsers[ "terrain" ] = animation;
//        _resource_parsers[ "transform" ] = transform;
//        _resource_parsers[ "orthographic" ] = orthographic;
//        _resource_parsers[ "perspective" ] = perspective;
//        _resource_parsers[ "atmosphere" ] = atmosphere;
//        _resource_parsers[ "rigidbody" ] = rigidbody;
//        _resource_parsers[ "directional" ] = directional;
//        _resource_parsers[ "point" ] = point;
//        _resource_parsers[ "spot" ] = spot;
//        //_resource_parsers[ "animation" ] = animation;
//        _resource_parsers[ "skeleton" ] = skeleton;
//        _resource_parsers[ "mesh" ] = mesh;
//        _resource_parsers[ "camera-controller" ] = cameraController;
//        _resource_parsers[ "movement-controller" ] = movementController;
//
//        _resource_parsers[ "atmosphere-component" ] = component;
//        _resource_parsers[ "terrain-component" ] = component;
//        _resource_parsers[ "planet-component" ] = component;
//        _resource_parsers[ "transform-component" ] = component;
//        _resource_parsers[ "particle-effect-component" ] = component;
//        _resource_parsers[ "light-component" ] = component;
//        _resource_parsers[ "camera-component" ] = component;
//        _resource_parsers[ "rigidbody-component" ] = component;
//        _resource_parsers[ "collider-component" ] = component;
//        _resource_parsers[ "material-component" ] = component;
//        _resource_parsers[ "skeleton-component" ] = component;
//        _resource_parsers[ "mesh-component" ] = component;
//        _resource_parsers[ "movement-controller-component" ] = component;
//        _resource_parsers[ "camera-controller-component" ] = component;
//
//        _resource_parsers[ "components" ] = components;
//        _resource_parsers[ "entity" ] = entity;
//
//        _resource_parsers[ "entities" ] = getAssetManager;
//        _resource_parsers[ "animations" ] = getAssetManager;
//        _resource_parsers[ "materials" ] = getAssetManager;
//        _resource_parsers[ "textures" ] = getAssetManager;
//        _resource_parsers[ "effects" ] = getAssetManager;
//        _resource_parsers[ "cameras" ] = getAssetManager;
//        _resource_parsers[ "lights" ] = getAssetManager;
//        _resource_parsers[ "meshs" ] = getAssetManager;
//        _resource_parsers[ "meta" ] = meta;
    }
}
