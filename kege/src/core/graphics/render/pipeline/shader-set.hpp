//
//  shader-set.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_set_hpp
#define shader_set_hpp

#include "shader-io.hpp"
#include "binding-point.hpp"

namespace kege{

    struct BufferBindInfo;
    struct ImageBindInfo;

    /**
     * @brief Abstract base class for shader binding sets.
     */
    class ShaderSet : public kege::RefCounter
    {
    public:

        virtual bool bind(int binding_index, const kege::BufferBindings& buffers) = 0;
        virtual bool bind(int binding_index, const kege::ImageBindings& images) = 0;

        virtual const vk::ShaderSet* vk() const { return nullptr; }
        virtual vk::ShaderSet* vk() { return nullptr; }

        const kege::SetLayout* getSetLayout()const;

        virtual void update() = 0;

        ShaderSet( kege::SetLayout* layout );
        virtual ~ShaderSet();

        //std::vector< kege::BindingSlot > _binding_slots;
        kege::SetLayout* _layout;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::ShaderSet > ShaderSet;
}
#endif /* shader_set_hpp */
