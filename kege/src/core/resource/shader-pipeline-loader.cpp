//
//  shader-pipeline-loader.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#include "shader-pipeline-loader.hpp"
#include "../graphics/render/pipeline/pipeline-loader.hpp"

namespace kege{

    void ShaderPipelineLoader::operator()( const kege::Response< kege::Graphics* >& response )
    {
        _graphics = response.response;
    }

    ref::ShaderPipeline ShaderPipelineLoader::load( const std::string& filename )
    {
        Json shader_pipeline_library = JsonParser::load( filename.data() );
        if ( shader_pipeline_library )
        {
            kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
            return {};
        }

        std::string path;
        ShaderPipelineLibContext context;
        std::vector< PipelinInfo > pipelines;
        parseShaderPipelineLib( path, _graphics, shader_pipeline_library, &context, &pipelines );

        ref::ShaderPipeline physical_pipeline = createShaderPipeline( _graphics, context, pipelines[0] );
        if ( physical_pipeline )
        {
            _manager->add< ref::ShaderPipeline >( physical_pipeline->getName(), physical_pipeline );
        }
        return physical_pipeline;
    }

    ShaderPipelineLoader::ShaderPipelineLoader( AssetManager* am )
    : kege::AssetLoaderT< ref::ShaderPipeline >( am )
    {
        Communication::broadcast< const Request< kege::Graphics* >& >({});
    }

}


namespace kege{

    std::vector< ref::ShaderPipeline > ShaderPipelineLibraryLoader::load( const std::string& filename )
    {
        Json shader_pipeline_library = JsonParser::load( filename.data() );
        if ( shader_pipeline_library )
        {
            kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
            return {};
        }

        std::string path;
        ShaderPipelineLibContext context;
        std::vector< PipelinInfo > pipelines;
        parseShaderPipelineLib( path, _graphics, shader_pipeline_library, &context, &pipelines );

        std::vector< ref::ShaderPipeline > shader_pipelines;
        for (const PipelinInfo& pipeline : pipelines )
        {
            ref::ShaderPipeline physical_pipeline = createShaderPipeline( _graphics, context, pipeline );
            if ( !physical_pipeline )
            {
                continue;
            }
            _manager->add< ref::ShaderPipeline >( physical_pipeline->getName(), physical_pipeline );
            shader_pipelines.push_back( physical_pipeline );
        }
        return shader_pipelines;
    }

    void ShaderPipelineLibraryLoader::operator()( const kege::Response< kege::Graphics* >& response )
    {
        _graphics = response.response;
    }

    ShaderPipelineLibraryLoader::ShaderPipelineLibraryLoader( AssetManager* am )
    : kege::AssetLoaderT< std::vector< ref::ShaderPipeline > >( am )
    {
        Communication::broadcast< const Request< kege::Graphics* >& >({});
    }

}
