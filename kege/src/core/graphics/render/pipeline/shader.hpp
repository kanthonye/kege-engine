//
//  shader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#ifndef shader_hpp
#define shader_hpp

#include "../core/declarations.hpp"

namespace kege{

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
