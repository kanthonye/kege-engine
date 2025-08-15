//
//  pipeline-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef pipeline_loader_hpp
#define pipeline_loader_hpp

#include "../graphics/core/graphics-device.hpp"
#include "../graphics/core/string-to-enum-types.hpp"
#include "../utils/json-parser.hpp"

namespace kege{

    bool loadTextFile( std::vector< char >& source, const char* filename );
    std::string getFilePath( const std::string& filename );

    std::vector< uint32_t > compileGlslToSpv
    (
        const char* shader_name,
        ShaderStage shader_stage,
        const std::vector< char >& source
    );
    
    kege::PipelineHandle createPipelineFromFile
    (
        kege::GraphicsDevice* device,
        const std::string& filename
    );

    kege::PipelineHandle createPipelineFromFile
    (
        kege::GraphicsDevice* device,
        kege::Json json,
        const std::string& path
    );

}
#endif /* pipeline_loader_hpp */
