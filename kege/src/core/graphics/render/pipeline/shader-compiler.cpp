//
//  shader-compiler.cpp
//  assets
//
//  Created by Kenneth Esdaile on 2/11/26.
//

#include "shader-compiler.hpp"

namespace kege{

    ShaderProgram* ShaderCompiler::compileVariant(const kege::PipelineKey& key)
    {
        set_index = 0;
        binding_index = 0;

        std::vector<std::string> defines;
        writeFeatures(key, defines);

        std::vector<VertexInfo> outputs;
        std::string vs = writeVertexShader(key, defines, outputs);
        std::string fs = writeFragmentShader(key, outputs, defines);
        return compileShader(key, vs, fs);
    }

    ShaderProgram* ShaderCompiler::getVariant(const kege::PipelineKey& key)
    {
        //auto it = _variant_cache.find(key);
        //if (it != _variant_cache.end())
        //    return it->second;

        // Compile new variant (expensive!)
        ShaderProgram* variant = compileVariant(key);
        //_variant_cache[key] = variant;
        return variant;
    }

    std::string ShaderCompiler::writeVertexShader
    (
     const kege::PipelineKey& key,
     const std::vector<std::string>& global_defines,
     std::vector<VertexInfo>& outputs
    )
    {
        std::vector<std::string> defines = global_defines;

        buildVertexLayout(key, outputs);
        defineVertexAttributeIO(key, outputs, defines);

        std::stringstream source;
        writeVersionAndExtensions(source);
        writeDefines(source, defines);
        writeVertexProcessingSelection(source, key);
        writeCameraData(source, key);
        writeObjectData(source, key);
        writeVertexShaderMainFunction(source, key);
        return source.str();
    }

    std::string ShaderCompiler::writeFragmentShader
    (
     const kege::PipelineKey& key,
     const std::vector<VertexInfo>& inputs,
     const std::vector<std::string>& global_defines
    )
    {
        std::vector<std::string> defines = global_defines;
        std::stringstream source;
        writeVersionAndExtensions(source);
        writeDefines(source, defines);
        writeFragmentShaderInput(source, inputs);
        writeFragmentShaderIO(source, key);
        writeCameraData(source, key);
        writeObjectData(source, key);
        writeLightingSelection(source, key);
        writeFragmentShaderMainFunction(source, key);
        return source.str();
    }

