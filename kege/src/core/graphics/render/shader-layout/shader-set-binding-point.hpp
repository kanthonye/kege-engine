//
//  shader-set-binding-point.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_set_binding_point_hpp
#define shader_set_binding_point_hpp

#include "shader-struct-block.hpp"

namespace kege {

    enum BindingType
    {
        BUFFER, TEXTURE, PUSH_CONSTANTS, SHADER_RESOURCE
    };

    /**
     * @brief Represents a set binding point in a shader.
     */
    class ShaderSetBindingPoint : public kege::ShaderBindingBlock
    {
    public:

        ShaderSetBindingPoint(): ShaderBindingBlock( ShaderBindingType::SET ) {}

        kege::DescriptorType descriptor;
        kege::BindingType binding_type;
        uint32_t binding = 0;
        uint32_t count = 0;
        uint32_t set = 0;
    };

    typedef std::vector< Ref< ShaderSetBindingPoint > > ShaderSetBindingPoints;
}
#endif /* shader_set_binding_point_hpp */
