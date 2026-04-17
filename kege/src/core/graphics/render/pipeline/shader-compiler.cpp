//
//  shader-compiler.cpp
//  assets
//
//  Created by Kenneth Esdaile on 2/11/26.
//

//#include "graphics.hpp"
#include "shader-compiler.hpp"

namespace kege{

    bool isNumeric(const std::string& str)
    {
        return std::all_of( str.begin(), str.end(), ::isdigit );
    }
    
    ref::ShaderPipeline ShaderCompiler::compileVariant(const kege::PipelineKey& key)
    {
        _key = key;
        _set_count = 0;
        //_binding_index = 0;
        
        writeFeatures();
        std::string vs = writeVertexShader();
        std::string fs = writeFragmentShader();
        return compileShader(key, vs, fs);
    }

    void ShaderCompiler::getVariant(const kege::PipelineKey& key)
    {
        //auto it = _variant_cache.find(key);
        //if (it != _variant_cache.end())
        //    return it->second;

        // Compile new variant (expensive!)
        //Ref< kege::ShaderPipeline > variant = compileVariant(key);
        //_variant_cache[key] = variant;
        //return variant;
    }

    std::string ShaderCompiler::writeVertexShader()
    {
        std::stringstream source;
        beginVertexShader(source);

        writeVersion(source);
        writeExtensions(source);
        processInput(source);
        processOutput(source);
        processResources(source);
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
        processInput(source);
        processOutput(source);
        processResources(source);
        writeFunctions(source);
        writeMainFn(source);

        endFragmentShader(source);
        return source.str();
    }

    ref::ShaderPipeline ShaderCompiler::compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs)
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


