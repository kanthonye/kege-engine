//
//  shader-binding-set.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_binding_set_hpp
#define shader_binding_set_hpp

#include "shader-struct-block.hpp"
#include "shader-binding-resrc.hpp"

namespace kege{

    /**
     * @brief Abstract base class for shader binding sets.
     */
    class ShaderSet : public kege::RefCounter
    {
    public:

        virtual void update( const std::vector< Ref< ShaderBindingResrc > >& binding_points ) = 0;
        virtual const vk::ShaderSet* vk()const = 0;
        virtual vk::ShaderSet* vk() = 0;
        virtual ~ShaderSet(){}
    };

}
#endif /* shader_binding_set_hpp */
