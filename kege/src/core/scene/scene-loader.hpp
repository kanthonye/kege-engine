//
//  scene-loader.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#ifndef scene_loader_hpp
#define scene_loader_hpp

#include "../utils/log.hpp"
#include "../math/algebra/vmath.hpp"
#include "../utils/json-parser.hpp"

#include "../graphics/components/all.hpp"

#include "../physics/3d/bodies/rigidbody.hpp"
#include "../physics/3d/bodies/rigid-shapes.hpp"


#include "../scene/ecs.hpp"
#include "scene.hpp"

// #include "rigid-shapes.hpp"

// #include "mesh-cuboid.hpp"
// #include "mesh-circle.hpp"
// #include "mesh-grid.hpp"
// #include "mesh-rect.hpp"
// #include "mesh-cylinder.hpp"
// #include "mesh-ellipsoid.hpp"
// #include "mesh-icosahedron.hpp"

// #include "mesh-loader.hpp"
// #include "material-loader.hpp"


namespace kege{

    class SceneLoader
    {
    public:

        struct Params
        {
            Scene* scene;
            AssetManager* assets;
            std::string id;
            std::string type;


//            Json asset;
//            Json scene;
//            Json scenes;
//            Json entities;
//            Json component_types;
//            Json components;
//            Json noise_generators;
//            Json terrain_layouts;
//            Json biomes;
//            Json textures;
//            Json systems;
//            Json meshs;
//            Json buffers;
//            Json buffer_views;
//            Json accessors;
        };

        typedef std::function< Ref< Mesh >( AssetManager* assets, Json json ) > MeshParserFunct;
        typedef std::map< std::string, MeshParserFunct > MeshParserFunctMap;

        typedef std::function< void( Params* params, ecs::Entity& entity, Json json ) > ResourceParserFunct;
        typedef std::map< std::string, ResourceParserFunct > ResourceParserFunctMap;

        typedef std::function< Ref< Behavior >( Json json ) > ParticleBehaviorFunct;
        typedef std::map< std::string, ParticleBehaviorFunct > ParticleBehaviorFunctMap;

        typedef std::function< Ref< Emitter >( Json json ) > ParticleEmitterFunct;
        typedef std::map< std::string, ParticleEmitterFunct > ParticleEmitterFunctMap;

        typedef std::function< Ref< Collider >( Json json ) > ColliderFunct;
        typedef std::map< std::string, ColliderFunct > ColliderFunctMap;


        typedef std::function< void( kege::ECS& ecs, ecs::Entity&, Json ) > EntityComponentParser;
        typedef std::map< std::string, EntityComponentParser > EntityComponentParsers;


        static ecs::Entity parseEntity( kege::ECS& ecs, kege::Json& entities, kege::Json& components, int entity_index );
        static kege::Ref< kege::Scene > load
        (
            kege::ECS& ecs,
            kege::RenderGraph* rg,
            kege::AssetManager* am,
            const std::string& filename
        );
        SceneLoader();

    private:

        static void getAssetManager( Params* params, ecs::Entity& entity, Json json );
        static void meta( Params* params, ecs::Entity& entity, Json json );

        static void cameraController( Params* params, ecs::Entity& entity, Json json );
        static void movementController( Params* params, ecs::Entity& entity, Json json );
        static void terrain( Params* params, ecs::Entity& entity, Json json );
        static void planet( Params* params, ecs::Entity& entity, Json json );
        static void particleEffect( Params* params, ecs::Entity& entity, Json json );
        static void atmosphere( Params* params, ecs::Entity& entity, Json json );
        static void rigidbody( Params* params, ecs::Entity& entity, Json json );
        static void transform( Params* params, ecs::Entity& entity, Json json );
        static void orthographic( Params* params, ecs::Entity& entity, Json json );
        static void perspective( Params* params, ecs::Entity& entity, Json json );
        static void camera( Params* params, ecs::Entity& entity, Json json );
        static void directional( Params* params, ecs::Entity& entity, Json json );
        static void point( Params* params, ecs::Entity& entity, Json json );
        static void spot( Params* params, ecs::Entity& entity, Json json );
        static void skeleton( Params* params, ecs::Entity& entity, Json json );
        static void pbr( Params* params, ecs::Entity& entity, Json json );

