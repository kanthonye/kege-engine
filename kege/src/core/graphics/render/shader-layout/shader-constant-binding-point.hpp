//
//  shader-constant-binding-point.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_constant_binding_point_hpp
#define shader_constant_binding_point_hpp

#include "shader-struct-block.hpp"

namespace kege {

    /**
     * @brief Represents a push constant binding point in a shader.
     */
    class ShaderConstantBindingPoint : public kege::ShaderBindingBlock
    {
    public:

        ShaderConstantBindingPoint(): ShaderBindingBlock( ShaderBindingType::PUSH_CONSTANT ) {}

        /**
         * @brief Byte offset of push constant range.
         */
        uint32_t offset = 0;

        /**
         * @brief Size in bytes of push constant range
         * Must be: Multiple of 4. Within device limits (typically 128-256 bytes)
         */
        uint32_t size = 0;
    };
}
#endif /* shader_constant_binding_point_hpp */
