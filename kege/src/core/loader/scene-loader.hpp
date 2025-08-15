//
//  scene-loader.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#ifndef scene_loader_hpp
#define scene_loader_hpp

#include "log.hpp"
#include "scene.hpp"
#include "entity-tag.hpp"
#include "mesh-cuboid.hpp"
#include "mesh-cone.hpp"
#include "mesh-rect.hpp"
#include "mesh-grid.hpp"
#include "mesh-circle.hpp"
#include "mesh-cylinder.hpp"
#include "mesh-ellipsoid.hpp"
#include "mesh-capsule.hpp"
#include "rigidbody.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "atmosphere.hpp"
#include "particle-effect.hpp"

namespace kege{

    class SceneLoader
    {
    public:

        struct Params
        {
            Scene* scene;
            AssetSystem* assets;
            std::string id;
            std::string type;
        };

        typedef std::function< Ref< Mesh >( AssetSystem* assets, Json json ) > MeshParserFunct;
        typedef std::map< std::string, MeshParserFunct > MeshParserFunctMap;

        typedef std::function< void( Params* params, Entity* entity, Json json ) > ResourceParserFunct;
        typedef std::map< std::string, ResourceParserFunct > ResourceParserFunctMap;

        typedef std::function< Ref< Behavior >( Json json ) > ParticleBehaviorFunct;
        typedef std::map< std::string, ParticleBehaviorFunct > ParticleBehaviorFunctMap;

        typedef std::function< Ref< Emitter >( Json json ) > ParticleEmitterFunct;
        typedef std::map< std::string, ParticleEmitterFunct > ParticleEmitterFunctMap;

        typedef std::function< Ref< Collider >( Json json ) > ColliderFunct;
        typedef std::map< std::string, ColliderFunct > ColliderFunctMap;

        static kege::Ref< kege::Scene > load( const std::string& filename );
        SceneLoader();

    private:

        static void resourceManager( Params* params, Entity* entity, Json json );
        static void meta( Params* params, Entity* entity, Json json );

        static void cameraController( Params* params, Entity* entity, Json json );
        static void movementController( Params* params, Entity* entity, Json json );
        static void terrain( Params* params, Entity* entity, Json json );
        static void planet( Params* params, Entity* entity, Json json );
        static void particleEffect( Params* params, Entity* entity, Json json );
        static void atmosphere( Params* params, Entity* entity, Json json );
        static void rigidbody( Params* params, Entity* entity, Json json );
        static void transform( Params* params, Entity* entity, Json json );
        static void orthographic( Params* params, Entity* entity, Json json );
        static void perspective( Params* params, Entity* entity, Json json );
        static void camera( Params* params, Entity* entity, Json json );
        static void directional( Params* params, Entity* entity, Json json );
        static void point( Params* params, Entity* entity, Json json );
        static void spot( Params* params, Entity* entity, Json json );
        static void skeleton( Params* params, Entity* entity, Json json );
        static void pbr( Params* params, Entity* entity, Json json );

        static void mesh( Params* params, Entity* entity, Json json );
        static void meshPath( Params* params, Entity* entity, Json json );
        static void meshBox( Params* params, Entity* entity, Json json );
        static void meshCone( Params* params, Entity* entity, Json json );
        static void meshRect( Params* params, Entity* entity, Json json );
        static void meshGrid( Params* params, Entity* entity, Json json );
        static void meshSphere( Params* params, Entity* entity, Json json );
        static void meshCircle( Params* params, Entity* entity, Json json );
        static void meshCylinder( Params* params, Entity* entity, Json json );
        static void meshIcosahedron( Params* params, Entity* entity, Json json );

        static void components( Params* params, Entity* entity, Json json );
        static void entities( Params* params, Entity* entity, Json json );

        static void component( Params* params, Entity* entity, Json json );
        static void entity( Params* params, Entity* entity, Json json );

        static void sourceEffect( Params* params, Entity* entity, Json json );
        static void sourceAtmosphere( Params* params, Entity* entity, Json json );
        static void sourceTerrain( Params* params, Entity* entity, Json json );
        static void sourcePlanet( Params* params, Entity* entity, Json json );
        static void sourceLight( Params* params, Entity* entity, Json json );
        static void sourceCamera( Params* params, Entity* entity, Json json );
        static void sourceCollider( Params* params, Entity* entity, Json json );
        static void sourceMaterial( Params* params, Entity* entity, Json json );
        static void sourceSkeleton( Params* params, Entity* entity, Json json );
        static void sourceMesh( Params* params, Entity* entity, Json json );
        static void sourceTexture( Params* params, Entity* entity, Json json );

        static void colliderCircle( Params* params, Entity* entity, Json json );
        static void colliderMesh( Params* params, Entity* entity, Json json );
        static void colliderCone( Params* params, Entity* entity, Json json );
        static void colliderCylinder( Params* params, Entity* entity, Json json );
        static void colliderPlane( Params* params, Entity* entity, Json json );
        static void colliderSphere( Params* params, Entity* entity, Json json );
        static void colliderBox( Params* params, Entity* entity, Json json );
        static void collider( Params* params, Entity* entity, Json json );

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
