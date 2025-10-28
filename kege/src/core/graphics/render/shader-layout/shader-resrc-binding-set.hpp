//
//  shader-resrc-binding-set.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_resrc_binding_set_hpp
#define shader_resrc_binding_set_hpp

#include "shader-set.hpp"
#include "shader-binding-resrc.hpp"

namespace kege{

    /**
     * @brief Represents a resource binding set for shaders.
     */
    struct ShaderResrcBindingSet
    {
        const Ref< ShaderBindingResrc >& operator[]( int index )const
        {
            return resources[ index ];
        }

        Ref< ShaderBindingResrc >& operator[]( int index )
        {
            return resources[ index ];
        }

        std::vector< Ref< ShaderBindingResrc > > resources;
        ref::ShaderSet gpu_handle;
    };
}
#endif /* shader_resrc_binding_set_hpp */
