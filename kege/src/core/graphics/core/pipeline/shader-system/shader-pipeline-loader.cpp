//
//  shader-pipeline-loader.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#include "../compiler/compile-glsl-to-spv.hpp"
#include "shader-pipeline-loader.hpp"

namespace kege{

    ref::Shader createShader
    (
        kege::GraphicsDevice* graphics,
        kege::ShaderStageFlag stage,
        const std::string& name,
        const std::string& source
    )
    {
        kege::ShaderDesc desc;
        desc.entry_point = "main";
        desc.name = name;
        desc.stage = kege::ShaderStageFlag::Vertex;
        desc.byte_code = kege::compileGlslToSpv
        (
            kege::ShaderStageFlag::Vertex,
            name.c_str(),
            source.c_str(),
            source.length()
        );
        if( desc.byte_code.empty() )
        {
            return 0;
        }
        return graphics->createShader( desc );
    }

    kege::ref::StructBlockDesc createShaderStructBlock
    (
        const std::string& name,
        const std::vector<meta::shader::FieldDecl>& fields
    )
    {
        int offset = 0;
        std::vector< kege::StructBlockMemberDesc > f(fields.size());
        for (int i=0; i<fields.size(); ++i)
        {
            f[i].size = sizeOf( kege::stringToShaderVarType(fields[i].type) );
            f[i].count = fields[i].count;
            f[i].type = fields[i].type;
            f[i].name = fields[i].name;
            f[i].offset = offset;
            offset += f[i].size;
        }
        return new StructBlockDesc( name, f );
    }

    kege::ShaderStageFlag toShaderStageFlag( const std::vector< std::string >& stages )
    {
        kege::ShaderStageFlag flags = ShaderStageFlag::Invalid;
        for (const std::string& s : stages)
        {
            flags |= stringToShaderStageFlag(s);
        }
        return flags;
    }

    ref::ShaderPipeline KMSLShaderLoader::load( const std::string& filename )
    {
        if (_graphics == nullptr) // make sure the required Graphics is set before loading anything
        {
            kege::CallbackRequest< kege::GraphicsDevice > request(this, &KMSLShaderLoader::operator() );
            kege::Communication::broadcast< kege::CallbackRequest< kege::GraphicsDevice >& >( request );
            if (_graphics == nullptr)
            {
                kege::Log::error << "FAILED: CallbackRequest -> Graphics. Null graphics pointer." << kege::Log::nl;
                return {};
            }
        }

        try
        {
            kege::ShaderPipelineDesc shader_pipeline_desc;
            {
                std::vector< kege::ShaderDesc > shader_descs;
                {
                    std::ifstream file( filename );
                    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    file.close();

                    kege::ShaderSources shader_sources;
                    meta::shader::Parser parser(source);
                    parser.parse( shader_pipeline_desc, shader_sources );

                    for (const kege::ShaderSource& src : shader_sources)
                    {
                        shader_descs.push_back
                        ({
                            .byte_code = kege::compileGlslToSpv(src.stage, src.name.c_str(), src.source.c_str(), src.source.length()),
                            .entry_point = src.entry,
                            .stage = src.stage,
                            .name = src.name,
                        });
                    }
                }

                for (const auto& stage : shader_descs)
                {
                    shader_pipeline_desc.shaders.push_back(_graphics->createShader(stage));
                }
            }
            return _graphics->createShaderPipeline( shader_pipeline_desc );
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return {};
        }
    }

    void KMSLShaderLoader::operator()( kege::GraphicsDevice* graphics )
    {
        _graphics = graphics;
    }

    KMSLShaderLoader::KMSLShaderLoader( kege::AssetManager* am )
    : kege::AssetLoaderT< kege::ref::ShaderPipeline >( am )
    {
    }
}


namespace kege{

    std::vector< ref::ShaderPipeline > ShaderPipelineLibraryLoader::load( const std::string& filename )
    {
        if (_graphics == nullptr)
        {
            kege::CallbackRequest< kege::GraphicsDevice > request(this, &kege::ShaderPipelineLibraryLoader::operator() );
            kege::Communication::broadcast< const kege::Request< kege::GraphicsDevice >& >({});
            if (_graphics == nullptr)
            {
                kege::Log::error << "FAILED: CallbackRequest -> Graphics. Null graphics pointer." << kege::Log::nl;
                return {};
            }
        }
        
        return{};
    }

    void ShaderPipelineLibraryLoader::operator()( kege::GraphicsDevice* graphics )
    {
        _graphics = graphics;
    }

    ShaderPipelineLibraryLoader::ShaderPipelineLibraryLoader( kege::AssetManager* am )
    : kege::AssetLoaderT< std::vector< kege::ref::ShaderPipeline > >( am )
    {
    }

}
