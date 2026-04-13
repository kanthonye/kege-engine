//
//  shader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#ifndef shader_hpp
#define shader_hpp

#include <string>
#include <vector>
#include "../core/graphics-core.hpp"

namespace kege{

    /**
     * @brief Bitmask flags representing shader stages in the rendering pipeline.
     *
     * Used to specify which stages a shader module targets or which pipeline stages
     * should be active. Values can be combined using bitwise OR.
     */
    enum class ShaderStageFlag : uint32_t
    {
        Invalid = 0,
        Vertex = 1 << 0,                  ///< Vertex shader stage
        Fragment = 1 << 1,                ///< Fragment (Pixel) shader stage
        Compute = 1 << 2,                 ///< Compute shader stage
        Geometry = 1 << 3,                ///< Geometry shader stage
        TessellationControl = 1 << 4,     ///< Tessellation Control (Hull) shader stage
        TessellationEvaluation = 1 << 5,  ///< Tessellation Evaluation (Domain) shader stage

        // Predefined combinations
        AllGraphics = Vertex | Fragment | Geometry | TessellationControl | TessellationEvaluation,
        //AllGraphics = (1 << 6) - 1,       ///< All graphics pipeline stages (excludes Compute)
        All = AllGraphics | Compute       ///< All possible shader stages
    };

    inline constexpr ShaderStageFlag operator|(ShaderStageFlag a, ShaderStageFlag b)
    {
        return static_cast<ShaderStageFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr ShaderStageFlag operator&(ShaderStageFlag a, ShaderStageFlag b)
    {
        return static_cast<ShaderStageFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr ShaderStageFlag& operator|=(ShaderStageFlag& a, ShaderStageFlag b)
    {
        a = a | b;
        return a;
    }

    inline constexpr ShaderStageFlag& operator&=(ShaderStageFlag& a, ShaderStageFlag b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool isset(const ShaderStageFlag& flags, const ShaderStageFlag& flag)
    {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
    }

    inline constexpr ShaderStageFlag unset(const ShaderStageFlag& flags, const ShaderStageFlag& flag) noexcept
    {
        return static_cast<ShaderStageFlag>(static_cast<uint32_t>(flags) & ~static_cast<uint32_t>(flag));
    }





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
