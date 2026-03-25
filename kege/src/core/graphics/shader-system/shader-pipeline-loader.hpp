//
//  shader-pipeline-loader.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/1/25.
//

#ifndef shader_pipeline_loader_hpp
#define shader_pipeline_loader_hpp

#include <iostream>
#include <fstream>
#include "../render/core/graphics.hpp"
#include "../../resource/asset-manager.hpp"
#include "../../utils/communication.hpp"
#include "shader-library.hpp"
#include "meta-shader-parser.hpp"

namespace kege{

    class KMSLShaderLoader : public kege::AssetLoaderT< ref::ShaderPipeline >
    {
    public:

        ref::ShaderPipeline load( const std::string& filename );
        void operator()( kege::Graphics* response );

        KMSLShaderLoader( AssetManager* am );

        kege::Graphics* _graphics;
    };
}


namespace kege{

    class ShaderPipelineLibraryLoader : public kege::AssetLoaderT< std::vector< ref::ShaderPipeline > >
    {
    public:

        std::vector< ref::ShaderPipeline > load( const std::string& filename );
        void operator()( kege::Graphics* response );

        ShaderPipelineLibraryLoader( AssetManager* am );

        kege::Graphics* _graphics;
    };

}
#endif /* shader_pipeline_loader_hpp */
