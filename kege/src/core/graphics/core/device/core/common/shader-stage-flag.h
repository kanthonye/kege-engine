//
//  shader-stage-flag.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef shader_stage_flag_h
#define shader_stage_flag_h

#include <stdlib.h>

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

}
#endif /* shader_stage_flag_h */
