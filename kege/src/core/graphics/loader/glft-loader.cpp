//
//  glft-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/7/25.
//

#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../../../libs/third-party/tiny-gltf/tiny_gltf.h"
#include "glft-loader.hpp"
#include <vector>
#include "mesh.hpp"
#include "vmath.hpp"
#include "ecs.hpp"
#include "camera.hpp"


namespace kege::gltf{

    static std::string getFilePathExtension(const std::string &FileName)
    {
        if (FileName.find_last_of(".") != std::string::npos)
            return FileName.substr(FileName.find_last_of(".") + 1);
        return "";
    }

    std::vector< kege::vec4 > loadAccessorVec4(const tinygltf::Model &model, int accessor_index)
    {
        std::vector< kege::vec4 > out;

        const tinygltf::Accessor &accessor   = model.accessors[ accessor_index ];
        const tinygltf::BufferView &view     = model.bufferViews[ accessor.bufferView ];
        const tinygltf::Buffer &buffer       = model.buffers[ view.buffer ];

        // Number of components and size per component:
        int num_comp = tinygltf::GetNumComponentsInType( accessor.type );       // should be 3 for VEC3
        int comp_size = tinygltf::GetComponentSizeInBytes( accessor.componentType ); // FLOAT=4 bytes
        size_t stride = view.byteStride ? view.byteStride : num_comp * comp_size;

        // Pointer to first element:
        const unsigned char *data_ptr = buffer.data.data() + view.byteOffset + accessor.byteOffset;

        out.resize(accessor.count);
        for (size_t i = 0; i < accessor.count; ++i)
        {
            // This assumes floats; you can branch on accessor.componentType if needed.
            const float *v = reinterpret_cast<const float *>( data_ptr + i * stride );
            out[i] = kege::vec4( v[0], v[1], v[2], v[3] );
        }

        return out;
    }

    std::vector< kege::vec3 > loadAccessorVec3(const tinygltf::Model &model, int accessor_index)
    {
        std::vector< kege::vec3 > out;

        const tinygltf::Accessor &accessor   = model.accessors[ accessor_index ];
        const tinygltf::BufferView &view     = model.bufferViews[ accessor.bufferView ];
        const tinygltf::Buffer &buffer       = model.buffers[ view.buffer ];

        // Number of components and size per component:
        int num_comp = tinygltf::GetNumComponentsInType( accessor.type );       // should be 3 for VEC3
        int comp_size = tinygltf::GetComponentSizeInBytes( accessor.componentType ); // FLOAT=4 bytes
        size_t stride = view.byteStride ? view.byteStride : num_comp * comp_size;

        // Pointer to first element:
        const unsigned char *data_ptr = buffer.data.data() + view.byteOffset + accessor.byteOffset;

        out.resize(accessor.count);
        for (size_t i = 0; i < accessor.count; ++i)
        {
            // This assumes floats; you can branch on accessor.componentType if needed.
            const float *v = reinterpret_cast<const float *>( data_ptr + i * stride );
            out[i] = kege::vec3( v[0], v[1], v[2] );
        }

        return out;
    }

    std::vector< kege::vec2 > loadAccessorVec2(const tinygltf::Model &model, int accessor_index)
    {
        std::vector< kege::vec2 > out;

        const tinygltf::Accessor &accessor   = model.accessors[ accessor_index ];
        const tinygltf::BufferView &view     = model.bufferViews[ accessor.bufferView ];
        const tinygltf::Buffer &buffer       = model.buffers[ view.buffer ];

        // Number of components and size per component:
        int num_comp = tinygltf::GetNumComponentsInType( accessor.type );       // should be 3 for VEC3
        int comp_size = tinygltf::GetComponentSizeInBytes( accessor.componentType ); // FLOAT=4 bytes
        size_t stride = view.byteStride ? view.byteStride : num_comp * comp_size;

        // Pointer to first element:
        const unsigned char *data_ptr = buffer.data.data() + view.byteOffset + accessor.byteOffset;

        out.resize(accessor.count);
        for (size_t i = 0; i < accessor.count; ++i)
        {
            // This assumes floats; you can branch on accessor.componentType if needed.
            const float *v = reinterpret_cast<const float *>( data_ptr + i * stride );
            out[i] = kege::vec2( v[0], v[1] );
        }
        return out;
    }

