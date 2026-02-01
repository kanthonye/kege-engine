//
//  shader-pipeline-loader.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#include "shader-pipeline-loader.hpp"
#include "../graphics/render/pipeline/pipeline-loader.hpp"

namespace kege{

    ref::ShaderPipeline ShaderPipelineLoader::load( const std::string& filename )
    {
        // retrieve Graphics before loading
        if (_graphics == nullptr)
        {
            CallbackRequest< kege::Graphics > request(this, &ShaderPipelineLoader::operator() );
            Communication::broadcast< CallbackRequest< kege::Graphics >& >( request );
            if (_graphics == nullptr)
            {
                kege::Log::error << "FAILED: CallbackRequest -> Graphics. Null graphics pointer." << kege::Log::nl;
                return {};
            }
        }
        
        kege::LibraryContext context;
        if ( !loadLibraryContext(context, filename) )
        {
            kege::Log::error << "LOAD_FAILED -> loadLibraryContext(" << filename <<")" << Log::nl;
            return {};
        }

        std::vector< kege::ref::ShaderStructBlock > shader_struct_blocks;
        ref::ShaderPipeline pipeline = createShaderPipeline
        (
            _graphics, shader_struct_blocks, context, 0
        );
        return pipeline;
//
//        Json shader_pipeline_library = JsonParser::load( filename.data() );
//        if ( !shader_pipeline_library )
//        {
//            kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
//            return {};
//        }
//
//        std::filesystem::path path = filename;
//        
//        glsl::LibraryContext context;
//        if( parseShaderPipelineLib( shader_pipeline_library, path.parent_path(), _graphics, &context ) )
//        {
//            return createShaderPipeline( _graphics, context, 0 );
//        }
        return {};
    }

    void ShaderPipelineLoader::operator()( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    ShaderPipelineLoader::ShaderPipelineLoader( AssetManager* am )
    : kege::AssetLoaderT< ref::ShaderPipeline >( am )
    {
    }
}


namespace kege{

    std::vector< ref::ShaderPipeline > ShaderPipelineLibraryLoader::load( const std::string& filename )
    {
        if (_graphics == nullptr)
        {
            CallbackRequest< kege::Graphics > request(this, &ShaderPipelineLibraryLoader::operator() );
            Communication::broadcast< const Request< kege::Graphics >& >({});
            if (_graphics == nullptr)
            {
                kege::Log::error << "FAILED: CallbackRequest -> Graphics. Null graphics pointer." << kege::Log::nl;
                return {};
            }
        }

        Json shader_pipeline_library = JsonParser::load( filename.data() );
        if ( shader_pipeline_library )
        {
            kege::Log::error << "FILE_LOAD_FAILED -> " << filename << Log::nl;
            return {};
        }

        std::string path;
        glsl::LibraryContext context;
        parseShaderPipelineLib( shader_pipeline_library, path, _graphics, &context );

        std::vector< ref::ShaderPipeline > shader_pipelines;
        for ( int pipeline = 0 ; pipeline < context.pipelines.size(); ++pipeline )
        {
            ref::ShaderPipeline physical_pipeline = createShaderPipeline( _graphics, context, pipeline );
            if ( !physical_pipeline ) continue;

            if ( _manager )
            {
                _manager->add< ref::ShaderPipeline >( physical_pipeline->getName(), physical_pipeline );
            }
            
            shader_pipelines.push_back( physical_pipeline );
        }
        return shader_pipelines;
    }

    void ShaderPipelineLibraryLoader::operator()( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    ShaderPipelineLibraryLoader::ShaderPipelineLibraryLoader( AssetManager* am )
    : kege::AssetLoaderT< std::vector< ref::ShaderPipeline > >( am )
    {
    }

}
