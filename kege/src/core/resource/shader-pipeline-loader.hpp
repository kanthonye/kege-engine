//
//  shader-pipeline-loader.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#ifndef shader_pipeline_loader_hpp
#define shader_pipeline_loader_hpp

#include "asset-manager.hpp"
#include "../utils/communication.hpp"
#include "../graphics/render/pipeline/shader-pipeline.hpp"

namespace kege{

    template< typename T > struct Request
    {
        T request;
    };
    template< typename T > struct Response
    {
        T response;
    };

    class ShaderPipelineLoader : public kege::AssetLoaderT< ref::ShaderPipeline >
    {
    public:

        void operator()( const kege::Response< kege::Graphics* >& response );

        ref::ShaderPipeline load( const std::string& filename );
        ShaderPipelineLoader( AssetManager* am );

        kege::Graphics* _graphics;
    };
}


namespace kege{

    class ShaderPipelineLibraryLoader : public kege::AssetLoaderT< std::vector< ref::ShaderPipeline > >
    {
    public:

        std::vector< ref::ShaderPipeline > load( const std::string& filename );
        void operator()( const kege::Response< kege::Graphics* >& response );
        ShaderPipelineLibraryLoader( AssetManager* am );
        kege::Graphics* _graphics;
    };

}
#endif /* shader_pipeline_loader_hpp */