    std::vector<uint32_t> loadIndices( const tinygltf::Model &model, int accessor_index )
    {
        std::vector<uint32_t> out;

        const tinygltf::Accessor   &accessor = model.accessors[ accessor_index ];
        const tinygltf::BufferView &view     = model.bufferViews[ accessor.bufferView ];
        const tinygltf::Buffer     &buffer   = model.buffers[ view.buffer ];

        size_t stride = view.byteStride ? view.byteStride : tinygltf::GetComponentSizeInBytes( accessor.componentType );
        const unsigned char *data_ptr = buffer.data.data() + view.byteOffset + accessor.byteOffset;

        out.resize(accessor.count);
        switch (accessor.componentType)
        {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            {
                const uint8_t *p = reinterpret_cast< const uint8_t* >( data_ptr );
                for (size_t i = 0; i < accessor.count; ++i)
                    out[i] = p[i * stride / sizeof(uint8_t)];
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            {
                const uint16_t *p = reinterpret_cast< const uint16_t* >( data_ptr );
                for (size_t i = 0; i < accessor.count; ++i)
                    out[i] = p[i * stride / sizeof(uint16_t)];
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            {
                const uint32_t *p = reinterpret_cast< const uint32_t* >( data_ptr );
                for (size_t i = 0; i < accessor.count; ++i)
                    out[i] = p[i * stride / sizeof(uint32_t)];
                break;
            }
            default:
                // glTF only allows unsigned indices, so this should never happen
                throw std::runtime_error("Unsupported index component type");
        }
        return out;
    }

    kege::array< ref::Mesh > parseMesh( tinygltf::Model& model, tinygltf::Mesh& mesh )
    {
        //int submesh_index = 0;
        kege::array< ref::Mesh > mesh_set;
        mesh_set.resize( mesh.primitives.size() );

        for (int i=0; i<mesh.primitives.size(); ++i)
        {
            const auto &primitive = mesh.primitives[i];

            // POSITION
            auto pos_acc_index = primitive.attributes.at( "POSITION" );
            std::vector< kege::vec3 > positions = loadAccessorVec3( model, pos_acc_index );

            // NORMAL (optional)
            std::vector< kege::vec3 > normals;
            auto it_normal = primitive.attributes.find("NORMAL");
            if (it_normal != primitive.attributes.end())
            {
                normals = loadAccessorVec3(model, it_normal->second);
            }

            // TEXCOORD_0 (optional)
            std::vector< kege::vec2 > texcoords;
            auto itTex = primitive.attributes.find("TEXCOORD_0");
            if (itTex != primitive.attributes.end())
            {
                texcoords = loadAccessorVec2( model, itTex->second );
            }

            // TANGENT (optional)
            std::vector< kege::vec4 > tangents;
            auto it_tangent = primitive.attributes.find("TANGENT");
            if (it_tangent != primitive.attributes.end())
            {
                tangents = loadAccessorVec4( model, it_tangent->second );
            }

            Ref< MeshPrimitive > mesh_primitive = new MeshPrimitive;

            // Indices
            if (primitive.indices >= 0)
            {
                mesh_primitive->indices = loadIndices( model, primitive.indices );
            }

            for ( int i=0; i<positions.size(); ++i)
            {
                mesh_primitive->vertices[i].position = positions[i];
                if ( !normals.empty()   ) mesh_primitive->vertices[i].normal   = normals[i];
                if ( !texcoords.empty() ) mesh_primitive->vertices[i].texcoord = texcoords[i];
                if ( !tangents.empty()  ) mesh_primitive->vertices[i].tangent  = tangents[i].xyz;

                if ( !tangents.empty() && !normals.empty() )
                {
                    mesh_primitive->vertices[i].bitangent = cross( normalize( tangents[i].xyz ), normalize( tangents[i].xyz )) * tangents[i].w;
                }
            }
            mesh_set.at(i) = new kege::Mesh
            {
                mesh_primitive, 1, 0, 0,
                static_cast<uint32_t>(mesh_primitive->indices.size())
            };
        }
        return mesh_set;
    }

    Camera parseCamera( tinygltf::Model& model, tinygltf::Camera& camera )
    {
        Camera cam = {};
        if ( camera.type == "perspective" )
        {
            cam.projection = new Perspective
            (
                camera.perspective.aspectRatio,
                camera.perspective.yfov,
                camera.perspective.znear,
                camera.perspective.zfar
            );
        }
        else if ( camera.type == "orthographic" )
        {
            cam.projection = new Orthographic
            (
                0, camera.orthographic.xmag,
                0, camera.orthographic.ymag,
                camera.orthographic.znear,
                camera.orthographic.zfar
            );
        }
        return cam;
    }

    int load( kege::ECS* ecs, const std::string& filename )
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF gltf_ctx;
        std::string err;
        std::string warn;
        std::string ext = getFilePathExtension( filename );

        bool ret = false;
        if ( ext.compare("glb") == 0 )
        {
            //std::cout << "Reading binary glTF" << std::endl;
            // assume binary glTF.
            ret = gltf_ctx.LoadBinaryFromFile( &model, &err, &warn, filename.c_str() );
        }
        else
        {
            //std::cout << "Reading ASCII glTF" << std::endl;
            // assume ascii glTF.
            ret = gltf_ctx.LoadASCIIFromFile( &model, &err, &warn, filename.c_str() );
        }

        if ( !warn.empty() )
        {
            printf("Warn: %s\n", warn.c_str());
        }

        if ( !err.empty() )
        {
            printf("Err: %s\n", err.c_str());
        }

        if ( !ret )
        {
            printf("Failed to parse glTF\n");
            return -1;
        }

        //Dump(model);

        for (size_t i = 0; i < model.scenes.size(); i++)
        {
            for (size_t j = 0; j < model.scenes[i].nodes.size(); i++)
            {
                int node_id = model.scenes[i].nodes[j];
                tinygltf::Node& node = model.nodes[ node_id ];

                ecs::Entity entity = ecs->create();


                if ( node.translation.empty() )
                {
                    Transform* transform = ecs->add< Transform >( entity );
                    transform->position.x = node.translation[0];
                    transform->position.y = node.translation[1];
                    transform->position.z = node.translation[2];
                }
                if ( node.rotation.empty() )
                {
                    Transform* transform = ecs->add< Transform >( entity );
                    transform->orientation.x = node.rotation[0];
                    transform->orientation.y = node.rotation[1];
                    transform->orientation.z = node.rotation[2];
                    transform->orientation.w = node.rotation[3];
                }
                if ( node.scale.empty() )
                {
                    Transform* transform = ecs->add< Transform >( entity );
                    transform->scale.x = node.scale[0];
                    transform->scale.y = node.scale[1];
                    transform->scale.z = node.scale[2];
                }


                if ( 0 <= node.camera )
                {
                    *ecs->add< Camera >( entity ) = parseCamera( model, model.cameras[ node.camera ] );
                }

                if ( 0 <= node.mesh )
                {
                    kege::array< ref::Mesh > mesh = parseMesh( model, model.meshes[ node.mesh ] );
                    *ecs->add< kege::array< ref::Mesh > >( entity ) = mesh;
                }

                if ( 0 <= node.skin )
                {
                    //tinygltf::Skin& skin = model.skins[ node.skin ];
                }

                if ( 0 <= node.light )
                {
                    //tinygltf::Light& light = model.lights[ node.light ];
                }
            }

          //std::cout << Indent(1) << "scene[" << i << "] name  : " << model.scenes[i].name << std::endl;
          //DumpExtensions(model.scenes[i].extensions, 1);
        }
        return 0;
    }

}
