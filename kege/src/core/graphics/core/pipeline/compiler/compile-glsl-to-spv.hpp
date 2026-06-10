//
//  compile-glsl-to-spv.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef compile_glsl_to_spv_hpp
#define compile_glsl_to_spv_hpp

#include "../../../../utils/log.hpp"
#include "../../../core/device/core/common/graphics-common.h"

namespace kege{

    std::vector< uint32_t > compileGlslToSpv
    (
        kege::ShaderStageFlag shader_stage,
        const char* shader_name,
        const char* source,
        size_t source_size
    );

}
#endif /* compile_glsl_to_spv_hpp */
