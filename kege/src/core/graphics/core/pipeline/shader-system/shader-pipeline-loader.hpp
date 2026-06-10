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
#include "../../device/core/device.hpp"
#include "../../../../resource/asset-manager.hpp"
#include "../../../../utils/communication.hpp"
#include "meta-shader-parser.hpp"

namespace kege{

    class KMSLShaderLoader : public kege::AssetLoaderT< ref::ShaderPipeline >
    {
    public:

        ref::ShaderPipeline load( const std::string& filename );
        void operator()( kege::GraphicsDevice* response );

        KMSLShaderLoader( AssetManager* am );

        kege::GraphicsDevice* _graphics;
    };
}


namespace kege{

    class ShaderPipelineLibraryLoader : public kege::AssetLoaderT< std::vector< ref::ShaderPipeline > >
    {
    public:

        std::vector< ref::ShaderPipeline > load( const std::string& filename );
        void operator()( kege::GraphicsDevice* response );

        ShaderPipelineLibraryLoader( AssetManager* am );

        kege::GraphicsDevice* _graphics;
    };

}
#endif /* shader_pipeline_loader_hpp */