    ShaderProgram* ShaderCompiler::compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs)
    {
        std::cout << vs <<"\n";
        std::cout <<"\n--------- --------- --------- --------- --------- --------- ---------\n\n";
        std::cout << fs <<"\n";

//            ref::Shader vs_module = _graphics->createShader
//            ({
//                .name = "vertex-shader",
//                .entry_point = "main",
//                .stage = kege::ShaderStageFlag::Vertex,
//                .byte_code = _graphics->compileGlslToSpv(kege::ShaderStageFlag::Vertex, "", vs.data(), vs.length())
//            });
//
//            ref::Shader fs_module = _graphics->createShader
//            ({
//                .name = "fragment-shader",
//                .entry_point = "main",
//                .stage = kege::ShaderStageFlag::Fragment,
//                .byte_code = _graphics->compileGlslToSpv(kege::ShaderStageFlag::Fragment, "", vs.data(), vs.length())
//            });
//
//            ref::ShaderLayout shader_layout = _graphics->createShaderLayout
//            ({
//                .name = "shader-layout",
//                .push_blocks = {},
//                .bind_sets = {},
//            });

        return nullptr;
    }

    std::string ShaderCompiler::include(const std::string& fname )
    {
        const std::string filename = "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/generate" + fname;
        std::ifstream file(filename);
        std::string loaded_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return loaded_source;
    }

    void ShaderCompiler::defineVertexAttributeIO
    (
     const kege::PipelineKey& key,
     const std::vector<VertexInfo>& attributes,
     std::vector<std::string>& defines
    )
    {
        int in_location = 0;
        int out_location = 0;
        for(const VertexInfo& info : attributes)
        {
            switch (info.semantic)
            {
                case VertexInfo::POSITION:
                    defines.push_back("VERTEX_POSITION " + std::to_string(in_location++));
                    defines.push_back("VSOUT_POSITION " + std::to_string(out_location++));
                    break;

                case VertexInfo::TEXCOORD:
                    defines.push_back("VERTEX_TEXCOORD " + std::to_string(in_location++));
                    defines.push_back("VSOUT_TEXCOORD " + std::to_string(out_location++));
                    break;

                case VertexInfo::NORMAL:
                    defines.push_back("VERTEX_NORMAL " + std::to_string(in_location++));
                    defines.push_back("VSOUT_NORMAL " + std::to_string(out_location++));
                    break;

                case VertexInfo::COLOR:
                    defines.push_back("VERTEX_COLOR " + std::to_string(in_location++));
                    defines.push_back("VSOUT_COLOR " + std::to_string(out_location++));
                    break;

                case VertexInfo::TANGENT:
                    defines.push_back("VERTEX_TANGENT " + std::to_string(in_location++));
                    defines.push_back("VSOUT_TANGENT " + std::to_string(out_location++));
                    break;

                case VertexInfo::BITANGENT:
                    defines.push_back("VERTEX_BITANGENT " + std::to_string(in_location++));
                    defines.push_back("VSOUT_BITANGENT " + std::to_string(out_location++));
                    break;

                case VertexInfo::JOINTS:
                    defines.push_back("VERTEX_JOINTS " + std::to_string(in_location++));
                    defines.push_back("VSOUT_JOINTS " + std::to_string(out_location++));
                    break;

                case VertexInfo::WEIGHTS:
                    defines.push_back("VERTEX_WEIGHTS " + std::to_string(in_location++));
                    defines.push_back("VSOUT_WEIGHTS " + std::to_string(out_location++));
                    break;

                default: break;
            }
        }
    }

    void ShaderCompiler::buildVertexLayout(const kege::PipelineKey& key, std::vector<VertexInfo>& attributes)
    {
        int location = 0;

        if (key.renderer_type == kege::RendererType::StaticMesh ||
            //key.renderer_type == kege::RendererType::SkinnedMesh ||
            key.renderer_type == kege::RendererType::Particle)
        {
            attributes.push_back({ VertexInfo::POSITION, location++ });
        }
        if (key.renderer_type == kege::RendererType::StaticMesh ||
            //key.renderer_type == kege::RendererType::SkinnedMesh ||
            key.renderer_type == kege::RendererType::Particle)
        {
            attributes.push_back({ VertexInfo::TEXCOORD, location++ });
        }
//        if (key.renderer_type == kege::RendererType::StaticMesh ||
//            key.renderer_type == kege::RendererType::SkinnedMesh)
//        {
//            attributes.push_back({ VertexInfo::NORMAL, location++ });
//        }
        if (key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            attributes.push_back({ VertexInfo::TANGENT, location++ });
            attributes.push_back({ VertexInfo::BITANGENT, location++ });
        }
//        if (key.renderer_type == kege::RendererType::SkinnedMesh ||
//            key.features & kege::FeatureFlag::GPU_SKINNING)
//        {
//            attributes.push_back({ VertexInfo::JOINTS, location++ });
//            attributes.push_back({ VertexInfo::WEIGHTS, location++ });
//        }
        if (key.features == kege::FeatureFlag::VERTEX_COLOR)
        {
            attributes.push_back({ VertexInfo::COLOR, location++ });
        }
    }

    void ShaderCompiler::writeFragmentShaderInput(std::stringstream& source, const std::vector<VertexInfo>& inputs)
    {
        for (const auto& input : inputs)
        {
            switch (input.semantic)
            {
                case VertexInfo::POSITION:
                    source << "layout(location = \"" << input.location <<"\") in vec3 _world_position;\n";
                    break;

                case VertexInfo::TEXCOORD:
                    source << "layout(location = \"" << input.location <<"\") in vec2 _texcoord;\n";
                    break;

                case VertexInfo::NORMAL:
                    source << "layout(location = \"" << input.location <<"\") in vec3 _world_normal;\n";
                    break;

                case VertexInfo::COLOR:
                    source << "layout(location = \"" << input.location <<"\") in vec4 _color;\n";
                    break;

                case VertexInfo::TANGENT:
                    source << "layout(location = \"" << input.location <<"\") in vec3 _world_tangent;\n";
                    break;

                case VertexInfo::BITANGENT:
                    source << "layout(location = \"" << input.location <<"\") in vec3 _world_bitangent;\n";
                    break;

                default: break;
            }
        }
    }

    void ShaderCompiler::writeFragmentShaderIO(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// Render pass\n";
        switch (key.render_pass)
        {
          case kege::RenderPass::GBuffer: // Multiple render targets for GBuffer
              source << "layout(location = 0) out vec4 outGBuffer0; // Albedo + MaterialID\n";
              source << "layout(location = 1) out vec4 outGBuffer1; // Normal + Roughness\n";
              source << "layout(location = 2) out vec4 outGBuffer2; // Metallic + Specular + AO\n";
              break;

          case kege::RenderPass::Forward:
          case kege::RenderPass::UI: // Single render target
              source << "layout(location = 0) out vec4 out_color;\n";
              break;

          case kege::RenderPass::Shadow: // Depth only (might have no color output)
              if (!(key.features & kege::FeatureFlag::DEPTH_ONLY))
              {
                  source << "layout(location = 0) out vec4 outShadow;\n";
              }
              break;

          default: break;
      }
      source << "\n";
    }

    void ShaderCompiler::writeVertexShaderIO(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// Vertex outputs\n";
        //source << "layout(location = 0) out vec3 outWorldPosition;\n";

        int location = 0;
//        if (key.renderer_type == kege::RendererType::StaticMesh ||
//            key.renderer_type == kege::RendererType::SkinnedMesh ||
//            key.renderer_type == kege::RendererType::Particle)
//        {
//            source << "layout(location = \"" << location++ <<"\") out vec3 _world_position;\n";
//        }
//        if (key.renderer_type == kege::RendererType::StaticMesh ||
//            key.renderer_type == kege::RendererType::SkinnedMesh)
//        {
//            source << "layout(location = \"" << location++ <<"\") out vec3 _world_normal;\n";
//        }
//        if (key.renderer_type == kege::RendererType::StaticMesh ||
//            key.renderer_type == kege::RendererType::SkinnedMesh ||
//            key.renderer_type == kege::RendererType::Particle)
//        {
//            source << "layout(location = \"" << location++ <<"\") out vec2 _texcoord;\n";
//        }
        if (key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            source << "layout(location = \"" << location++ <<"\") out vec3 _tangent;\n";
            source << "layout(location = \"" << location++ <<"\") out vec3 _bitangent;\n";
        }
        if (key.features == kege::FeatureFlag::VERTEX_COLOR)
        {
            source << "layout(location = \"" << location++ <<"\") out vec4 _color;\n";
        }
        source << "\n";
    }

    void ShaderCompiler::writeCameraData(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// Uniform buffer CameraData\n";
        source << include("/common/camera-data.glsl");
        source << "\n";
    }

    void ShaderCompiler::writeObjectData(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// Uniform buffer ObjectData\n";
        source << include("/common/object-data.glsl");
        source << "\n";
    }

    void ShaderCompiler::writeDefines(std::stringstream& source, const std::vector<std::string>& defines)
    {
        for (const auto& define : defines)
        {
            source << "#define " << define << "\n";
        }
        source << "\n";
    }

    void ShaderCompiler::writeVertexProcessingSelection(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// Vertex processing\n";
        switch (key.renderer_type)
        {
            default:
            case kege::RendererType::StaticMesh:
                source << include("/vertex/static.glsl");
                break;
//            case kege::RendererType::SkinnedMesh:
//                source << include("/vertex/skinning.glsl");
//                break;
            case kege::RendererType::Particle:
                source << include("/vertex/billboard-particles.glsl");
                break;
        }
        source << "\n";
    }

    void ShaderCompiler::writeLightingSelection(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "// BRDF includes\n";
//        switch (key.shading_model)
//        {
//            default:
//            case kege::ShadingModel::Unlit:
//                source << include("/brdf/unlit.glsl");
//                break;
//            case kege::ShadingModel::LitPBR_Metallic:
//                source << include("/common/lights.glsl");
//                source << include("/common/material.glsl");
//                source << include("/brdf/brdf.glsl");
//                source << include("/brdf/pbr-metallic.glsl");
//                break;
//            case kege::ShadingModel::LitPBR_Specular:
//                source << include("/common/lights.glsl");
//                source << include("/common/material.glsl");
//                source << include("/brdf/brdf.glsl");
//                source << include("/brdf/pbr-specular.glsl");
//                break;
//            case kege::ShadingModel::LitPBR_Subsurface:
//                source << include("/common/lights.glsl");
//                source << include("/common/material.glsl");
//                source << include("/brdf/brdf.glsl");
//                source << include("/brdf/pbr-subsurface.glsl");
//                break;
//        }
        source << "\n";
    }

    void ShaderCompiler::writeVertexShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key)
    {
        source << "void main() {\n";
        source << "    // Process vertex based on renderer type\n";

        source << "    _world_position = processVertexPosition();\n";

        switch (key.renderer_type)
        {
//            case kege::RendererType::SkinnedMesh:
//                source << "    _world_normal = processVertexNormal();\n";
//                source << "    _texcoord = processVertexTexcoord();\n";
//                break;

            case kege::RendererType::StaticMesh:
                source << "    _world_normal = processVertexNormal();\n";
                source << "    _texcoord = processVertexTexcoord();\n";
                break;

            case kege::RendererType::Particle:
                source << "    _texcoord = processVertexTexcoord();\n";
                break;

            case kege::RendererType::ScreenRect:
                source << "    _texcoord = processVertexTexcoord();\n";
                break;

            default:
                source << "    _world_position = in_position;\n";
        }

        if ((key.features & kege::FeatureFlag::NORMAL_MAPPING))
        {
            source << "    _world_tangent = normalize(object.normal_matrix * in_tangent);\n";
            source << "    _world_bitangent = normalize(object.normal_matrix * in_bitangent);\n";
        }

        if (key.vertex_signature & kege::VertexBit::COLOR)
        {
            source << "    _color = processVertexColor();\n";
        }

        // Final position
        source << "    gl_Position = camera.view_projection_matrix * vec4(world_position, 1.0);\n";
        source << "}\n";
    }

    void ShaderCompiler::writeFragmentForwardOpaque(std::stringstream& source, const kege::PipelineKey& key)
    {
//        source << "    // Forward shading\n";
//        if (key.shading_model == kege::ShadingModel::LitPBR_Metallic ||
//            key.shading_model == kege::ShadingModel::LitPBR_Specular ||
//            key.shading_model == kege::ShadingModel::LitPBR_Cloth ||
//            key.shading_model == kege::ShadingModel::LitPBR_Clearcoat ||
//            key.shading_model == kege::ShadingModel::LitPBR_Skin ||
//            key.shading_model == kege::ShadingModel::LitPBR_Hair ||
//            key.shading_model == kege::ShadingModel::LitPBR_Subsurface)
//        {
//
//            // Apply features in order
//            source << "    // Initial values\n";
//            source << "    vec2 texcoord = in_texcoord;\n";
//            source << "    vec3 normal = normalize(_world_normal);\n";
//            source << "    \n";
//            source << "    vec3 view_dir = normalize(camera.camera_position - in_position);\n";
//            source << "    vec3 result = calculateLighting\n";
//            source << "    (\n";
//            source << "        in_position,\n";
//            source << "        normal,\n";
//            source << "        base_color.rgb,\n";
//            source << "        material.roughness,\n";
//            source << "        material.metallic\n";
//            source << "    );\n";
//            source << "    out_color = vec4(result, base_color.a);\n";
//        }
//        else if(key.shading_model == kege::ShadingModel::Toon_Cel)
//        {
//        }
//        else if(key.shading_model == kege::ShadingModel::Skybox)
//        {
//        }
//        else if(key.shading_model == kege::ShadingModel::Emissive)
//        {
//        }
//        else if(key.shading_model == kege::ShadingModel::Toon_UnlitOutline)
//        {
//        }
//        else if(key.shading_model == kege::ShadingModel::DepthOnly)
//        {
//            source << "    out_shadow = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);\n";
//        }
//        else if(key.shading_model == kege::ShadingModel::Unlit)
//        {
//            if(key.features & kege::FeatureFlag::VERTEX_COLOR)
//            {
//                source << "    out_color = _color;\n";
//            }
//            else
//            {
//                source << "    out_color = vec4(1.0);\n";
//            }
//        }
    }

    void ShaderCompiler::writeFragmentShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key)
    {
        // Main function
        source << "void main() {\n";

        // Parallax occlusion (modifies texcoord)
        if (key.features & kege::FeatureFlag::PARALLAX_OCCLUSION)
        {
            source << include("/features/parallax-occlusion.glsl");
            source << "    \n";
        }

        // Normal mapping (modifies normal)
        if (key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            source << include("/features/normal-mapping.glsl");
            source << "    \n";
        }


        // Sample base color
        //source << "    // Sample textures\n";
        //source << "    vec4 base_color = texture(base_colorTexture, texcoord) * material.base_color;\n";
        //source << "    \n";

        // Execute shading based on render pass
        switch (key.render_pass)
        {
            case kege::RenderPass::GBuffer:
                source << "    // GBuffer output\n";
                source << "    outGBuffer0 = vec4(base_color.rgb, 0.0); // MaterialID in alpha\n";
                source << "    outGBuffer1 = vec4(normal * 0.5 + 0.5, material.roughness);\n";
                source << "    outGBuffer2 = vec4(material.metallic, 0.0, material.ambient_occlusion, 0.0);\n";
                break;

            case kege::RenderPass::Forward:
                writeFragmentForwardOpaque(source, key);
                break;

            case kege::RenderPass::Shadow:
                if (!(key.features & kege::FeatureFlag::DEPTH_ONLY))
                {
                    source << "    // Shadow map (maybe output depth variance)\n";
                    source << "    outShadow = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);\n";
                }
                break;

            default:
            case kege::RenderPass::UI:
                source << "    // UI rendering (simple)\n";
                source << "    out_color = base_color;\n";
                break;
        }

        source << "}\n";
    }

    void ShaderCompiler::writeVersionAndExtensions(std::stringstream& source)
    {
        source << "#version 450\n";
        source << "#extension GL_ARB_separate_shader_objects : enable\n\n";
    }

    void ShaderCompiler::writeFeatures(const kege::PipelineKey& key, std::vector<std::string>& defines)
    {
        // ====== VERTEX SHADER FEATURES ======
        if (key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            defines.push_back("FEATURE_VERTEX_ANIMATION");
        if (key.features & kege::FeatureFlag::MORPH_TARGETS)
            defines.push_back("FEATURE_MORPH_TARGETS");
        if (key.features & kege::FeatureFlag::GPU_SKINNING)
            defines.push_back("FEATURE_GPU_SKINNING");
        if (key.features & kege::FeatureFlag::VERTEX_DISPLACEMENT)
            defines.push_back("FEATURE_VERTEX_DISPLACEMENT");
        if (key.features & kege::FeatureFlag::WIND_ANIMATION)
            defines.push_back("FEATURE_WIND_ANIMATION");
        if (key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            defines.push_back("FEATURE_VERTEX_ANIMATION");
        if (key.features & kege::FeatureFlag::INSTANCED_RENDERING)
            defines.push_back("FEATURE_INSTANCED_RENDERING");
        if (key.features & kege::FeatureFlag::VERTEX_COLOR)
            defines.push_back("FEATURE_VERTEX_COLOR");

        // ====== TESSELLATION FEATURES ======
        if (key.features & kege::FeatureFlag::TESSELLATION)
            defines.push_back("FEATURE_TESSELLATION");
        if (key.features & kege::FeatureFlag::PN_TRIANGLES)
            defines.push_back("FEATURE_PN_TRIANGLES");
        if (key.features & kege::FeatureFlag::TESSELLATION_DISPLACEMENT)
            defines.push_back("FEATURE_DISPLACEMENT_TESSELLATION");

        // ====== FRAGMENT SHADER FEATURES ======
        if (key.features & kege::FeatureFlag::NORMAL_MAPPING)
            defines.push_back("FEATURE_NORMAL_MAPPING");
        if (key.features & kege::FeatureFlag::TRIPLANAR_MAPPING)
            defines.push_back("FEATURE_TRIPLANAR_MAPPING");
        if (key.features & kege::FeatureFlag::PARALLAX_MAPPING)
            defines.push_back("FEATURE_PARALLAX_MAPPING");
        if (key.features & kege::FeatureFlag::PARALLAX_OCCLUSION)
            defines.push_back("FEATURE_PARALLAX_OCCLUSION");

        // ====== MATERIAL LAYERS ======
        if (key.features & kege::FeatureFlag::CLEAR_COAT)
            defines.push_back("FEATURE_CLEAR_COAT");
        if (key.features & kege::FeatureFlag::ANISOTROPIC_REFLECTION)
            defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (key.features & kege::FeatureFlag::SHEEN_LAYER)
            defines.push_back("FEATURE_SHEEN_LAYER");
        if (key.features & kege::FeatureFlag::THIN_FILM)
            defines.push_back("FEATURE_THIN_FILM");
        if (key.features & kege::FeatureFlag::TRANSMISSION)
            defines.push_back("FEATURE_TRANSMISSION");
        if (key.features & kege::FeatureFlag::SUBSURFACE_SCATTERING)
            defines.push_back("FEATURE_SUBSURFACE_SCATTERING");

        // ====== TRANSPARENCY/ALPHA ======
        if (key.features & kege::FeatureFlag::ALPHA_TEST)
            defines.push_back("FEATURE_ALPHA_TEST");
        if (key.features & kege::FeatureFlag::ALPHA_TO_COVERAGE)
            defines.push_back("FEATURE_ALPHA_TO_COVERAGE");
        if (key.features & kege::FeatureFlag::DITHERED_OPACITY)
            defines.push_back("FEATURE_DITHERED_OPACITY");
        if (key.features & kege::FeatureFlag::PREMULTIPLIED_ALPHA)
            defines.push_back("FEATURE_PREMULTIPLIED_ALPHA");

        // ====== PARTICLE EFFECTS ======
        if (key.features & kege::FeatureFlag::SOFT_PARTICLES)
            defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (key.features & kege::FeatureFlag::PARTICLE_ROTATION)
            defines.push_back("FEATURE_PARTICLE_ROTATION");

        // ====== ENVIRONMENT INTERACTION ======
        if (key.features & kege::FeatureFlag::SCREEN_SPACE_REFLECTION)
            defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (key.features & kege::FeatureFlag::REFRACTION)
            defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (key.features & kege::FeatureFlag::WATER_EFFECTS)
            defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (key.features & kege::FeatureFlag::DECAL)
            defines.push_back("FEATURE_DECAL");

        // ====== LIGHTING FEATURES ======
        if (key.features & kege::FeatureFlag::MULTIPLE_BOUNCE_GI)
            defines.push_back("FEATURE_MULTIPLE_BOUNCE_GI");
        if (key.features & kege::FeatureFlag::AREA_LIGHTS)
            defines.push_back("FEATURE_AREA_LIGHTS");
        if (key.features & kege::FeatureFlag::CONTACT_SHADOWS)
            defines.push_back("FEATURE_CONTACT_SHADOWS");
        if (key.features & kege::FeatureFlag::VOLUMETRIC_LIGHTING)
            defines.push_back("FEATURE_VOLUMETRIC_LIGHTING");


        // ====== POST-PROCESS/SCREEN SPACE ======
        if (key.features & kege::FeatureFlag::MOTION_VECTORS)
            defines.push_back("FEATURE_MOTION_VECTORS");
        if (key.features & kege::FeatureFlag::VELOCITY_BUFFER)
            defines.push_back("FEATURE_VELOCITY_BUFFER");
        if (key.features & kege::FeatureFlag::DEPTH_ONLY)
            defines.push_back("FEATURE_DEPTH_ONLY");
        if (key.features & kege::FeatureFlag::CUSTOM_DEPTH)
            defines.push_back("FEATURE_CUSTOM_DEPTH");

        // ====== TERRAIN/VEGETATION ======
        if (key.features & kege::FeatureFlag::TERRAIN_BLENDING)
            defines.push_back("FEATURE_TERRAIN_BLENDING");
        if (key.features & kege::FeatureFlag::VERTEX_GRASS_WIND)
            defines.push_back("FEATURE_VERTEX_GRASS_WIND");
        if (key.features & kege::FeatureFlag::LOD_CROSSFADE)
            defines.push_back("FEATURE_LOD_CROSSFADE");
        if (key.features & kege::FeatureFlag::DECAL_LAYER)
            defines.push_back("FEATURE_DECAL_LAYER");

        // ====== DEBUG/DEVELOPMENT ======
        if (key.features & kege::FeatureFlag::WIREFRAME_OVERLAY)
            defines.push_back("FEATURE_WIREFRAME_OVERLAY");
        if (key.features & kege::FeatureFlag::DEBUG_NORMALS)
            defines.push_back("FEATURE_DEBUG_NORMALS");
        if (key.features & kege::FeatureFlag::DEBUG_UVS)
            defines.push_back("FEATURE_DEBUG_UVS");
        if (key.features & kege::FeatureFlag::DEBUG_TANGENTS)
            defines.push_back("FEATURE_DEBUG_TANGENTS");

        // ====== SHADOW FEATURES ======
        if (key.features & kege::FeatureFlag::CONTACT_HARDENING)
            defines.push_back("FEATURE_CONTACT_HARDENING");
        if (key.features & kege::FeatureFlag::VARIANCE_SHADOW_MAP)
            defines.push_back("FEATURE_VARIANCE_SHADOW_MAP");
        if (key.features & kege::FeatureFlag::CASCADE_BLENDING)
            defines.push_back("FEATURE_CASCADE_BLENDING");

        // ====== ADVANCED RENDERING ======
        if (key.features & kege::FeatureFlag::RAY_TRACING_OVERRIDE)
            defines.push_back("FEATURE_RAY_TRACING_OVERRIDE");
        if (key.features & kege::FeatureFlag::VIRTUAL_TEXTURE)
            defines.push_back("FEATURE_VIRTUAL_TEXTURE");
        if (key.features & kege::FeatureFlag::FLIPBOOK_ANIMATION)
            defines.push_back("FEATURE_FLIPBOOK_ANIMATION");

        // ====== UI RENDERING ======
        if (key.features & kege::FeatureFlag::ROUNDED_UI_CORNERS)
            defines.push_back("FEATURE_ROUNDED_UI_CORNERS");
        if (key.features & kege::FeatureFlag::SINE_DIST_TEXT)
            defines.push_back("FEATURE_SINE_DIST_TEXT");

        // ====== FRAGMENT SHADER FEATURES ======
        if (key.features & kege::FeatureFlag::DETAIL_NORMAL)
            defines.push_back("FEATURE_DETAIL_NORMAL");


        defines.push_back("CAMERA_SET_INDEX " + std::to_string(set_index++));
        defines.push_back("CAMERA_BINDING_INDEX 0");

        defines.push_back("OBJECT_SET_INDEX " + std::to_string(set_index++));
        defines.push_back("OBJECT_BINDING_INDEX 0");
    }

}
