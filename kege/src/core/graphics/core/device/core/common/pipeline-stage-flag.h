//
//  pipeline-stage-flag.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef pipeline_stage_flag_hpp
#define pipeline_stage_flag_hpp

#include <stdlib.h>

namespace kege{

    /**
     * @brief Namespace defining flags for render graph stages.
     *
     * These flags represent different stages within the graphics pipeline
     * where memory dependencies and synchronization might be necessary.
     * They are used in barrier descriptions to specify the source and
     * destination stages of a resource transition.
     */
    enum struct PipelineStageFlag : uint32_t
    {
        None = 0,
        TopOfPipe                    = 1 << 1,
        DrawIndirect                 = 1 << 2,
        VertexInput                  = 1 << 3,
        VertexShader                 = 1 << 4,
        TessellationControlShader    = 1 << 5,
        TessellationEvaluationShader = 1 << 6,
        GeometryShader               = 1 << 7,
        FragmentShader               = 1 << 8,
        EarlyFragmentTests           = 1 << 9,
        LateFragmentTests            = 1 << 10,
        ColorOutput                  = 1 << 11,
        ComputeShader                = 1 << 12,
        Transfer                     = 1 << 13,
        Host                         = 1 << 14,
        RayTrace                     = 1 << 15,
        BottomOfPipe                 = 1 << 16,
        AllCommands = 0xFFFFFFFF,
        AllGraphics = VertexInput | VertexShader | TessellationControlShader |
                      TessellationEvaluationShader | GeometryShader | FragmentShader |
                      EarlyFragmentTests | LateFragmentTests | ColorOutput,
        AllTransfer = Transfer,
    };

    /**
     * @brief Enables bitwise OR operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise OR of the two flags.
     */
    inline PipelineStageFlag operator|(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast< PipelineStageFlag >( static_cast<uint32_t>(a) | static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise AND operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise AND of the two flags.
     */
    inline PipelineStageFlag operator&(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast< PipelineStageFlag >( static_cast<uint32_t>(a) & static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise XOR operation for PipelineStageFlag.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise XOR of the two flags.
     */
    inline PipelineStageFlag operator^(PipelineStageFlag a, PipelineStageFlag b)
    {
        return static_cast<PipelineStageFlag>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise NOT operation for PipelineStageFlag.
     * @param a The flag to negate.
     * @return The bitwise NOT of the flag.
     */
    inline PipelineStageFlag operator~(PipelineStageFlag a)
    {
        return static_cast<PipelineStageFlag>(~static_cast<uint32_t>(a));
    }

    /**
     * @brief Enables the |= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to OR with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator|=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a | b;
    }

    /**
     * @brief Enables the &= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to AND with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator&=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a & b;
    }

    /**
     * @brief Enables the ^= assignment operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The flag to XOR with.
     * @return A reference to the modified flag.
     */
    inline PipelineStageFlag& operator^=(PipelineStageFlag& a, PipelineStageFlag b)
    {
        return a = a ^ b;
    }

    /**
     * @brief Enables the == operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The integer
     * @return A true if equal, false otherwise.
     */
    inline bool operator==(PipelineStageFlag a, int b)
    {
        return static_cast<uint32_t>(a) == b;
    }

    /**
     * @brief Enables the == operator for PipelineStageFlag.
     * @param a The integer
     * @param b The flag to modify.
     * @return A true if equal, false otherwise.
     */
    inline bool operator==(int a, PipelineStageFlag b)
    {
        return static_cast<uint32_t>(b) == a;
    }

    /**
     * @brief Enables the != operator for PipelineStageFlag.
     * @param a The flag to modify.
     * @param b The integer
     * @return A true if equal, false otherwise.
     */
    inline bool operator!=(PipelineStageFlag a, int b)
    {
        return static_cast<uint32_t>(a) != b;
    }

    /**
     * @brief Enables the != operator for PipelineStageFlag.
     * @param a The integer
     * @param b The flag to modify.
     * @return A true if equal, false otherwise.
     */
    inline bool operator!=(int a, PipelineStageFlag b)
    {
        return static_cast<uint32_t>(b) != a;
    }

}
#endif /* pipeline_stage_flag_hpp */
