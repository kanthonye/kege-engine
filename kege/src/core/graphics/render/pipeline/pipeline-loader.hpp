//
//  pipeline-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef pipeline_loader_hpp
#define pipeline_loader_hpp

#include "../../../renderer/core/graphics.hpp"
#include "../../../utils/json-parser.hpp"

namespace kege{

    bool loadTextFile( std::vector< char >& source, const char* filename );
    std::string getFilePath( const std::string& filename );

    /**
     * @brief Convert a shader stage string to ShaderStage enum.
     * @param stage The shader stage as a string.
     * @return Corresponding ShaderStage enum value.
     */
    kege::ShaderStage convertShaderStage( const std::string& stage );

    /**
     * @brief Convert a JSON array of shader stages to ShaderStage enum flags.
     * @param json JSON array containing shader stage strings.
     * @return Combined ShaderStage enum flags.
     */
    kege::ShaderStage convertShaderStageFlages( kege::Json json );  

    /**
     * @brief Compile GLSL source code to SPIR-V bytecode.
     * @param shader_name Name of the shader.
     * @param shader_stage The shader stage.
     * @param source GLSL source code as a vector of characters.
     * @return Compiled SPIR-V bytecode as a vector of uint32_t.
     */
    std::vector< uint32_t > compileGlslToSpv
    (
        const char* shader_name,
        ShaderStage shader_stage,
        const std::vector< char >& source
    );
    
}

namespace kege{

    class PipelineLoader {
    public:

        static kege::PipelineHandle load
        (
            kege::Graphics* graphics,
            const std::string& filename
        );

        static bool createPipelineFromFile
        (
            kege::Graphics* graphics,
            kege::GraphicsPipelineDesc* info,
            kege::Json json,
            const std::string& path
        );

    };

    bool parseShaderPipelineInfo
    (
        kege::Graphics* graphics,
        kege::CreateShaderPipelineInfo* info,
        kege::Json json,
        const std::string& path
    );
}
#endif /* pipeline_loader_hpp */