//        kege::ShaderPipelineDesc description;
//
//        description.shaders = std::vector< ref::Shader >
//        {
//            _graphics->createShader(kege::ShaderDesc{
//                .stage = kege::ShaderStageFlag::Vertex,
//                .byte_code = _graphics->compileGlslToSpv
//                (
//                    kege::ShaderStageFlag::Vertex,
//                    "name", vs.c_str(), vs.length()
//                ),
//                .entry_point = "main",
//                .name = "",
//            }),
//            _graphics->createShader(kege::ShaderDesc{
//                .stage = kege::ShaderStageFlag::Fragment,
//                .byte_code = _graphics->compileGlslToSpv
//                (
//                    kege::ShaderStageFlag::Fragment,
//                    "name", fs.c_str(), fs.length()
//                ),
//                .entry_point = "main",
//                .name = "",
//            }),
//        };
//
//        description.input_assembly.topology = key.topology;
//        description.depth_stencil = key.depth_state;
//        description.rasterizer = key.raster_state;
//        description.features = key.features;
//
//        _curr_stage = &_stages[ kege::ShaderStageFlag::Vertex ];
//        description.vertex_layout.signature = 0;
//        uint32_t stride = 0;
//        for (auto a : _curr_stage->input)
//        {
//            VertexAttribute va;
//            va.name = a.name;
//            va.binding = 0;
//            va.semantic = resolveVertexBitType( a.name );
//            va.location = resolveDefnInt(_curr_stage, a.location);
//            va.type = stringToShaderVarType(a.type);
//
//            description.vertex_layout.attributes.push_back(va);
//            description.vertex_layout.signature |= va.semantic;
//
//            stride += sizeOf( va.type );
//        }
//        description.vertex_layout.input_rates.push_back({
//            .index = 0,
//            .stride = stride,
//            .input_rate = kege::VertexInputRate::Vertex,
//        });
//
//        for (auto m = _bind_set_descs.begin(); m != _bind_set_descs.end(); m++)
//        {
//            description.shader_layout.bind_sets.push_back( m->second );
//        }
//
//        switch ( _key.render_pass )
//        {
//            default:
//            {
//                description.pipeline_rendering.color_attachment_formats.push_back(kege::Format::bgra_8_srbg);
//                description.pipeline_rendering.depth_attachment_format = kege::Format::depth_32;
//                description.color_blend.attachments.push_back( kege::BlendStatePreset::alphaBlend() );
//                description.color_blend.logic_op_enable = true;
//                break;
//            }
//            case kege::RenderPass::GBuffer:
//            {
//                // position.x | position.y | position.z | metallic
//                description.pipeline_rendering.color_attachment_formats.push_back(kege::Format::rgba_f32);
//                // normal.x   | normal.y   | normal.z   | roughness
//                description.pipeline_rendering.color_attachment_formats.push_back(kege::Format::rgba_f16);
//                // albedo.x   | albedo.y   | albedo.z   | ambient-occlusion
//                description.pipeline_rendering.color_attachment_formats.push_back(kege::Format::rgba_u8_norm);
//                description.pipeline_rendering.depth_attachment_format = kege::Format::depth_32;
//
//                description.color_blend.logic_op_enable = false;
//                break;
//            }
//            case kege::RenderPass::Lighting:
//            {
//                description.pipeline_rendering.color_attachment_formats.push_back(kege::Format::rgba_f16);
//                description.color_blend.attachments.push_back( kege::BlendStatePreset::alphaBlend() );
//                description.color_blend.logic_op_enable = true;
//                break;
//            }
//        }
//        return _graphics->createShaderPipeline(description);
        return nullptr;
    }

    void ShaderCompiler::addAttribute(kege::VertexBit vertex_bit, int location, kege::ShaderVar type, const char* name )
    {
//        _vertex_layout.attributes.push_back(kege::VertexAttribute{
//            .name = name,
//            .location = uint8_t(location),
//            .binding = uint8_t(0),
//            .type = type,
//            .semantic = vertex_bit
//        });
//        _inputs.push_back({ vertex_bit, location });
    }

    void ShaderCompiler::beginVertexShader(std::stringstream& source)
    {
        source << "// Vertex Shader: automatically generated by KEGE based on pipeline key.\n\n";
        _curr_stage = &_stages[ kege::ShaderStageFlag::Vertex ];
        _curr_stage->type = kege::ShaderStageFlag::Vertex;
    }

    void ShaderCompiler::endVertexShader(std::stringstream& source){
//        _inputs = _outputs;
//        _outputs = {};
    }

    void ShaderCompiler::beginFragmentShader(std::stringstream& source)
    {
        source << "// Fragment Shader: automatically generated by KEGE based on pipeline key.\n\n";
        _curr_stage = &_stages[ kege::ShaderStageFlag::Fragment ];
        _curr_stage->type = kege::ShaderStageFlag::Fragment;

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
    }

    void ShaderCompiler::endFragmentShader(std::stringstream& source)
    {
//        _inputs = _outputs;
//        _outputs = {};
    }

    void ShaderCompiler::writeVersion(std::stringstream& source)
    {
        source << "#version 450\n";
    }

    void ShaderCompiler::writeExtensions(std::stringstream& source)
    {
        source << "#extension GL_ARB_separate_shader_objects : enable\n\n";
    }

    void ShaderCompiler::writeResource(std::stringstream& source, const std::string& fname)
    {
        const std::string& snippet = include(fname);

        uint32_t binding_index = 0;
        SetMeta* meta = nullptr;

        ShaderReflection sr;
        //sr.add_define("", 0)
        sr.parse_source( snippet );

        const std::vector<ShaderResource>& resources = sr.get_resources();
        for (const ShaderResource& res : resources)
        {
            auto iset = _set_meta_map.find( res.set );
            if (iset == _set_meta_map.end())
            {
                meta = &_set_meta_map[ res.set ];
                // if the set index is a numeric value, get index.
                if ( isNumeric(res.set) )
                {
                    meta->set = static_cast< uint32_t >( std::stoul( res.set ) );
                }
                else
                {
                    meta->set = _set_count++;
                }
                // define it in the shader to prevent duplicate macros
                _curr_stage->defined_sets.insert( res.set );
                define(source, res.set, meta->set);
            }
            else
            {
                meta = &iset->second;
                // if the set was not define in the current shader stage define add it.
                if(_curr_stage->defined_sets.end() == _curr_stage->defined_sets.find( res.set ))
                {
                    define( source, res.set, meta->set );
                }
            }

            // if the set binding index is a numeric value, get binding index.
            if ( !isNumeric( res.binding ) )
            {
                auto b = meta->bindings.find(res.binding);
                if (b == meta->bindings.end())
                {
                    uint32_t next_index = static_cast< uint32_t >( meta->bindings.size() );
                    uint32_t* binding = &meta->bindings[ res.binding ];
                    // if the set binding index is a numeric value, get binding index.
                    if (isNumeric(res.binding))
                    {
                        *binding = static_cast< uint32_t >( std::stoul( res.binding ) );
                    }
                    else
                    {
                        *binding = next_index;
                    }
                    define(source, res.binding, *binding);
                }
                else
                {
                    define(source, res.binding, b->second);
                }
                binding_index = b->second;
            }
            else
            {
                binding_index = static_cast< uint32_t >( std::stoul( res.binding ) );
            }

            
            kege::Ref< kege::ShaderStructBlock > block;
            if( !res.members.empty() )
            {
                std::vector<kege::ShaderStructField> fields;
                for (auto& m : res.members)
                {
                    fields.push_back
                    ({
                        .count = (m.array_size != 0)? m.array_size : 1,
                        .type = m.type,
                        .name = m.name,
                        .size = static_cast<uint32_t>( kege::sizeOf( kege::stringToShaderVarType(m.type) ) )
                    });
                }
                block = new ShaderStructBlock(ShaderStructType::Buffer, res.struct_type, fields);
            }

            if( res.type != kege::ShaderResourceType::PushConstant )
            {
                BindPointDesc bind_point;
                bind_point.name = res.name;
                bind_point.stages |= _curr_stage->type;
                bind_point.block = block;
                bind_point.index = binding_index;
                bind_point.count = kege::max<uint32_t>(1, sr.resolve_int(res.count));

                switch (res.type)
                {
                    case ShaderResourceType::UniformBuffer: bind_point.usage = kege::BindingUsage::UniformBuffer;
                        break;
                    case ShaderResourceType::StorageBuffer: bind_point.usage = kege::BindingUsage::StorageBuffer;
                        break;
                    case ShaderResourceType::StorageImage: bind_point.usage = kege::BindingUsage::StorageImage;
                        break;
                    case ShaderResourceType::SampledImage: bind_point.usage = kege::BindingUsage::SampledImage;
                        break;
                    case ShaderResourceType::Sampler: bind_point.usage = kege::BindingUsage::Sampler;
                        break;
                    default: break;
                }
                
                auto m = _bind_set_descs.find( meta->set );
                if (m == _bind_set_descs.end())
                {
                    BindSetDesc& bind_set = _bind_set_descs[ meta->set ];
                    bind_set.bindings.push_back(bind_point);
                    bind_set.name = res.block_name;
                    bind_set.index = meta->set;
                }
                else
                {
                    m->second.bindings.push_back(bind_point);
                }
            }
            else
            {
                PushBlockDesc push_block = {};
                push_block.name = res.name;
                push_block.size = res.block_size;
                push_block.stages = _curr_stage->type;
                push_block.block = block;
                push_block.count = 1;
                _push_blocks.push_back(push_block);
            }
        }
        source << snippet;
    }

    void ShaderCompiler::writeOutputs(std::stringstream& source, const std::string& fname)
    {
        std::string snippet = include("output/" + fname);

        ShaderReflection sr;
        sr.parse_source( snippet );

        const std::vector<ShaderIO>& outputs = sr.get_outputs();
        for (const ShaderIO& io : outputs)
        {
            _curr_stage->output.push_back(io);
            if (!std::all_of(io.location.begin(), io.location.end(), ::isdigit))
            {
                define(source, io.location, _output_location);

                if(io.location.compare(0, 3, "OUT_"))
                {
                    const char* str = io.location.c_str();
                    str += 4;
                    std::string in = std::string("IN_") + str;
                    _io_defs[ in ] = _output_location;
                }
                _output_location += 1;
            }
        }
        source << snippet;

        _output_fnames.push_back(fname);
    }

    void ShaderCompiler::writeInputs(std::stringstream& source, const std::string& fname)
    {
        const std::string& snippet = include("input/" + fname);
        {
            ShaderReflection sr;
            sr.parse_source( snippet );

            const std::vector<ShaderIO>& inputs = sr.get_inputs();
            for (const ShaderIO& io : inputs)
            {
                if (!std::all_of(io.location.begin(), io.location.end(), ::isdigit))
                {
                    _curr_stage->input.push_back(io);
                    auto a = _io_defs.find(io.location);
                    if(a == _io_defs.end())
                    {
                        _curr_stage->defines[ io.location ] = _input_location;
                        define(source, io.location, _input_location++);
                    }
                    else
                    {
                        define(source, io.location, a->second);
                    }
                }
            }
        }
        source << snippet;
    }

    void ShaderCompiler::processInput(std::stringstream& source)
    {
        if(_curr_stage->type == kege::ShaderStageFlag::Vertex)
        {
            switch (_key.renderer_type)
            {
                case kege::MeshType::StaticMesh:
                    writeInputs(source, "static-mesh.glsl");
                    break;

                default:
                case kege::MeshType::Point:
                    writeInputs(source, "point.glsl");
                    break;

                case kege::MeshType::ScreenRect:
                    writeInputs(source, "screen-rect.glsl");
                    break;

                case kege::MeshType::Particle:
                    writeInputs(source, "particle.glsl");
                    break;

                case kege::MeshType::FlatTerrain:
                    writeInputs(source, "flat-terrain.glsl");
                    break;

                case kege::MeshType::SphericalTerrain:
                    writeInputs(source, "spherical-terrain.glsl");
                    break;
            }

            if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
            {
                writeInputs(source, "normal-mapping.glsl");
            }
            if (_key.features & kege::FeatureFlag::GPU_SKINNING)
            {
                writeInputs(source, "gpu-skinning.glsl");
            }

            if (_key.features == kege::FeatureFlag::VERTEX_COLOR)
            {
                writeInputs(source, "vertex-color.glsl");
            }
        }
        else if(_curr_stage->type == kege::ShaderStageFlag::Fragment)
        {
            for(const std::string& input : _output_fnames)
            {
                writeInputs(source, input);
            }
            _output_fnames = {};
        }
        source << "\n";
    }

    void ShaderCompiler::processOutput(std::stringstream& source)
    {
        if(_curr_stage->type == kege::ShaderStageFlag::Vertex)
        {
            switch (_key.renderer_type)
            {
                case kege::MeshType::StaticMesh:
                    writeOutputs(source, "static-mesh.glsl");
                    break;

                default:
                case kege::MeshType::Point:
                    writeOutputs(source, "point.glsl");
                    break;

                case kege::MeshType::ScreenRect:
                    writeOutputs(source, "screen-rect.glsl");
                    break;

                case kege::MeshType::Particle:
                    writeOutputs(source, "particle.glsl");
                    break;

                case kege::MeshType::FlatTerrain:
                    writeOutputs(source, "flat-terrain.glsl");
                    break;

                case kege::MeshType::SphericalTerrain:
                    writeOutputs(source, "spherical-terrain.glsl");
                    break;
            }

            if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
            {
                writeOutputs(source, "normal-mapping.glsl");
            }

            if (_key.features == kege::FeatureFlag::VERTEX_COLOR)
            {
                writeOutputs(source, "vertex-color.glsl");
            }
        }
        else if(_curr_stage->type == kege::ShaderStageFlag::Fragment)
        {
            switch ( _key.render_pass )
            {
                default:
                {
                    writeOutputs(source, "color-buffer.glsl");
                    break;
                }
                case kege::RenderPass::GBuffer:
                {
                    writeOutputs(source, "geometry-buffer.glsl");
                    break;
                }
                case kege::RenderPass::Lighting:
                {
                    writeOutputs(source, "hd-color-buffer.glsl");
                    break;
                }
            }
        }
        source << "\n";
    }

    void ShaderCompiler::processResources(std::stringstream& source)
    {
        writeResource(source, "resources/uniform-camera-data.glsl");
        writeResource(source, "resources/uniform-object-data.glsl");

        if(_curr_stage->type == kege::ShaderStageFlag::Fragment)
        {
            if ((_key.features & kege::FeatureFlag::MATERIAL))
            {
                writeResource(source, "resources/uniform-material.glsl");
            }

            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                writeResource(source, "resources/uniform-textures-ibl.glsl");
            }

            if ((_key.features & kege::FeatureFlag::DIRECTIONAL_LIGHT))
            {
                writeResource(source, "resources/uniform-directional-light.glsl");
            }
            if ((_key.features & kege::FeatureFlag::POINT_LIGHT))
            {
                writeResource(source, "resources/uniform-point-light.glsl");
            }
            if ((_key.features & kege::FeatureFlag::SPOT_LIGHT))
            {
                writeResource(source, "resources/uniform-spot-light.glsl");
            }
        }
    }

    void ShaderCompiler::writeFunctions(std::stringstream& source)
    {
        if(_curr_stage->type == kege::ShaderStageFlag::Fragment)
        {
            if( _has_material )
            {
                source << include( "functions/fn-get-albedo.glsl") <<"\n";
                source << include( "functions/fn-get-roughness.glsl") <<"\n";
                source << include( "functions/fn-get-metallic.glsl") <<"\n";
                source << include( "functions/fn-get-opacity.glsl") <<"\n";
                source << include( "functions/fn-get-ambient-occlusion.glsl") <<"\n";
                source << include( "functions/fn-get-emissive.glsl") <<"\n";

                if (_has_normal_mapping)
                {
                    source << include( "functions/fn-get-tbn-normal.glsl") <<"\n";
                }
                else
                {
                    source << include( "functions/fn-get-normal.glsl") <<"\n";
                }

                if ((_key.features & kege::FeatureFlag::ANISOTROPIC_REFLECTION))
                {
                    source << include( "functions/fn-anisotropy.glsl") <<"\n";
                }

                if ((_key.features & kege::FeatureFlag::CLEAR_COAT))
                {
                    source << include( "functions/fn-clear-coat.glsl") <<"\n";
                }
            }

            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                source << include( "functions/fn-calc-ambient-ibl.glsl") <<"\n";
            }

            if (_key.shading_model == kege::ShadingModel::LitPBR_Metallic ||
                _key.shading_model == kege::ShadingModel::LitPBR_Skin ||
                _key.shading_model == kege::ShadingModel::LitPBR_Hair ||
                _key.shading_model == kege::ShadingModel::LitPBR_Cloth ||
                _key.shading_model == kege::ShadingModel::LitPBR_Subsurface ||
                _key.shading_model == kege::ShadingModel::LitPBR_Clearcoat )
            {
                source << include( "functions/fn-brdf.glsl") <<"\n";
            }

            if (_key.shading_model == kege::ShadingModel::LitPBR_Metallic)
            {
                source << include( "functions/fn-lighting.glsl") <<"\n";
            }

            if ((_key.features & kege::FeatureFlag::IMAGE_BASE_LIGHTING))
            {
                source << include( "functions/fn-calc-ambient-ibl.glsl") <<"\n";
            }
        }
    }
    
    void ShaderCompiler::writeMainFn(std::stringstream& source)
    {
        source <<"\n";
        source << "void main() {\n";
        switch ( _curr_stage->type )
        {
            case kege::ShaderStageFlag::Vertex:
            {
                source << "    // Process vertex based on renderer type\n";

                if (_key.renderer_type == kege::MeshType::StaticMesh && (_key.features & kege::FeatureFlag::GPU_SKINNING))
                {
                    source << include( "procedures/calc-skin-matrix.glsl") <<"\n";
                    source << include( "procedures/write-skin-pnt.glsl") <<"\n";
                    if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
                    {
                        source << include( "procedures/write-skin-tang-bitang.glsl") <<"\n";
                    }
                }
                else if (_key.renderer_type == kege::MeshType::StaticMesh)
                {
                    source << include( "procedures/write-pos-norm-texc.glsl") <<"\n";
                    if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
                    {
                        source << include( "procedures/write-tang-bitang.glsl") <<"\n";
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
                    source << include( "procedures/write-vertex-color.glsl") <<"\n";
                }

                source << include( "procedures/write-vertex-position.glsl") <<"\n";
            }
            break;

            case kege::ShaderStageFlag::Fragment:
            {
                source << include( "procedures/def-v3-final-color.glsl") <<"\n";
                if( _has_light_funct )
                {
                    source << include( "procedures/def-v3-lighting.glsl") <<"\n";
                }
                if(( _input_signature & VertexBit::POSITION ))
                {
                    source << "    vec3 position = in_position;\n";
                }
                if( _has_material )
                {
                    source << include( "procedures/get-albedo.glsl") <<"\n";
                    if (_has_normal_mapping)
                    {
                        source << include( "procedures/get-normal.glsl") <<"\n";
                    }
                }
                if( _has_light_funct )
                {
                    if( _has_material && _has_lights )
                    {
                        source << include( "procedures/get-opacity.glsl") <<"\n";
                        source << include( "procedures/get-ambient-occlusion.glsl") <<"\n";
                        source << include( "procedures/get-roughness.glsl") <<"\n";
                        source << include( "procedures/get-metallic.glsl") <<"\n";
                        source << include( "procedures/get-normal.glsl") <<"\n";
                        source << include( "procedures/get-emissive.glsl") <<"\n";
                    }
                    else
                    {
                        source << include( "procedures/use-internal-material.glsl") <<"\n";
                        if(( _input_signature & (1 << VertexBit::NORMAL) ))
                        {
                            source << include( "resources/get-input-normal.glsl") <<"\n";
                        }
                    }
                    
                    if(( _input_signature & (1 << VertexBit::POSITION) ))
                    {
                        source << "    vec3 view_dir = normalize(camera.cameraPosition - in_position);\n";
                    }

                    if( _has_lights )
                    {
                        if ((_key.features & kege::FeatureFlag::DIRECTIONAL_LIGHT))
                        {
                            source << include( "procedures/integrate-directional-light.glsl") <<"\n";
                        }
                        if ((_key.features & kege::FeatureFlag::POINT_LIGHT))
                        {
                            source << include( "procedures/integrate-point-light.glsl") <<"\n\n";
                        }
                        if ((_key.features & kege::FeatureFlag::SPOT_LIGHT))
                        {
                            source << include( "procedures/integrate-spot-light.glsl") <<"\n\n";
                        }
                    }
                    else
                    {
                        source << include( "procedures/use-internal-light.glsl") <<"\n";
                    }
                }
                else if (_key.shading_model == kege::ShadingModel::Unlit)
                {
                    if( _has_material )
                    {
                        source << "    vec3 final_color = getAlbedo();\n";
                    }
                    if(( _input_signature & VertexBit::COLOR ))
                    {
                        source << "    vec3 final_color = in_color.rgb;\n";
                    }
                }

                if ((_key.features & kege::FeatureFlag::TONE_MAPPING))
                {
                    source << include( "procedures/tone-mapping.glsl") <<"\n\n";
                }
                
                if ((_key.features & kege::FeatureFlag::GAMMA))
                {
                    source << include( "procedures/gamma.glsl") <<"\n\n";
                }

                switch (_key.render_pass)
                {
                    default:
                    case RenderPass::Forward:
                        source << include( "procedures/write-to-color-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::GBuffer:
                        source << include( "procedures/write-to-geometry-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::Shadow:
                        source << include( "procedures/write-to-depth-buffer.glsl") <<"\n";
                        break;

                    case RenderPass::DepthPrePass:
                        source << include( "procedures/write-to-depth-buffer.glsl") <<"\n";
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
        const std::string filename = _filepath + fname;
        std::ifstream file(filename);
        std::string loaded_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        if (loaded_source.empty())
        {
            kege::Log::error <<"file not found " << fname <<"\n";
        }
        return loaded_source;
    }

    void ShaderCompiler::define(std::stringstream& source, const std::string& name, int index)
    {
        source << "#define " << name <<" " << index << "\n";
    }

    void ShaderCompiler::addDefine(Defines& defines, const std::string& name, int index)
    {
        if( defines.table.find(name) != defines.table.end()) return;
        defines.table[ name ] = defines.defs.size();
        defines.defs.push_back({name, index});
    }

    kege::VertexBit ShaderCompiler::resolveVertexBitType( const std::string& str )
    {
        auto it = _vertex_bit_string_map.find( str );
        if (it != _vertex_bit_string_map.end()) return it->second;
        return kege::VertexBit::POSITION;
    }

    int ShaderCompiler::resolveDefnInt(Stage* stage, const std::string& str)
    {
        std::string e = str;
        if (isNumeric(str)) {
            return static_cast<uint32_t>( std::stoul( str ) );
        }
        auto it = stage->defines.find(e);
        if (it != stage->defines.end()) return it->second;
        return 0;
    }


    void ShaderCompiler::writeFeatures()
    {
        _vertex_bit_string_map["in_position"] = kege::VertexBit::POSITION;
        _vertex_bit_string_map["in_texcoord"] = kege::VertexBit::TEXCOORD;
        _vertex_bit_string_map["in_normal"] = kege::VertexBit::NORMAL;
        _vertex_bit_string_map["in_tangent"] = kege::VertexBit::TANGENT;
        _vertex_bit_string_map["in_bitangent"] = kege::VertexBit::BITANGENT;
        _vertex_bit_string_map["in_weights"] = kege::VertexBit::WEIGHTS;
        _vertex_bit_string_map["in_joints"] = kege::VertexBit::JOINTS;
        _vertex_bit_string_map["in_color"] = kege::VertexBit::COLOR;

        // ====== VERTEX SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            addDefine(_global_defines, "FEATURE_VERTEX_ANIMATION");
        if (_key.features & kege::FeatureFlag::MORPH_TARGETS)
            addDefine(_global_defines, "FEATURE_MORPH_TARGETS");
        if (_key.features & kege::FeatureFlag::GPU_SKINNING)
            addDefine(_global_defines, "FEATURE_GPU_SKINNING");
        if (_key.features & kege::FeatureFlag::VERTEX_DISPLACEMENT)
            addDefine(_global_defines, "FEATURE_VERTEX_DISPLACEMENT");
        if (_key.features & kege::FeatureFlag::WIND_ANIMATION)
            addDefine(_global_defines, "FEATURE_WIND_ANIMATION");
        if (_key.features & kege::FeatureFlag::VERTEX_ANIMATION)
            addDefine(_global_defines, "FEATURE_VERTEX_ANIMATION");
        if (_key.features & kege::FeatureFlag::INSTANCED_RENDERING)
            addDefine(_global_defines, "FEATURE_INSTANCED_RENDERING");
        if (_key.features & kege::FeatureFlag::VERTEX_COLOR)
            addDefine(_global_defines, "FEATURE_VERTEX_COLOR");

        // ====== TESSELLATION FEATURES ======
        if (_key.features & kege::FeatureFlag::TESSELLATION)
            addDefine(_global_defines, "FEATURE_TESSELLATION");
        if (_key.features & kege::FeatureFlag::PN_TRIANGLES)
            addDefine(_global_defines, "FEATURE_PN_TRIANGLES");
        if (_key.features & kege::FeatureFlag::TESSELLATION_DISPLACEMENT)
            addDefine(_global_defines, "FEATURE_DISPLACEMENT_TESSELLATION");

        // ====== FRAGMENT SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::NORMAL_MAPPING)
            addDefine(_global_defines, "FEATURE_NORMAL_MAPPING");
        if (_key.features & kege::FeatureFlag::TRIPLANAR_MAPPING)
            addDefine(_global_defines, "FEATURE_TRIPLANAR_MAPPING");
        if (_key.features & kege::FeatureFlag::PARALLAX_MAPPING)
            addDefine(_global_defines, "FEATURE_PARALLAX_MAPPING");
        if (_key.features & kege::FeatureFlag::PARALLAX_OCCLUSION)
            addDefine(_global_defines, "FEATURE_PARALLAX_OCCLUSION");

        // ====== MATERIAL LAYERS ======
        if (_key.features & kege::FeatureFlag::CLEAR_COAT)
            addDefine(_global_defines, "FEATURE_CLEAR_COAT");
        if (_key.features & kege::FeatureFlag::ANISOTROPIC_REFLECTION)
            addDefine(_global_defines, "FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::SHEEN_LAYER)
            addDefine(_global_defines, "FEATURE_SHEEN_LAYER");
        if (_key.features & kege::FeatureFlag::THIN_FILM)
            addDefine(_global_defines, "FEATURE_THIN_FILM");
        if (_key.features & kege::FeatureFlag::TRANSMISSION)
            addDefine(_global_defines, "FEATURE_TRANSMISSION");
        if (_key.features & kege::FeatureFlag::SUBSURFACE_SCATTERING)
            addDefine(_global_defines, "FEATURE_SUBSURFACE_SCATTERING");

        // ====== TRANSPARENCY/ALPHA ======
        if (_key.features & kege::FeatureFlag::ALPHA_TEST)
            addDefine(_global_defines, "FEATURE_ALPHA_TEST");
        if (_key.features & kege::FeatureFlag::ALPHA_TO_COVERAGE)
            addDefine(_global_defines, "FEATURE_ALPHA_TO_COVERAGE");
        if (_key.features & kege::FeatureFlag::DITHERED_OPACITY)
            addDefine(_global_defines, "FEATURE_DITHERED_OPACITY");
        if (_key.features & kege::FeatureFlag::PREMULTIPLIED_ALPHA)
            addDefine(_global_defines, "FEATURE_PREMULTIPLIED_ALPHA");

        // ====== PARTICLE EFFECTS ======
        if (_key.features & kege::FeatureFlag::SOFT_PARTICLES)
            addDefine(_global_defines, "FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::PARTICLE_ROTATION)
            addDefine(_global_defines, "FEATURE_PARTICLE_ROTATION");

        // ====== ENVIRONMENT INTERACTION ======
        if (_key.features & kege::FeatureFlag::SCREEN_SPACE_REFLECTION)
            addDefine(_global_defines, "FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::REFRACTION)
            addDefine(_global_defines, "FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::WATER_EFFECTS)
            addDefine(_global_defines, "FEATURE_ANISOTROPIC_REFLECTION");
        if (_key.features & kege::FeatureFlag::DECAL)
            addDefine(_global_defines, "FEATURE_DECAL");

        // ====== LIGHTING FEATURES ======
        if (_key.features & kege::FeatureFlag::MULTIPLE_BOUNCE_GI)
            addDefine(_global_defines, "FEATURE_MULTIPLE_BOUNCE_GI");
        if (_key.features & kege::FeatureFlag::AREA_LIGHTS)
            addDefine(_global_defines, "FEATURE_AREA_LIGHTS");
        if (_key.features & kege::FeatureFlag::CONTACT_SHADOWS)
            addDefine(_global_defines, "FEATURE_CONTACT_SHADOWS");
        if (_key.features & kege::FeatureFlag::VOLUMETRIC_LIGHTING)
            addDefine(_global_defines, "FEATURE_VOLUMETRIC_LIGHTING");


        // ====== POST-PROCESS/SCREEN SPACE ======
        if (_key.features & kege::FeatureFlag::MOTION_VECTORS)
            addDefine(_global_defines, "FEATURE_MOTION_VECTORS");
        if (_key.features & kege::FeatureFlag::VELOCITY_BUFFER)
            addDefine(_global_defines, "FEATURE_VELOCITY_BUFFER");
        if (_key.features & kege::FeatureFlag::DEPTH_ONLY)
            addDefine(_global_defines, "FEATURE_DEPTH_ONLY");
        if (_key.features & kege::FeatureFlag::CUSTOM_DEPTH)
            addDefine(_global_defines, "FEATURE_CUSTOM_DEPTH");

        // ====== TERRAIN/VEGETATION ======
        if (_key.features & kege::FeatureFlag::TERRAIN_BLENDING)
            addDefine(_global_defines, "FEATURE_TERRAIN_BLENDING");
        if (_key.features & kege::FeatureFlag::VERTEX_GRASS_WIND)
            addDefine(_global_defines, "FEATURE_VERTEX_GRASS_WIND");
        if (_key.features & kege::FeatureFlag::LOD_CROSSFADE)
            addDefine(_global_defines, "FEATURE_LOD_CROSSFADE");
        if (_key.features & kege::FeatureFlag::DECAL_LAYER)
            addDefine(_global_defines, "FEATURE_DECAL_LAYER");

        // ====== DEBUG/DEVELOPMENT ======
        if (_key.features & kege::FeatureFlag::WIREFRAME_OVERLAY)
            addDefine(_global_defines, "FEATURE_WIREFRAME_OVERLAY");
        if (_key.features & kege::FeatureFlag::DEBUG_NORMALS)
            addDefine(_global_defines, "FEATURE_DEBUG_NORMALS");
        if (_key.features & kege::FeatureFlag::DEBUG_UVS)
            addDefine(_global_defines, "FEATURE_DEBUG_UVS");
        if (_key.features & kege::FeatureFlag::DEBUG_TANGENTS)
            addDefine(_global_defines, "FEATURE_DEBUG_TANGENTS");

        // ====== SHADOW FEATURES ======
        if (_key.features & kege::FeatureFlag::CONTACT_HARDENING)
            addDefine(_global_defines, "FEATURE_CONTACT_HARDENING");
        if (_key.features & kege::FeatureFlag::VARIANCE_SHADOW_MAP)
            addDefine(_global_defines, "FEATURE_VARIANCE_SHADOW_MAP");
        if (_key.features & kege::FeatureFlag::CASCADE_BLENDING)
            addDefine(_global_defines, "FEATURE_CASCADE_BLENDING");

        // ====== ADVANCED RENDERING ======
        if (_key.features & kege::FeatureFlag::RAY_TRACING_OVERRIDE)
            addDefine(_global_defines, "FEATURE_RAY_TRACING_OVERRIDE");
        if (_key.features & kege::FeatureFlag::VIRTUAL_TEXTURE)
            addDefine(_global_defines, "FEATURE_VIRTUAL_TEXTURE");
        if (_key.features & kege::FeatureFlag::FLIPBOOK_ANIMATION)
            addDefine(_global_defines, "FEATURE_FLIPBOOK_ANIMATION");

        // ====== UI RENDERING ======
        if (_key.features & kege::FeatureFlag::ROUNDED_UI_CORNERS)
            addDefine(_global_defines, "FEATURE_ROUNDED_UI_CORNERS");
        if (_key.features & kege::FeatureFlag::SINE_DIST_TEXT)
            addDefine(_global_defines, "FEATURE_SINE_DIST_TEXT");

        // ====== FRAGMENT SHADER FEATURES ======
        if (_key.features & kege::FeatureFlag::DETAIL_NORMAL)
            addDefine(_global_defines, "FEATURE_DETAIL_NORMAL");
    }

    ShaderCompiler::ShaderCompiler(kege::Graphics* graphics, const std::string& filepath)
    :   _graphics(graphics)
    ,   _filepath(filepath)
    {}
}
