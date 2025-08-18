//
//  scene-loader.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#include "scene-loader.hpp"

namespace kege{

    SceneLoader::ParticleBehaviorFunctMap SceneLoader::_particle_behavior_creators;
    SceneLoader::ParticleEmitterFunctMap SceneLoader::_particle_emitter_creators;
    SceneLoader::ColliderFunctMap SceneLoader::_collider_funct_map;
    SceneLoader::ResourceParserFunctMap SceneLoader::_resource_parsers;

    kege::Ref< kege::Scene > SceneLoader::load( const std::string& filename )
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            KEGE_LOG_ERROR << "Failed to open scene file: " << filename;
            return {};
        }

        kege::Ref< kege::Scene > scene = new kege::Scene();

        Params params;
        params.scene = scene.ref();
        params.assets = &scene->getAssetSystem();

        for (int i = 0; i < json.count(); ++i )
        {
            Json element = json[i];
            std::string type = element[ "type" ].value();
            ResourceParserFunctMap::iterator funct = _resource_parsers.find( type );
            if ( funct != _resource_parsers.end() )
            {
                funct->second( &params, nullptr, element[ type ] );
            }
        }

        return scene;
    }

    void SceneLoader::resourceManager( Params* params, Entity* entity, Json json )
    {
        for (int i = 0; i < json.count(); ++i )
        {
            Json data = json[ i ][ "data" ];
            params->id = json[ i ][ "id" ].value();
            params->type = data[ "type" ].value();
            ResourceParserFunctMap::iterator funct = _resource_parsers.find( params->type );
            if ( funct != _resource_parsers.end() )
            {
                funct->second( params, nullptr, data[ params->type ] );
            }
        }
    }

    void SceneLoader::atmosphere( Params* params, Entity* entity, Json json )
    {
        kege::Atmosphere atmosphere = kege::Atmosphere
        {
            .inner_radius = json[ "inner_radius" ].getDouble(),
            .outer_radius = json[ "outer_radius" ].getDouble(),

            .rayleigh.scattering_coefficient = toVec3( json[ "rayleigh" ][ "scattering_coefficient" ] ),
            .rayleigh.scale_height = json[ "rayleigh" ][ "scale_height" ].getFloat(),

            .mie.scattering_coefficient = toVec3( json[ "mie" ][ "scattering_coefficient" ] ),
            .mie.scale_height = json[ "mie" ][ "scale_height" ].getFloat(),
            .mie.anisotropy = json[ "mie" ][ "anisotropy" ].getFloat(),

            .absorption.scattering_coefficient = toVec3( json[ "absorption" ][ "absorption_coefficient" ] ),
            .absorption.ozone_center_height = json[ "absorption" ][ "ozone_center_height" ].getFloat(),

            .rendering.ground_color = toVec3( json[ "rendering" ][ "ground_color" ] ),
            .rendering.sky_color = toVec3( json[ "rendering" ][ "sky_color" ] ),
            .rendering.enable_multiple_scattering = json[ "rendering" ][ "enable_multiple_scattering" ].getBool(),
            .rendering.samples = json[ "rendering" ][ "samples" ].getInt()
        };

        if ( entity )
        {
            entity->add< kege::Atmosphere >( atmosphere );
        }
        else
        {
            params->assets->add< kege::Atmosphere >( params->id, atmosphere );
        }
    }

    float invMass( float mass )
    {
        return ( mass != 0 ) ? (1.0 / mass) : 0.0;
    }

    void SceneLoader::rigidbody( Params* params, Entity* entity, Json json )
    {
        kege::Rigidbody rigidbody = kege::Rigidbody
        {
            .linear = LinearMotion
            {
                .forces = {},
                .acceleration = {},
                .velocity = toVec3( json[ "linear" ][ "velocity" ] ),
                .invmass = invMass( json[ "linear" ][ "mass" ].getFloat() ),
                .damping = json[ "linear" ][ "damping" ].getFloat()
            },
            .angular = AngularMotion
            {
                .torques = {},
                .rotation = {},
                .velocity = toVec3( json[ "angular" ][ "velocity" ] ),
                .inertia_inverse = mat33( json[ "angular" ][ "inertia" ].getFloat() ),
                .damping = json[ "angular" ][ "damping" ].getFloat()
            },
            .center = toVec3( json[ "center" ] ),
            .orientation = toQuat( json[ "orientation" ] ),
            .anti_gravity = json[ "anti_gravity" ].getBool(),
            .immovable = json[ "immovable" ].getBool(),
            .sleepable = json[ "sleepable" ].getBool(),
            .up = vec3(0.f, 1.f, 0.f)
        };

        Json collider = json[ "collider" ];
        if ( entity )
        {
            entity->add< kege::Rigidbody >( rigidbody );
            SceneLoader::collider( params, entity, collider );
        }
        else
        {
            SceneLoader::collider( params, entity, collider );
            params->assets->add< kege::Rigidbody >
            (
                params->id, kege::Rigidbody{ rigidbody }
            );
        }
    }

    void SceneLoader::transform( Params* params, Entity* entity, Json json )
    {
        kege::Transform transform = kege::Transform
        {
            .position = toVec3( json[ "position" ] ),
            .orientation = toQuat( json[ "orientation" ] ),
            .scale = toVec3( json[ "scale" ] ),
        };

        if ( entity )
        {
            entity->add< kege::Transform >( transform );
        }
        else
        {
            params->assets->add< kege::Transform >( params->id, transform );
        }
    }

    void SceneLoader::particleEffect( Params* params, Entity* entity, Json json )
    {
        kege::ParticleEffect effect;
        effect.rate_of_deterioration = json[ "rate_of_deterioration" ].getFloat();

        Json behaviors = json[ "behaviors" ];
        if( behaviors )
        {
            effect.behaviors.resize( behaviors.count() );
            for (int i=0; i<json.count(); ++i)
            {
                ParticleBehaviorFunct behaviors_creator = _particle_behavior_creators[ behaviors[i][ "type" ].value() ];
                effect.behaviors[i] = behaviors_creator( behaviors[i] );
            }
        }

        Json initial = json[ "initial" ];
        if( initial )
        {
            effect.initails = new ParticleInitails
            ({
                .size     = rand1f( initial[ "size" ][ "min" ].getFloat(),     initial[ "size" ][ "max" ].getFloat() ),
                .speed    = rand1f( initial[ "speed" ][ "min" ].getFloat(),    initial[ "speed" ][ "max" ].getFloat() ),
                .lifetime = rand1f( initial[ "lifetime" ][ "min" ].getFloat(), initial[ "lifetime" ][ "max" ].getFloat() ),
                .mass     = rand1f( initial[ "mass" ][ "min" ].getFloat(),     initial[ "mass" ][ "max" ].getFloat() ),
                .rotation = rand1f( initial[ "rotation" ][ "min" ].getFloat(), initial[ "rotation" ][ "max" ].getFloat() ),
                .velocity = rand3f
                (
                    rand1f( initial[ "start" ][ 0 ].getFloat(), json[ "end" ][ 0 ].getFloat() ),
                    rand1f( initial[ "start" ][ 1 ].getFloat(), json[ "end" ][ 1 ].getFloat() ),
                    rand1f( initial[ "start" ][ 2 ].getFloat(), json[ "end" ][ 2 ].getFloat() )
                ),
                .color = rand4f
                (
                    rand1f( initial[ "start" ][ 0 ].getFloat(), initial[ "end" ][ 0 ].getFloat() ),
                    rand1f( initial[ "start" ][ 1 ].getFloat(), initial[ "end" ][ 1 ].getFloat() ),
                    rand1f( initial[ "start" ][ 2 ].getFloat(), initial[ "end" ][ 2 ].getFloat() ),
                    rand1f( initial[ "start" ][ 3 ].getFloat(), initial[ "end" ][ 3 ].getFloat() )
                ),
                .saturation = rand1f( json[ "saturation" ][ "min" ].getFloat(), json[ "saturation" ][ "max" ].getFloat() ),
            });
        }

        Json emitter = json[ "emitter" ];
        if ( emitter )
        {
            ParticleEmitterFunct emitter_creator = _particle_emitter_creators[ emitter[ "type" ].value() ];
            if ( emitter_creator )
            {
                if ( entity )
                {
                    entity->add< kege::ParticleEmitter >({ emitter_creator( emitter ) });
                }
                else
                {
                    params->assets->add< kege::ParticleEmitter >( params->id, { emitter_creator( emitter ) });
                }
            }
        }

        if ( entity )
        {
            entity->add< kege::ParticleBuffer >({ array< kege::Particle >( json[ "max_particles" ].getInt() ) });
            entity->add< kege::ParticleEffect >( effect );
        }
        else
        {
            params->assets->add< kege::ParticleEffect >( params->id, effect );
        }
    }

    void SceneLoader::orthographic( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Projection > projection = new Orthographic
        {
            json[ "left"  ].getFloat(),
            json[ "right" ].getFloat(),
            json[ "below" ].getFloat(),
            json[ "above" ].getFloat(),
            json[ "znear" ].getFloat(),
            json[ "zfar"  ].getFloat()
        };

        if ( entity )
        {
            entity->add< kege::Ref< kege::Projection > >( projection );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Projection > >( params->id, projection );
        }
    }

    void SceneLoader::perspective( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Projection > projection = new Perspective
        {
            json[ "aspect_ratio" ].getFloat(),
            json[ "fov" ].getFloat(),
            json[ "znear" ].getFloat(),
            json[ "zfar" ].getFloat()
        };

        if ( entity )
        {
            entity->add< kege::Camera >({ projection });
        }
        else
        {
            params->assets->add< kege::Ref< kege::Projection > >( params->id, projection );
        }
    }
    
    void SceneLoader::directional( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Light > light = new Light
        {
            kege::DirectionalLight
            {
                .color = toVec3( json[ "color" ] ),
                .direction = toVec3( json[ "direction" ] )
            }
        };

        if ( entity )
        {
            entity->add< kege::Ref< kege::Light > >( light );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
        }
    }

    void SceneLoader::point( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Light > light = new Light
        {
            kege::PointLight
            {
                .color = toVec3( json[ "color" ] ),
                .linear_attenuation = json[ "linear_attenuation" ].getFloat(),
                .quadratic_attenuation = json[ "quadratic_attenuation" ].getFloat()
            }
        };

        if ( entity )
        {
            entity->add< kege::Ref< kege::Light > >( light );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
        }
    }

    void SceneLoader::spot( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Light > light = new Light
        {
            kege::SpotLight
            {
                .color = toVec3( json[ "color" ] ),
                .direction = toVec3( json[ "color" ] ),
                .spot_cutoff = json[ "cutoff" ].getFloat(),
                .spot_exponent = json[ "exponent" ].getFloat(),
                .linear_attenuation = json[ "linear_attenuation" ].getFloat(),
                .quadratic_attenuation = json[ "quadratic_attenuation" ].getFloat()
            }
        };

        if ( entity )
        {
            entity->add< kege::Ref< kege::Light > >( light );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Light > >( params->id, light );
        }
    }

    void SceneLoader::skeleton( Params* params, Entity* entity, Json json )
    {}

    void SceneLoader::pbr( Params* params, Entity* entity, Json json )
    {
        Json properties = json[ "properties" ];
        Json textures = json[ "textures" ];
        Json passes = json[ "passes" ];
    }

    void SceneLoader::mesh( Params* params, Entity* entity, Json json )
    {
        Json json_operation = json[ "draw-operation" ];

        std::vector< float > vertices = json[ "vertices" ][ "data" ].getVector< float >( stof );
        std::vector< int32_t > indices = json[ "indices" ].getVector< int32_t >( atoi );
    }

    void SceneLoader::meshPath( Params* params, Entity* entity, Json json )
    {
        std::string mesh_path = json[ "mesh-path" ].value();
        MeshLoader::load( params->assets, mesh_path );
    }

    void SceneLoader::meshBox( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new CuboidMesh( vec3( 0.0 ), toVec3( json[ "extent" ] ) );
        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshCone( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new ConeMesh
        (
            json[ "radius" ].getFloat(),
            json[ "hegith" ].getFloat(),
            json[ "columns" ].getFloat()
        );
        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshRect( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new RectMesh
        (
            vec3( 0.0 ),
            json[ "radius" ].getFloat(),
            json[ "hegith" ].getFloat()
        );

        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshGrid( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new GridMesh
        (
            json[ "radius" ].getFloat(),
            json[ "hegith" ].getFloat(),
            json[ "columns" ].getFloat(),
            json[ "rows" ].getFloat()
        );

        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshSphere( Params* params, Entity* entity, Json json )
    {
        float v_radius = 1;
        float h_radius = 1;
        float columns = 16;
        float rows = 8;

        if( json[ "v_radius" ] ) v_radius = json[ "v_radius" ].getFloat();
        if( json[ "h_radius" ] ) h_radius = json[ "h_radius" ].getFloat();
        if( json[ "columns" ] ) columns = json[ "columns" ].getFloat();
        if( json[ "rows" ] ) h_radius = json[ "rows" ].getFloat();
        if( json[ "radius" ] )
        {
            h_radius = json[ "radius" ].getFloat();
            v_radius = json[ "radius" ].getFloat();
        }

        kege::Ref< kege::Mesh > mesh = new EllipsoidMesh
        (
            v_radius,
            h_radius,
            columns,
            rows
        );
        
        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshCircle( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new CircleMesh
        (
            toVec3( json[ "center" ] ),
            json[ "radius" ].getFloat(),
            json[ "divs" ].getFloat()
        );

        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::meshCylinder( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new CylinderMesh
        (
            json[ "radius" ].getFloat(),
            json[ "height" ].getFloat(),
            json[ "columns" ].getFloat()
        );

        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }
    
    void SceneLoader::meshIcosahedron( Params* params, Entity* entity, Json json )
    {
        kege::Ref< kege::Mesh > mesh = new IcosahedronMesh
        (
            json[ "radius" ].getFloat()
        );

        if ( entity )
        {
            entity->add< kege::Ref< kege::Mesh > >( mesh );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Mesh > >( params->id, mesh );
        }
    }

    void SceneLoader::cameraController( Params* params, Entity* entity, Json json )
    {
        kege::CameraControls controller = kege::CameraControls
        {
            .angles = toVec3( json[ "angles" ] ),
            .euler = toVec3( json[ "euler" ] ),
            .sensitivity = json[ "sensitivity" ].getFloat(),
            .smoothness = json[ "smoothness" ].getFloat()
        };

        if ( entity )
        {
            entity->add< kege::CameraControls >( controller );
            if ( json[ "player-controled" ].getBool() )
            {
                params->scene->setCameraEntity( *entity );
            }
        }
        else
        {
            params->assets->add< kege::CameraControls >
            (
                params->id,
                controller
            );
        }
    }

    void SceneLoader::movementController( Params* params, Entity* entity, Json json )
    {
        kege::MovementControl controller = kege::MovementControl
        {
            .speed = toVec3( json[ "speed" ] )
        };

        if ( entity )
        {
            entity->add< kege::MovementControl >( controller );
            if ( json[ "player-controled" ].getBool() )
            {
                params->scene->setPlayer( *entity );
            }
        }
        else
        {
            params->assets->add< kege::MovementControl >
            (
                params->id,
                controller
            );
        }
    }

    void SceneLoader::components( Params* params, Entity* entity, Json json )
    {
        for (int i = 0; i < json.count(); ++i )
        {
            const std::string type = json[i][ "type" ].value();
            ResourceParserFunctMap::iterator funct = _resource_parsers.find( type );
            if ( funct != _resource_parsers.end() )
            {
                funct->second( params, entity, json[i][ type ] );
            }
        }
    }

    void SceneLoader::entities( Params* params, Entity* entity, Json json )
    {
        for (int i = 0; i < json.count(); ++i )
        {
            SceneLoader::entity( params, entity, json[i] );
        }
    }

    void SceneLoader::component( Params* params, Entity* entity, Json json )
    {
        const std::string type = json[ "type" ].value();
        ResourceParserFunctMap::iterator funct = _resource_parsers.find( type );
        if ( funct != _resource_parsers.end() )
        {
            funct->second( params, entity, json[ type ] );
        }
    }

    void SceneLoader::entity( Params* params, Entity* parent, Json json )
    {
        const std::string name = json[ "name" ].value();
        const Json components = json[ "components" ];
        const Json entities = json[ "entities" ];

        Entity entity = Entity::create();
        entity.add< EntityTag >({ name.c_str() });
        SceneLoader::components( params, &entity, components );
        SceneLoader::entities( params, &entity, entities );

        if ( parent )
        {
            parent->attach( entity );
        }
        else
        {
            params->scene->insert( entity );
        }
    }

    void SceneLoader::sourceEffect( Params* params, Entity* entity, Json json )
    {
        ParticleEffect* resource = params->assets->getAsset< ParticleEffect >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< ParticleEffect >( *resource );
        }
    }
    void SceneLoader::sourceAtmosphere( Params* params, Entity* entity, Json json )
    {
        Atmosphere* resource = params->assets->getAsset< Atmosphere >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Atmosphere >( *resource );
        }
    }
    void SceneLoader::sourceTerrain( Params* params, Entity* entity, Json json )
    {
//        Ref< ParticleEffect >* resource = assets->getAsset< Ref< Atmosphere > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< ParticleEffect > >( *resource );
//        }
    }
    void SceneLoader::sourcePlanet( Params* params, Entity* entity, Json json )
    {
//        Ref< ParticleEffect >* resource = assets->getAsset< Ref< ParticleEffect > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< ParticleEffect > >( *resource );
//        }
    }
    void SceneLoader::sourceLight( Params* params, Entity* entity, Json json )
    {
        Ref< Light >* resource = params->assets->getAsset< Ref< Light > >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Ref< Light > >( *resource );
        }
    }
    void SceneLoader::sourceTexture( Params* params, Entity* entity, Json json )
    {
        Ref< Light >* resource = params->assets->getAsset< Ref< Light > >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Ref< Light > >( *resource );
        }
    }
    void SceneLoader::sourceCamera( Params* params, Entity* entity, Json json )
    {
        Ref< Projection >* resource = params->assets->getAsset< Ref< Projection > >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Camera >({ .projection = (*resource).ref() });
        }
    }
    void SceneLoader::sourceCollider( Params* params, Entity* entity, Json json )
    {
        Ref< Collider >* resource = params->assets->getAsset< Ref< Collider > >( json[ "id" ].value() );
        if ( resource )
        {
            if ( !entity->has< Rigidbody >() )
            {
                entity->add< Rigidbody >();
            }
            entity->get< Rigidbody >()->collider = *resource;
        }
    }
    void SceneLoader::sourceMaterial( Params* params, Entity* entity, Json json )
    {
        Ref< Material >* resource = params->assets->getAsset< Ref< Material > >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Ref< Material > >( *resource );
        }
    }
    void SceneLoader::sourceSkeleton( Params* params, Entity* entity, Json json )
    {
//        Ref< ParticleEffect >* resource = assets->getAsset< Ref< ParticleEffect > >( json[ "id" ].value() );
//        if ( resource )
//        {
//            entity->add< Ref< ParticleEffect > >( *resource );
//        }
    }
    void SceneLoader::sourceMesh( Params* params, Entity* entity, Json json )
    {
        Ref< Mesh >* resource = params->assets->getAsset< Ref< Mesh > >( json[ "id" ].value() );
        if ( resource )
        {
            entity->add< Ref< Mesh > >( *resource );
        }
    }

    void SceneLoader::meta( Params* params, Entity* entity, Json json )
    {
    }

    void setRigidbodyCollider( Entity* entity, kege::Ref< kege::Collider > collider )
    {
        kege::Rigidbody* rigidbody = entity->get< kege::Rigidbody >();
        if ( rigidbody )
        {
            rigidbody->collider = collider;
        }
        else
        {
            entity->add< kege::Ref< kege::Collider > >( collider );
        }
    }

    void SceneLoader::colliderCircle( Params* params, Entity* entity, Json json )
    {
        Circle shape;
        shape.center = toVec3( json[ "center" ] );
        shape.right  = toVec3( json[ "right" ] );
        shape.normal = toVec3( json[ "normal" ] );
        shape.radius = json[ "center" ].getFloat();

        kege::Ref< kege::Collider > collider = new ColliderCircle( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::colliderMesh( Params* params, Entity* entity, Json json )
    {
    }

    void SceneLoader::colliderCone( Params* params, Entity* entity, Json json )
    {
        Cone shape;
        shape.height = json[ "height" ].getFloat();
        shape.direction = toVec3( json[ "direction" ] );
        shape.radius = json[ "center" ].getFloat();

        kege::Ref< kege::Collider > collider = new ColliderCone( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::colliderCylinder( Params* params, Entity* entity, Json json )
    {
        Cylinder shape;
        shape.height = json[ "height" ].getFloat();
        shape.center = toVec3( json[ "center" ] );
        shape.radius = json[ "center" ].getFloat();
        shape.axes[0] = toVec3( json[ "axes" ][ 0 ] );
        shape.axes[1] = toVec3( json[ "axes" ][ 1 ] );

        kege::Ref< kege::Collider > collider = new ColliderCylinder( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::colliderPlane( Params* params, Entity* entity, Json json )
    {
        Plane shape;
        shape.point = toVec3( json[ "point" ] );
        shape.distance = json[ "distance" ].getFloat();
        shape.normal = toVec3( json[ "normal" ][ 0 ] );
        shape.one_sided = json[ "one_sided" ].getBool();

        kege::Ref< kege::Collider > collider = new ColliderPlane( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::colliderSphere( Params* params, Entity* entity, Json json )
    {
        Sphere shape;
        shape.center = toVec3( json[ "center" ] );
        shape.radius = json[ "radius" ].getFloat();

        kege::Ref< kege::Collider > collider = new ColliderSphere( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::colliderBox( Params* params, Entity* entity, Json json )
    {
        OBB shape;
        shape.center = toVec3( json[ "center" ] );
        shape.extents = toVec3( json[ "extents" ] );
        shape.axes[0] = toVec3( json[ "axes" ][0] );
        shape.axes[1] = toVec3( json[ "axes" ][1] );
        shape.axes[2] = toVec3( json[ "axes" ][2] );

        kege::Ref< kege::Collider > collider = new ColliderBox( shape );

        if ( entity )
        {
            setRigidbodyCollider( entity, collider );
        }
        else
        {
            params->assets->add< kege::Ref< kege::Collider > >( params->id, collider );
        }
    }

    void SceneLoader::collider( Params* params, Entity* entity, Json json )
    {
        Json shape = json[ "shape" ];
        Json type = shape[ "type" ];
        ResourceParserFunctMap::iterator funct = _resource_parsers.find( type.value() );
        if ( funct != _resource_parsers.end() )
        {
            funct->second( params, entity, shape[ type.value() ] );

            Ref< Collider > collider;
            if ( entity )
            {
                if ( entity->get< Ref< Collider > >() )
                {
                    collider = *entity->get< Ref< Collider > >();
                }
                else if ( entity->get< Rigidbody >() )
                {
                    collider = entity->get< Rigidbody >()->collider;
                }
            }
            else
            {
                Ref< Collider >* res = params->assets->getAsset< Ref< Collider > >( params->id );
                if ( res )
                {
                    collider = *res;
                }
            }

            if ( collider != nullptr )
            {
                collider->friction = json[ "friction" ].getFloat();
                collider->cor = json[ "restitution" ].getFloat();
                collider->is_trigger = json[ "is_trigger" ].getBool();
            }
        }
    }

    Ref< Behavior > SceneLoader::airResistance( Json json )
    {
        if( !json ) return {};
        return new AirResistance
        (
            json[ "coefficient" ].getFloat(),
            json[ "fluid_density" ].getFloat(),
            json[ "use_quadratic_drag" ].getFloat()
        );
    }
    Ref< Behavior > SceneLoader::colorOverLifetime( Json json )
    {
        if( !json ) return {};
        Json json_gradient = json[ "gradient" ];
        Gradient gradient;
        for (int i=0; i<json.count(); ++i)
        {
            gradient.addKey( json_gradient[ "t" ].getFloat(), toVec4( json_gradient[ "color" ] ) );
        }
        return new ColorOverLifetime( gradient );
    }

    Ref< Behavior > SceneLoader::directionalGravity( Json json )
    {
        if( !json ) return {};
        return new DirectionalGravity( toVec3( json[ "force" ] ) );
    }

    Ref< Behavior > SceneLoader::centerOfMassGravity( Json json )
    {
        if( !json ) return {};
        return new CenterOfMassGravity
        (
            json[ "strength" ].getFloat(),
            json[ "falloff" ].getFloat()
        );
    }

    Ref< Behavior > SceneLoader::sizeOverLifetime( Json json )
    {
        if( !json ) return {};
        Json json_curve = json[ "curve" ];

        array< float > curve( json_curve.count() );
        for (int i=0; i<json.count(); ++i)
        {
            curve[i] = json_curve[ i ].getFloat();
        }
        return new SizeOverLifetime( curve );
    }

    Ref< Behavior > SceneLoader::velocityOverLifetime( Json json )
    {
        if( !json ) return {};
        return new AirResistance
        (
            json[ "coefficient" ].getFloat(),
            json[ "fluid_density" ].getFloat(),
            json[ "use_quadratic_drag" ].getFloat()
        );
    }

    Ref< Behavior > SceneLoader::windBehavior( Json json )
    {
        if( !json ) return {};
        return new AirResistance
        (
            json[ "coefficient" ].getFloat(),
            json[ "fluid_density" ].getFloat(),
            json[ "use_quadratic_drag" ].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterCone( Json json )
    {
        if( !json ) return {};

        float emissions_per_second = json[ "emissions_per_second" ].getFloat();
        bool burst = json[ "burst" ].getBool();
        kege::vec3 axes[2];
        axes[0] = toVec3( json[ "up" ] );
        axes[1] = toVec3( json[ "right" ] );
        float height = json[ "height" ].getFloat();
        float min_radius = json[ "min_radius" ].getFloat();
        float max_radius = json[ "max_radius" ].getFloat();

        return new EmitterCone
        (
            emissions_per_second,
            burst,
            axes,
            height,
            min_radius,
            max_radius
        );
    }

    Ref< Emitter > SceneLoader::emitterLine( Json json )
    {
        if( !json ) return {};

        float emissions_per_second = json[ "emissions_per_second" ].getFloat();
        bool burst = json[ "burst" ].getBool();
        kege::point3 start = toVec3( json[ "start" ] );
        kege::point3 end = toVec3( json[ "end" ] );

        return new EmitterLine
        (
            emissions_per_second,
            burst,
            start,
            end
        );
    }

    Ref< Emitter > SceneLoader::emitterCube( Json json )
    {
        if( !json ) return {};
        return new EmitterCube
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            json[ "width" ][0].getFloat(),
            json[ "height" ][0].getFloat(),
            json[ "depth" ][0].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterPlane( Json json )
    {
        if( !json ) return {};
        return new EmitterPlane
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            toVec3( json[ "a" ] ),
            toVec3( json[ "b" ] ),
            toVec3( json[ "c" ] ),
            toVec2( json[ "extents" ] )
        );
    }

    Ref< Emitter > SceneLoader::emitterCircle( Json json )
    {
        if( !json ) return {};

        kege::vec3 axis[2] =
        {
            toVec3( json[ "up" ] ),
            toVec3( json[ "right" ] )
        };

        return new EmitterCircleArea
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            axis,
            json[ "min-radius" ].getFloat(),
            json[ "max-radius" ].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterPyrimid( Json json )
    {
        if( !json ) return {};
        return new EmitterPyrimid
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            json[ "width" ].getFloat(),
            json[ "height" ].getFloat(),
            json[ "depth" ].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterSphere( Json json )
    {
        if( !json ) return {};
        return new EmitterSphereArea
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            json[ "min-radius" ].getFloat(),
            json[ "max-radius" ].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterTriangle( Json json )
    {
        if( !json ) return {};
        return new EmitterTriangle
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            toVec3( json[ "a" ] ),
            toVec3( json[ "b" ] ),
            toVec3( json[ "c" ] ),
            json[ "height" ].getFloat(),
            json[ "base" ].getFloat()
        );
    }

    Ref< Emitter > SceneLoader::emitterCylinder( Json json )
    {
        if( !json ) return {};
        kege::vec3 axes[2] =
        {
            toVec3( json[ "up" ] ),
            toVec3( json[ "right" ] )
        };

        return new EmitterCylinder
        (
            json[ "emissions_per_second" ].getFloat(),
            json[ "burst" ].getBool(),
            axes,
            json[ "height" ].getFloat(),
            json[ "min_radius" ].getFloat(),
            json[ "max_radius" ].getFloat()
        );
    }



    SceneLoader::SceneLoader()
    {
        _particle_behavior_creators[ "air-resistance" ] = airResistance;
        _particle_behavior_creators[ "color_over_lifetime" ] = colorOverLifetime;
        _particle_behavior_creators[ "directional_gravity" ] = directionalGravity;
        _particle_behavior_creators[ "center_of_mass_gravity" ] = centerOfMassGravity;
        _particle_behavior_creators[ "size_over_lifetime" ] = sizeOverLifetime;
        _particle_behavior_creators[ "velocity_over_lifetime" ] = velocityOverLifetime;
        _particle_behavior_creators[ "air_behavior" ] = windBehavior;

        _particle_emitter_creators[ "emitter-cone" ] = emitterCone;
        _particle_emitter_creators[ "emitter-line" ] = emitterLine;
        _particle_emitter_creators[ "emitter-cube" ] = emitterCube;
        _particle_emitter_creators[ "emitter-plane" ] = emitterPlane;
        _particle_emitter_creators[ "emitter-circle" ] = emitterCircle;
        _particle_emitter_creators[ "emitter-pyrimid" ] = emitterPyrimid;
        _particle_emitter_creators[ "emitter-sphere" ] = emitterSphere;
        _particle_emitter_creators[ "emitter-triangle" ] = emitterTriangle;
        _particle_emitter_creators[ "emitter-cylinder" ] =  emitterCylinder;

        _resource_parsers[ "source-mesh" ] = sourceMesh;
        _resource_parsers[ "source-collider" ] = sourceCollider;
        _resource_parsers[ "source-material" ] = sourceMaterial;
        _resource_parsers[ "source-light" ] = sourceLight;
        _resource_parsers[ "source-texture" ] = sourceTexture;
        _resource_parsers[ "source-effect" ] = sourceEffect;
        _resource_parsers[ "source-atmosphere" ] = sourceAtmosphere;
        _resource_parsers[ "source-terrain" ] = sourceTerrain;
        _resource_parsers[ "source-planet" ] = sourcePlanet;
        _resource_parsers[ "source-camera" ] = sourceCamera;
        _resource_parsers[ "source-skeleton" ] = sourceSkeleton;





        _resource_parsers[ "collider-mesh" ] = colliderMesh;
        _resource_parsers[ "collider-cont" ] = colliderCone;
        _resource_parsers[ "collider-circle" ] = colliderCircle;
        _resource_parsers[ "collider-cylinder" ] = colliderCylinder;
        _resource_parsers[ "collider-plane" ] = colliderPlane;
        _resource_parsers[ "collider-sphere" ] = colliderSphere;
        _resource_parsers[ "collider-box" ] = colliderBox;
        _resource_parsers[ "collider" ] = collider;

        _resource_parsers[ "mesh-box" ] = meshBox;
        _resource_parsers[ "mesh-cone" ] = meshCone;
        _resource_parsers[ "mesh-rect" ] = meshRect;
        _resource_parsers[ "mesh-grid" ] = meshGrid;
        _resource_parsers[ "mesh-sphere" ] = meshSphere;
        _resource_parsers[ "mesh-circle" ] = meshCircle;
        _resource_parsers[ "mesh-cylinder" ] = meshCylinder;
        _resource_parsers[ "mesh-icosahedron" ] = meshIcosahedron;
        _resource_parsers[ "mesh-path" ] = meshPath;

        _resource_parsers[ "transform" ] = transform;
        _resource_parsers[ "orthographic" ] = orthographic;
        _resource_parsers[ "perspective" ] = perspective;
        _resource_parsers[ "atmosphere" ] = atmosphere;
        _resource_parsers[ "rigidbody" ] = rigidbody;
        _resource_parsers[ "directional" ] = directional;
        _resource_parsers[ "point" ] = point;
        _resource_parsers[ "spot" ] = spot;
        //_resource_parsers[ "animation" ] = animation;
        _resource_parsers[ "skeleton" ] = skeleton;
        _resource_parsers[ "mesh" ] = mesh;
        _resource_parsers[ "camera-controller" ] = cameraController;
        _resource_parsers[ "movement-controller" ] = movementController;

        _resource_parsers[ "transform-component" ] = component;
        _resource_parsers[ "particle-effect-component" ] = component;
        _resource_parsers[ "atmosphere-component" ] = component;
        _resource_parsers[ "terrain-component" ] = component;
        _resource_parsers[ "planet-component" ] = component;
        _resource_parsers[ "light-component" ] = component;
        _resource_parsers[ "camera-component" ] = component;
        _resource_parsers[ "rigidbody-component" ] = component;
        _resource_parsers[ "collider-component" ] = component;
        _resource_parsers[ "material-component" ] = component;
        _resource_parsers[ "skeleton-component" ] = component;
        _resource_parsers[ "mesh-component" ] = component;
        _resource_parsers[ "movement-controller-component" ] = component;
        _resource_parsers[ "camera-controller-component" ] = component;

        _resource_parsers[ "components" ] = components;
        _resource_parsers[ "entity" ] = entity;

        _resource_parsers[ "entities" ] = resourceManager;
        _resource_parsers[ "animations" ] = resourceManager;
        _resource_parsers[ "materials" ] = resourceManager;
        _resource_parsers[ "textures" ] = resourceManager;
        _resource_parsers[ "effects" ] = resourceManager;
        _resource_parsers[ "cameras" ] = resourceManager;
        _resource_parsers[ "lights" ] = resourceManager;
        _resource_parsers[ "meshs" ] = resourceManager;
        _resource_parsers[ "meta" ] = meta;
    }
}