        static void mesh( Params* params, ecs::Entity& entity, Json json );
        static void meshPath( Params* params, ecs::Entity& entity, Json json );
        static void meshBox( Params* params, ecs::Entity& entity, Json json );
        static void meshCone( Params* params, ecs::Entity& entity, Json json );
        static void meshRect( Params* params, ecs::Entity& entity, Json json );
        static void meshGrid( Params* params, ecs::Entity& entity, Json json );
        static void meshSphere( Params* params, ecs::Entity& entity, Json json );
        static void meshCircle( Params* params, ecs::Entity& entity, Json json );
        static void meshCylinder( Params* params, ecs::Entity& entity, Json json );
        static void meshIcosahedron( Params* params, ecs::Entity& entity, Json json );

        static void components( Params* params, ecs::Entity& entity, Json json );
        static void entities( Params* params, ecs::Entity& entity, Json json );

        static void component( Params* params, ecs::Entity& entity, Json json );
        static void entity( Params* params, ecs::Entity& entity, Json json );

        static void sourceEffect( Params* params, ecs::Entity& entity, Json json );
        static void sourceAtmosphere( Params* params, ecs::Entity& entity, Json json );
        static void sourceTerrain( Params* params, ecs::Entity& entity, Json json );
        static void sourcePlanet( Params* params, ecs::Entity& entity, Json json );
        static void sourceLight( Params* params, ecs::Entity& entity, Json json );
        static void sourceCamera( Params* params, ecs::Entity& entity, Json json );
        static void sourceCollider( Params* params, ecs::Entity& entity, Json json );
        static void sourceMaterial( Params* params, ecs::Entity& entity, Json json );
        static void sourceSkeleton( Params* params, ecs::Entity& entity, Json json );
        static void sourceMesh( Params* params, ecs::Entity& entity, Json json );
        static void sourceTexture( Params* params, ecs::Entity& entity, Json json );

        static void colliderCircle( Params* params, ecs::Entity& entity, Json json );
        static void colliderMesh( Params* params, ecs::Entity& entity, Json json );
        static void colliderCone( Params* params, ecs::Entity& entity, Json json );
        static void colliderCylinder( Params* params, ecs::Entity& entity, Json json );
        static void colliderPlane( Params* params, ecs::Entity& entity, Json json );
        static void colliderSphere( Params* params, ecs::Entity& entity, Json json );
        static void colliderBox( Params* params, ecs::Entity& entity, Json json );
        static void collider( Params* params, ecs::Entity& entity, Json json );

        static Ref< Behavior > airResistance( Json json );
        static Ref< Behavior > colorOverLifetime( Json json );
        static Ref< Behavior > directionalGravity( Json json );
        static Ref< Behavior > centerOfMassGravity( Json json );
        static Ref< Behavior > sizeOverLifetime( Json json );
        static Ref< Behavior > velocityOverLifetime( Json json );
        static Ref< Behavior > windBehavior( Json json );

        static Ref< Emitter > emitterCone( Json json );
        static Ref< Emitter > emitterLine( Json json );
        static Ref< Emitter > emitterCube( Json json );
        static Ref< Emitter > emitterPlane( Json json );
        static Ref< Emitter > emitterCircle( Json json );
        static Ref< Emitter > emitterPyrimid( Json json );
        static Ref< Emitter > emitterSphere( Json json );
        static Ref< Emitter > emitterTriangle( Json json );
        static Ref< Emitter > emitterCylinder( Json json );

    private:

        static EntityComponentParsers _entity_component_parsers;

        static MeshParserFunctMap _mesh_parser_funct_map;
        static ParticleBehaviorFunctMap _particle_behavior_creators;
        static ParticleEmitterFunctMap _particle_emitter_creators;
        static ColliderFunctMap _collider_funct_map;
        static ResourceParserFunctMap _resource_parsers;
    };


    inline double  stoF( const char* s){ return atof(s);  }
    inline float   stof( const char* s){ return atof(s);  }
    inline int64_t stoI( const char* s){ return atoll(s); }
    inline int32_t stoi( const char* s){ return atoi(s);  }

    inline vec2 toVec2( Json json )
    {
        arr< float, 2 > a = json.getArray< float, 2 >( stof );
        return vec2( a.data[0], a.data[1] );
    }
    inline vec3 toVec3( Json json )
    {
        arr< float, 3 > a = json.getArray< float, 3 >( stof );
        return vec3( a.data[0], a.data[1], a.data[2] );
    }
    inline vec4 toVec4( Json json )
    {
        arr< float, 4 > a = json.getArray< float, 4 >( stof );
        return vec4( a.data[0], a.data[1], a.data[2], a.data[3] );
    }
    inline quat toQuat( Json json )
    {
        arr< float, 4 > a = json.getArray< float, 4 >( stof );
        return quat( a.data[0], a.data[1], a.data[2], a.data[3] );
    }

}

#endif /* scene_loader_hpp */
