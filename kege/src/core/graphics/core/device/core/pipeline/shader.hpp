//
//  shader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#ifndef shader_hpp
#define shader_hpp

#include "shader-common.hpp"

namespace kege
{
    class Shader;
}

namespace kege::vk
{
    class Shader;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Shader > Shader;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Shader > Shader;
}

namespace kege{

    /**
     * @brief Describes a shader module for pipeline creation.
     *
     * Contains the shader code and metadata needed to create a shader module.
     * Supports both precompiled bytecode and potentially runtime compilation.
     */
    struct ShaderDesc
    {
        /**
         * @brief Shader bytecode in the target API's format.
         *
         * Supported formats:
         * - Vulkan: SPIR-V bytecode
         * - Direct3D 12: DXIL/DXBC
         * - Metal: AIR (Apple Intermediate Representation) or source
         *
         * @note For better cross-API compatibility, prefer SPIR-V where possible.
         */
        std::vector<uint32_t> byte_code;

        /**
         * @brief Entry point function name in the shader.
         *
         * Defaults to "main" which is conventional for most shading languages.
         * For HLSL, this would match the function name marked as [shader("type")].
         */
        std::string entry_point = "main";

        /**
         * @brief Which pipeline stage this shader targets.
         *
         * Must match the actual shader type (e.g., a fragment shader can't be
         * specified as ShaderStageFlag::Vertex).
         */
        ShaderStageFlag stage = ShaderStageFlag::Vertex;

        /**
         * @brief Debug label for the shader module.
         *
         * Visible in graphics debuggers like RenderDoc or NVIDIA Nsight.
         * Highly recommended for better debugging experience.
         */
        std::string name = "";
    };

    /**
     * @brief Validates that the shader description is complete.
     * @return true if valid, false if missing critical data
     */
    inline bool isValid( const ShaderDesc& desc )
    {
        return !desc.byte_code.empty() && !desc.entry_point.empty() && desc.stage != ShaderStageFlag(0);
    }

    
    /**
     * @brief Abstract base class for shader binding sets.
     */
    class Shader : public kege::RefCounter
    {
    public:

        virtual const vk::Shader* vk()const = 0;
        virtual vk::Shader* vk() = 0;
        virtual ~Shader(){}
    };

}
#endif /* shader_hpp */
