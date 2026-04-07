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
        _key = key;
        
        writeFeatures();
        std::string vs = writeVertexShader();
        std::string fs = writeFragmentShader();
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

    std::string ShaderCompiler::writeVertexShader()
    {
        std::stringstream source;
        beginVertexShader(source);

        writeVersion(source);
        writeExtensions(source);
        writeMacros(source);
        writeInputs(source);
        writeOutputs(source);
        writeResources(source);
        writeFunctions(source);
        writeMainFn(source);

        endVertexShader(source);
        return source.str();
    }

    std::string ShaderCompiler::writeFragmentShader()
    {
        std::stringstream source;
        beginFragmentShader(source);

        writeVersion(source);
        writeExtensions(source);
        writeMacros(source);
        writeInputs(source);
        writeOutputs(source);
        writeResources(source);
        writeFunctions(source);
        writeMainFn(source);

        endFragmentShader(source);
        return source.str();
    }

    ShaderProgram* ShaderCompiler::compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs)
    {


        const std::string path = "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/";

        // Open file for writing
        std::ofstream vsFile(path + "vs.glsl");
        // Write the entire string buffer
        vsFile << vs;
        // Close the file
        vsFile.close();


        // Open file for writing
        std::ofstream fsFile(path + "fs.glsl");
        // Write the entire string buffer
        fsFile << fs;
        // Close the file
        fsFile.close();




        //std::cout << vs <<"\n";
        //std::cout <<"\n--------- --------- --------- --------- --------- --------- ---------\n\n";
        //std::cout << fs <<"\n";

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

    void ShaderCompiler::beginVertexShader(std::stringstream& source)
    {
        _current_stage = ShaderStge::VERTEX;

        int input_location = 0;
        int output_location = 0;
        if (_key.renderer_type == kege::MeshType::StaticMesh)
        {
            _inputs.push_back({ VertexInfo::POSITION, input_location++ });
            _inputs.push_back({ VertexInfo::TEXCOORD, input_location++ });
            _inputs.push_back({ VertexInfo::NORMAL, input_location++ });

            _outputs.push_back({ VertexInfo::POSITION, output_location++ });
            _outputs.push_back({ VertexInfo::TEXCOORD, output_location++ });
            _outputs.push_back({ VertexInfo::NORMAL, output_location++ });
        }

        if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            _inputs.push_back({ VertexInfo::TANGENT, input_location++ });
            _inputs.push_back({ VertexInfo::BITANGENT, input_location++ });

            _outputs.push_back({ VertexInfo::TANGENT, output_location++ });
            _outputs.push_back({ VertexInfo::BITANGENT, output_location++ });
        }
        if (_key.features & kege::FeatureFlag::GPU_SKINNING)
        {
            _inputs.push_back({ VertexInfo::JOINTS, input_location++ });
            _inputs.push_back({ VertexInfo::WEIGHTS, input_location++ });
        }

        if (_key.features == kege::FeatureFlag::VERTEX_COLOR)
        {
            _inputs.push_back({ VertexInfo::COLOR, input_location++ });
        }
    }

    void ShaderCompiler::endVertexShader(std::stringstream& source){
        _inputs = _outputs;
        _outputs = {};
    }

    void ShaderCompiler::beginFragmentShader(std::stringstream& source)
    {
        _current_stage = ShaderStge::FRAGMENT;

        _has_normal_mapping = (_key.features & kege::FeatureFlag::NORMAL_MAPPING);
        _has_material = (_key.features & kege::FeatureFlag::MATERIAL);

        _has_lights = (_key.features & kege::FeatureFlag::DIRECTIONAL_LIGHT) ||
        (_key.features & kege::FeatureFlag::POINT_LIGHT) ||
        (_key.features & kege::FeatureFlag::SPOT_LIGHT);

        _has_light_funct =
        _key.shading_model == kege::ShadingModel::LitPBR_Metallic ||
        _key.shading_model == kege::ShadingModel::LitPBR_Subsurface ||
        _key.shading_model == kege::ShadingModel::LitPBR_Clearcoat ||
        _key.shading_model == kege::ShadingModel::LitPBR_Cloth |
        _key.shading_model == kege::ShadingModel::LitPBR_Skin ||
        _key.shading_model == kege::ShadingModel::LitPBR_Hair;

        int output_location = 0;
        switch ( _key.render_pass )
        {
            default:
            {
                _outputs.push_back({ VertexInfo::COLOR, output_location++ });
                break;
            }
            case kege::RenderPass::GBuffer:
            {
                _outputs.push_back({ VertexInfo::COLOR,    output_location++ });
                _outputs.push_back({ VertexInfo::POSITION, output_location++ });
                _outputs.push_back({ VertexInfo::NORMAL,   output_location++ });
                _outputs.push_back({ VertexInfo::EMISSIVE,   output_location++ });
                break;
            }
        }
    }

    void ShaderCompiler::endFragmentShader(std::stringstream& source)
    {
        _inputs = _outputs;
        _outputs = {};
    }

    void ShaderCompiler::writeVersion(std::stringstream& source)
    {
        source << "#version 450\n";
    }

    void ShaderCompiler::writeExtensions(std::stringstream& source)
    {
        source << "#extension GL_ARB_separate_shader_objects : enable\n\n";
    }

    void ShaderCompiler::writeMacros(std::stringstream& source)
    {
        for (const auto& define : _defines)
        {
            source << "#define " << define << "\n";
        }
        source << "\n";

        for(const VertexInfo& info : _inputs)
        {
            _input_signature |= (1 << int(info.semantic));
            switch (info.semantic)
            {
                case VertexInfo::POSITION:
                    source << include( "define/input-position.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::TEXCOORD:
                    source << include( "define/input-texcoord.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::NORMAL:
                    source << include( "define/input-normal.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::COLOR:
                    source << include( "define/input-color.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::TANGENT:
                    source << include( "define/input-tangent.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::BITANGENT:
                    source << include( "define/input-bitangent.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::JOINTS:
                    source << include( "define/input-joints.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::WEIGHTS:
                    source << include( "define/input-weights.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::EMISSIVE:
                    source << include( "define/input-emissive.glsl") << info.location <<"\n";
                    break;

                default: break;
            }
        }
        if( !_inputs.empty() ) source <<"\n";

        for(const VertexInfo& info : _outputs)
        {
            switch (info.semantic)
            {
                case VertexInfo::POSITION:
                    source << include( "define/output-position.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::TEXCOORD:
                    source << include( "define/output-texcoord.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::NORMAL:
                    source << include( "define/output-normal.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::COLOR:
                    source << include( "define/output-color.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::TANGENT:
                    source << include( "define/output-tangent.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::BITANGENT:
                    source << include( "define/output-bitangent.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::JOINTS:
                    source << include( "define/output-joints.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::WEIGHTS:
                    source << include( "define/output-weights.glsl") << info.location <<"\n";
                    break;

                case VertexInfo::EMISSIVE:
                    source << include( "define/output-emissive.glsl") << info.location <<"\n";
                    break;

                default: break;
            }
        }
        if( !_outputs.empty() ) source <<"\n";
    }

    void ShaderCompiler::writeInputs(std::stringstream& source)
    {
        switch (_key.renderer_type)
        {
            default:
            case kege::MeshType::Point:
                source << include( "input/point.glsl") <<"\n";
                break;

            case kege::MeshType::ScreenRect:
                source << include( "input/screen-rect.glsl") <<"\n";
                break;

            case kege::MeshType::StaticMesh:
                source << include( "input/static-mesh.glsl") <<"\n";
                break;
                
            case kege::MeshType::Particle:
                source << include( "input/particle.glsl") <<"\n";
                break;
                
            case kege::MeshType::FlatTerrain:
                source << include( "input/flat-terrain.glsl") <<"\n";
                break;
                
            case kege::MeshType::SphericalTerrain:
                source << include( "input/spherical-terrain.glsl") <<"\n";
                break;
        }

        if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            source << include( "input/normal-mapping.glsl") <<"\n";
        }

        if (_key.features & kege::FeatureFlag::GPU_SKINNING)
        {
            source << include( "input/gpu-skinning.glsl") <<"\n";
        }

        if (_key.features == kege::FeatureFlag::VERTEX_COLOR)
        {
            source << include( "input/vertex-color.glsl") <<"\n";
        }
        source <<"\n";
    }

    void ShaderCompiler::writeOutputs(std::stringstream& source)
    {
        if( _current_stage == ShaderStge::VERTEX )
        {
            for(const VertexInfo& info : _outputs)
            {
                switch (info.semantic)
                {
                    case VertexInfo::POSITION:
                        source << include( "output/position.glsl") <<"\n";
                        break;

                    case VertexInfo::TEXCOORD:
                        source << include( "output/texcoord.glsl") <<"\n";
                        break;

                    case VertexInfo::NORMAL:
                        source << include( "output/normal.glsl") <<"\n";
                        break;

                    case VertexInfo::COLOR:
                        source << include( "output/color.glsl") <<"\n";
                        break;

                    case VertexInfo::TANGENT:
                        source << include( "output/tangent.glsl") <<"\n";
                        break;

                    case VertexInfo::BITANGENT:
                        source << include( "output/bitangent.glsl") <<"\n";
                        break;

                    case VertexInfo::JOINTS:
                        source << include( "output/joints.glsl") <<"\n";
                        break;

                    case VertexInfo::WEIGHTS:
                        source << include( "output/weights.glsl") <<"\n";
                        break;

                    case VertexInfo::EMISSIVE:
                        source << include( "output/emissive.glsl") <<"\n";
                        break;

                    default: break;
                }
            }
        }
        else if( _current_stage == ShaderStge::FRAGMENT )
        {
            for(const VertexInfo& info : _outputs)
            {
                switch (info.semantic)
                {
                    case VertexInfo::POSITION:
                        source << include( "output/buffer-position.glsl") <<"\n";
                        break;

                    case VertexInfo::EMISSIVE:
                        source << include( "output/buffer-emissive.glsl") <<"\n";
                        break;

                    case VertexInfo::NORMAL:
                        source << include( "output/buffer-normal.glsl") <<"\n";
                        break;

                    case VertexInfo::COLOR:
                        source << include( "output/buffer-color.glsl") <<"\n";
                        break;

                    case VertexInfo::TEXCOORD:
                        source << include( "output/buffer-texcoord.glsl") <<"\n";
                        break;

                    case VertexInfo::TANGENT:
                        source << include( "output/buffer-tangent.glsl") <<"\n";
                        break;

                    case VertexInfo::BITANGENT:
                        source << include( "output/buffer-bitangent.glsl") <<"\n";
                        break;

                    case VertexInfo::JOINTS:
                        source << include( "output/buffer-joints.glsl") <<"\n";
                        break;

                    case VertexInfo::WEIGHTS:
                        source << include( "output/buffer-weights.glsl") <<"\n";
                        break;

                    default: break;
                }
            }
        }
        source <<"\n";
    }

    void ShaderCompiler::writeResources(std::stringstream& source)
    {
        source << include("resources/uniform-camera-data.glsl") << "\n\n";
        source << include("resources/uniform-object-data.glsl") << "\n\n";

        if( _current_stage == ShaderStge::FRAGMENT )
        {
            if ((_key.features & kege::FeatureFlag::MATERIAL))
            {
                source << include( "resources/uniform-material.glsl") <<"\n";
            }

            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                source << include( "resources/textures-ibl.glsl") <<"\n";
            }

            if ((_key.features & kege::FeatureFlag::DIRECTIONAL_LIGHT))
            {
                source <<"\n";
                source << include( "resources/uniform-directional-light.glsl") <<"\n";
            }
            if ((_key.features & kege::FeatureFlag::POINT_LIGHT))
            {
                source <<"\n";
                source << include( "resources/uniform-point-light.glsl") <<"\n";
            }
            if ((_key.features & kege::FeatureFlag::SPOT_LIGHT))
            {
                source <<"\n";
                source << include( "resources/uniform-spot-light.glsl") <<"\n";
            }
        }
    }

    void ShaderCompiler::writeFunctions(std::stringstream& source)
    {
        if( _current_stage == ShaderStge::FRAGMENT )
        {
            if( _has_material )
            {
                source << include( "resources/fn-get-albedo.glsl") <<"\n";
                if (_has_normal_mapping)
                {
                    source << include( "resources/fn-get-tbn-normal.glsl") <<"\n";
                }
            }
            if( _has_material && _has_light_funct && _has_lights )
            {
                source << include( "resources/fn-get-opacity.glsl") <<"\n";
                source << include( "resources/fn-get-ambient-occlusion.glsl") <<"\n";
                source << include( "resources/fn-get-roughness.glsl") <<"\n";
                source << include( "resources/fn-get-metallic.glsl") <<"\n";
                source << include( "resources/fn-get-albedo.glsl") <<"\n";
                source << include( "resources/fn-get-normal.glsl") <<"\n";
                source << include( "resources/fn-get-emissive.glsl") <<"\n";
            }


            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                source << include( "shading-model/fn-calc-ambient-ibl.glsl") <<"\n";
            }

            if (_key.shading_model == kege::ShadingModel::LitPBR_Metallic ||
                _key.shading_model == kege::ShadingModel::LitPBR_Skin ||
                _key.shading_model == kege::ShadingModel::LitPBR_Hair ||
                _key.shading_model == kege::ShadingModel::LitPBR_Cloth ||
                _key.shading_model == kege::ShadingModel::LitPBR_Subsurface ||
                _key.shading_model == kege::ShadingModel::LitPBR_Clearcoat )
            {
                source << include( "shading-model/fn-brdf.glsl") <<"\n";
            }

            if (_key.shading_model == kege::ShadingModel::LitPBR_Metallic)
            {
                source << include( "shading-model/fn-lighting.glsl") <<"\n";
            }

            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                source << include( "shading-model/fn-calc-ambient-ibl.glsl") <<"\n";
            }
            if ((_key.features & kege::FeatureFlag::MATERIAL))
            {
                source << include( "resources/fn-material.glsl") <<"\n";
                if ((_key.features & kege::FeatureFlag::NORMAL_MAPPING))
                {
                    source << include( "resources/fn-normal-mapping.glsl") <<"\n";
                }

                if ((_key.features & kege::FeatureFlag::ANISOTROPIC_REFLECTION))
                {
                    source << include( "resources/fn-anisotropy.glsl") <<"\n";
                }

                if ((_key.features & kege::FeatureFlag::CLEAR_COAT))
                {
                    source << include( "resources/fn-clear-coat.glsl") <<"\n";
                }
            }
        }
    }
    
    void ShaderCompiler::writeMainFn(std::stringstream& source)
    {
        source <<"\n";
        source << "void main() {\n";
        switch ( _current_stage )
        {
            case ShaderStge::VERTEX:
            {
                source << "    // Process vertex based on renderer type\n";

                if (_key.renderer_type == kege::MeshType::StaticMesh && (_key.features & kege::FeatureFlag::GPU_SKINNING))
                {
                    source << include( "renderer-type/gpu-skinning.glsl") <<"\n";
                    if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
                    {
                        source << include( "renderer-type/gpu-skinning-normal-mapping.glsl") <<"\n";
                    }
                }
                else if (_key.renderer_type == kege::MeshType::StaticMesh)
                {
                    source << include( "renderer-type/static-mesh.glsl") <<"\n";
                    if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
                    {
                        source << include( "renderer-type/static-mesh-normal-mapping.glsl") <<"\n";
                    }
                }
                else if (_key.renderer_type == kege::MeshType::FlatTerrain)
                {
                }
                else if (_key.renderer_type == kege::MeshType::SphericalTerrain)
                {
                }
                else if (_key.renderer_type == kege::MeshType::Particle)
                {
                }

                if (_key.features == kege::FeatureFlag::VERTEX_COLOR)
                {
                    source << include( "main/vertex/static-color.glsl") <<"\n";
                }

                source << include( "renderer-type/final-vertex-output.glsl") <<"\n";
            }
            break;

            case ShaderStge::FRAGMENT:
            {
                if(( _input_signature & VertexInfo::POSITION ))
                {
                    source << "    vec3 position = in_position;\n";
                }
                if( _has_material )
                {
                    source << include( "resources/get-albedo.glsl") <<"\n";
                    if (_has_normal_mapping)
                    {
                        source << include( "resources/get-tbn-normal.glsl") <<"\n";
                    }
                }
                if( _has_light_funct )
                {
                    if( _has_material && _has_lights )
                    {
                        source << include( "resources/get-opacity.glsl") <<"\n";
                        source << include( "resources/get-ambient-occlusion.glsl") <<"\n";
                        source << include( "resources/get-roughness.glsl") <<"\n";
                        source << include( "resources/get-metallic.glsl") <<"\n";
                        source << include( "resources/get-normal.glsl") <<"\n";
                        source << include( "resources/get-emissive.glsl") <<"\n";
                    }
                    else
                    {
                        source << include( "resources/use-internal-material.glsl") <<"\n";
                        if(( _input_signature & (1 << VertexInfo::NORMAL) ))
                        {
                            source << include( "resources/get-input-normal.glsl") <<"\n";
                        }
                    }
                    
                    if(( _input_signature & (1 << VertexInfo::POSITION) ))
                    {
                        source << "    vec3 view_dir = normalize(camera.cameraPosition - in_position);\n";
                    }

                    if( _has_lights )
                    {
                        if ((_key.features & kege::FeatureFlag::DIRECTIONAL_LIGHT))
                        {
                            source << include( "resources/integrate-directional-light.glsl") <<"\n";
                        }
                        if ((_key.features & kege::FeatureFlag::POINT_LIGHT))
                        {
                            source << include( "resources/integrate-point-light.glsl") <<"\n\n";
                        }
                        if ((_key.features & kege::FeatureFlag::SPOT_LIGHT))
                        {
                            source << include( "resources/integrate-spot-light.glsl") <<"\n\n";
                        }
                    }
                    else
                    {
                        source << include( "resources/use-internal-light.glsl") <<"\n";
                    }
                }
                else if (_key.shading_model == kege::ShadingModel::Unlit)
                {
                    if( _has_material )
                    {
                        source << "    vec3 final_color = getAlbedo();\n";
                    }
                    if(( _input_signature & VertexInfo::COLOR ))
                    {
                        source << "    vec3 final_color = in_color.rgb;\n";
                    }
                }

                if ((_key.features & kege::FeatureFlag::TONE_MAPPING))
                {
                    source << include( "features/tone-mapping.glsl") <<"\n\n";
                }
                
                if ((_key.features & kege::FeatureFlag::GAMMA))
                {
                    source << include( "features/gamma.glsl") <<"\n\n";
                }

                switch (_key.render_pass)
                {
                    default:
                    case RenderPass::Forward:
                        source << include( "render-pass/to-color-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::GBuffer:
                        source << include( "render-pass/to-geometry-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::Shadow:
                        source << include( "render-pass/to-depth-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::DepthPrePass:
                        source << include( "render-pass/to-depth-buffer.glsl") <<"\n";
                        break;
                }
            }
            break;

            default: break;
        }
        source << "}\n";
    }

    std::string ShaderCompiler::include(const std::string& fname )
    {
        const std::string filename = "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/snippets/" + fname;
        std::ifstream file(filename);
        std::string loaded_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        if(loaded_source.length() >= 1)
        {
            size_t i = loaded_source.length() - 1;
            if(loaded_source[ i ] == '\n')
                loaded_source[ i ] = ' ';
        }
        return loaded_source;
    }

    void ShaderCompiler::processShadingModel(std::stringstream& source, const kege::ShadingModel& model)
    {
        switch(model)
        {
            default:
            case kege::ShadingModel::Unlit:
            {
                source << include("shading-model/unlit.glsl");
                break;
            }

            case kege::ShadingModel::LitPBR_Metallic:
            {
                source << include("shading-model/lit-pbr-metallic.glsl");
                break;
            }
        }
    }

    void ShaderCompiler::processMeshType(std::stringstream& source, const kege::PipelineKey& key)
    {
        switch(key.renderer_type)
        {
            default:
            case kege::MeshType::Point:
            {
                source << include("renderer-type/point.glsl");
                break;
            }
            case kege::MeshType::StaticMesh:
            {
                source << include("renderer-type/static-mesh.glsl");
                break;
            }
        }

        if (key.features & kege::FeatureFlag::NORMAL_MAPPING)
        {
            source << include("feature/vertex/normal-mapping.glsl");
        }

        if (key.features & kege::FeatureFlag::GPU_SKINNING)
        {
            source << include("feature/vertex/gpu-skinning.glsl");
        }

        if (key.features & kege::FeatureFlag::VERTEX_COLOR)
        {
            source << include("feature/vertex/vertex-color.glsl");
        }
    }

    void ShaderCompiler::writeFeatures()
    {
        // ====== VERTEX SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            _defines.push_back("FEATURE_VERTEX_ANIMATION");
        if (_key.features & kege::FeatureFlag::MORPH_TARGETS)
            _defines.push_back("FEATURE_MORPH_TARGETS");
        if (_key.features & kege::FeatureFlag::GPU_SKINNING)
            _defines.push_back("FEATURE_GPU_SKINNING");
        if (_key.features & kege::FeatureFlag::VERTEX_DISPLACEMENT)
            _defines.push_back("FEATURE_VERTEX_DISPLACEMENT");
        if (_key.features & kege::FeatureFlag::WIND_ANIMATION)
            _defines.push_back("FEATURE_WIND_ANIMATION");
        if (_key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            _defines.push_back("FEATURE_VERTEX_ANIMATION");
        if (_key.features & kege::FeatureFlag::INSTANCED_RENDERING)
            _defines.push_back("FEATURE_INSTANCED_RENDERING");
        if (_key.features & kege::FeatureFlag::VERTEX_COLOR)
            _defines.push_back("FEATURE_VERTEX_COLOR");

        // ====== TESSELLATION FEATURES ======
        if (_key.features & kege::FeatureFlag::TESSELLATION)
            _defines.push_back("FEATURE_TESSELLATION");
        if (_key.features & kege::FeatureFlag::PN_TRIANGLES)
            _defines.push_back("FEATURE_PN_TRIANGLES");
        if (_key.features & kege::FeatureFlag::TESSELLATION_DISPLACEMENT)
            _defines.push_back("FEATURE_DISPLACEMENT_TESSELLATION");

        // ====== FRAGMENT SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
            _defines.push_back("FEATURE_NORMAL_MAPPING");
        if (_key.features & kege::FeatureFlag::TRIPLANAR_MAPPING)
            _defines.push_back("FEATURE_TRIPLANAR_MAPPING");
        if (_key.features & kege::FeatureFlag::PARALLAX_MAPPING)
            _defines.push_back("FEATURE_PARALLAX_MAPPING");
        if (_key.features & kege::FeatureFlag::PARALLAX_OCCLUSION)
            _defines.push_back("FEATURE_PARALLAX_OCCLUSION");

        // ====== MATERIAL LAYERS ======
        if (_key.features & kege::FeatureFlag::CLEAR_COAT)
            _defines.push_back("FEATURE_CLEAR_COAT");
        if (_key.features & kege::FeatureFlag::ANISOTROPIC_REFLECTION)
            _defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::SHEEN_LAYER)
            _defines.push_back("FEATURE_SHEEN_LAYER");
        if (_key.features & kege::FeatureFlag::THIN_FILM)
            _defines.push_back("FEATURE_THIN_FILM");
        if (_key.features & kege::FeatureFlag::TRANSMISSION)
            _defines.push_back("FEATURE_TRANSMISSION");
        if (_key.features & kege::FeatureFlag::SUBSURFACE_SCATTERING)
            _defines.push_back("FEATURE_SUBSURFACE_SCATTERING");

        // ====== TRANSPARENCY/ALPHA ======
        if (_key.features & kege::FeatureFlag::ALPHA_TEST)
            _defines.push_back("FEATURE_ALPHA_TEST");
        if (_key.features & kege::FeatureFlag::ALPHA_TO_COVERAGE)
            _defines.push_back("FEATURE_ALPHA_TO_COVERAGE");
        if (_key.features & kege::FeatureFlag::DITHERED_OPACITY)
            _defines.push_back("FEATURE_DITHERED_OPACITY");
        if (_key.features & kege::FeatureFlag::PREMULTIPLIED_ALPHA)
            _defines.push_back("FEATURE_PREMULTIPLIED_ALPHA");

        // ====== PARTICLE EFFECTS ======
        if (_key.features & kege::FeatureFlag::SOFT_PARTICLES)
            _defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::PARTICLE_ROTATION)
            _defines.push_back("FEATURE_PARTICLE_ROTATION");

        // ====== ENVIRONMENT INTERACTION ======
        if (_key.features & kege::FeatureFlag::SCREEN_SPACE_REFLECTION)
            _defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::REFRACTION)
            _defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::WATER_EFFECTS)
            _defines.push_back("FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::DECAL)
            _defines.push_back("FEATURE_DECAL");

        // ====== LIGHTING FEATURES ======
        if (_key.features & kege::FeatureFlag::MULTIPLE_BOUNCE_GI)
            _defines.push_back("FEATURE_MULTIPLE_BOUNCE_GI");
        if (_key.features & kege::FeatureFlag::AREA_LIGHTS)
            _defines.push_back("FEATURE_AREA_LIGHTS");
        if (_key.features & kege::FeatureFlag::CONTACT_SHADOWS)
            _defines.push_back("FEATURE_CONTACT_SHADOWS");
        if (_key.features & kege::FeatureFlag::VOLUMETRIC_LIGHTING)
            _defines.push_back("FEATURE_VOLUMETRIC_LIGHTING");


        // ====== POST-PROCESS/SCREEN SPACE ======
        if (_key.features & kege::FeatureFlag::MOTION_VECTORS)
            _defines.push_back("FEATURE_MOTION_VECTORS");
        if (_key.features & kege::FeatureFlag::VELOCITY_BUFFER)
            _defines.push_back("FEATURE_VELOCITY_BUFFER");
        if (_key.features & kege::FeatureFlag::DEPTH_ONLY)
            _defines.push_back("FEATURE_DEPTH_ONLY");
        if (_key.features & kege::FeatureFlag::CUSTOM_DEPTH)
            _defines.push_back("FEATURE_CUSTOM_DEPTH");

        // ====== TERRAIN/VEGETATION ======
        if (_key.features & kege::FeatureFlag::TERRAIN_BLENDING)
            _defines.push_back("FEATURE_TERRAIN_BLENDING");
        if (_key.features & kege::FeatureFlag::VERTEX_GRASS_WIND)
            _defines.push_back("FEATURE_VERTEX_GRASS_WIND");
        if (_key.features & kege::FeatureFlag::LOD_CROSSFADE)
            _defines.push_back("FEATURE_LOD_CROSSFADE");
        if (_key.features & kege::FeatureFlag::DECAL_LAYER)
            _defines.push_back("FEATURE_DECAL_LAYER");

        // ====== DEBUG/DEVELOPMENT ======
        if (_key.features & kege::FeatureFlag::WIREFRAME_OVERLAY)
            _defines.push_back("FEATURE_WIREFRAME_OVERLAY");
        if (_key.features & kege::FeatureFlag::DEBUG_NORMALS)
            _defines.push_back("FEATURE_DEBUG_NORMALS");
        if (_key.features & kege::FeatureFlag::DEBUG_UVS)
            _defines.push_back("FEATURE_DEBUG_UVS");
        if (_key.features & kege::FeatureFlag::DEBUG_TANGENTS)
            _defines.push_back("FEATURE_DEBUG_TANGENTS");

        // ====== SHADOW FEATURES ======
        if (_key.features & kege::FeatureFlag::CONTACT_HARDENING)
            _defines.push_back("FEATURE_CONTACT_HARDENING");
        if (_key.features & kege::FeatureFlag::VARIANCE_SHADOW_MAP)
            _defines.push_back("FEATURE_VARIANCE_SHADOW_MAP");
        if (_key.features & kege::FeatureFlag::CASCADE_BLENDING)
            _defines.push_back("FEATURE_CASCADE_BLENDING");

        // ====== ADVANCED RENDERING ======
        if (_key.features & kege::FeatureFlag::RAY_TRACING_OVERRIDE)
            _defines.push_back("FEATURE_RAY_TRACING_OVERRIDE");
        if (_key.features & kege::FeatureFlag::VIRTUAL_TEXTURE)
            _defines.push_back("FEATURE_VIRTUAL_TEXTURE");
        if (_key.features & kege::FeatureFlag::FLIPBOOK_ANIMATION)
            _defines.push_back("FEATURE_FLIPBOOK_ANIMATION");

        // ====== UI RENDERING ======
        if (_key.features & kege::FeatureFlag::ROUNDED_UI_CORNERS)
            _defines.push_back("FEATURE_ROUNDED_UI_CORNERS");
        if (_key.features & kege::FeatureFlag::SINE_DIST_TEXT)
            _defines.push_back("FEATURE_SINE_DIST_TEXT");

        // ====== FRAGMENT SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::DETAIL_NORMAL)
            _defines.push_back("FEATURE_DETAIL_NORMAL");

        _defines.push_back("CAMERA_SET " + std::to_string(set_index++));
        _defines.push_back("CAMERA_BINDING 0");

        _defines.push_back("OBJECT_SET " + std::to_string(set_index++));
        _defines.push_back("OBJECT_BINDING 0");
        
        // ====== FRAGMENT SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::MATERIAL)
            _defines.push_back("MATERIAL_SET " + std::to_string(set_index++));
    }

}
