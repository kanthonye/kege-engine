//
//  binding-point.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#ifndef binding_point_hpp
#define binding_point_hpp

#include "shader-io.hpp"
#include "../core/image.hpp"
#include "../core/sampler.hpp"
#include "../core/buffer.hpp"

namespace kege{

    /**
     * @brief Information for updating buffer-type descriptors
     */
    struct BufferBindInfo
    {
        /** @brief Handle to the buffer resource */
        kege::ref::Buffer buffer {};

        /** @brief Byte offset into the buffer */
        uint64_t offset = 0;

        /** @brief Size of the region to bind, or VK_WHOLE_SIZE */
        uint64_t range = 0;
    };
    typedef std::vector< kege::BufferBindInfo > BufferBindings;

    /**
     * @brief Information for updating image-type descriptors
     */
    struct ImageBindInfo
    {
        /** @brief Handle to the texture resource */
        ref::Image image {};

        /** @brief Optional sampler for combined image samplers */
        ref::Sampler sampler {};

        /**
         * @brief Layout the texture will be in during shader access
         *
         * Commonly needed for explicit APIs like Vulkan.
         */
         ImageLayout layout = ImageLayout::ShaderRead;
    };
    typedef std::vector< kege::ImageBindInfo > ImageBindings;

    /**
     * @brief Represents a single resource binding within a descriptor set.
     *
     * Can hold one of several types of resources: buffer, image, or buffer view.
     * The type is indicated by the `type` member, and only the corresponding
     * union member should be accessed.
     */
    struct BindingSlot
    {
       /*
        * @brief The type of resource bound to this binding.
        */
       enum Type { INVALID, BUFFER, IMAGE };

       uint32_t count()const
       {
           switch ( type )
           {
               case BUFFER: return static_cast< uint32_t >( buffers.size() );
               case IMAGE: return static_cast< uint32_t >( images.size() );
               case INVALID: break;
           }
           return 0;
       }

        BindingSlot& operator =( const BindingSlot& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: buffers = info.buffers; break;
                case IMAGE: images = info.images; break;
                case INVALID: break;
            }
            return *this;
        }

        BindingSlot& operator =( BindingSlot&& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: info.buffers.swap( buffers ); break;
                case IMAGE:  info.images.swap( images ); break;
                case INVALID: break;
            }
            return *this;
        }

        BindingSlot( const BufferBindings& binding )
        :   buffers( binding )
        ,   type( BUFFER )
        {}

        BindingSlot( const ImageBindings& binding )
        :   images( binding )
        ,   type( IMAGE )
        {}

        BindingSlot( const BindingSlot& info )
        :   type( info.type )
        {
            this->operator =( info );
        }

        BindingSlot( BindingSlot&& info )
        :   type( info.type )
        {
            info.buffers.swap( buffers );
            this->operator =( info );
            info.type = INVALID;
        }

        BindingSlot()
       :   type( INVALID )
       {
       }

       ~BindingSlot()
       {
           switch (type)
           {
               case BUFFER: buffers.~vector(); break;
               case IMAGE: images.~vector(); break;
               case INVALID: break;
           }
       }

       union
       {
           kege::BufferBindings buffers;
           kege::ImageBindings images;
       };

       /*
        * @brief The type of resource bound to this binding.
        */
       Type type;
    };
}
#endif /* binding_point_hpp */
