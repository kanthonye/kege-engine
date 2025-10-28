//
//  shader-reflection.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_reflection_hpp
#define shader_reflection_hpp

#include <string>
#include <vector>
#include <unordered_map>

#include "shader-data-types.hpp"
#include "shader-set-binding-point.hpp"
#include "shader-constant-binding-point.hpp"

namespace kege::glsl{

    struct ShaderReflect
    {
        std::unordered_map< int, std::vector< Ref< ShaderSetBindingPoint > > > shader_set_binding_points;
        std::vector< Ref< ShaderConstantBindingPoint > > push_constant_blocks;
        std::vector< VertexAttributeDesc > attributes;
    };

}

#endif /* shader_reflection_hpp */
