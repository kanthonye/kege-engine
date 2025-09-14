//
//  uniform-resource-info.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/11/25.
//

#ifndef uniform_resource_info_hpp
#define uniform_resource_info_hpp

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Information for updating buffer-type descriptors
     */
    struct BufferInfo
    {
        /** @brief Handle to the buffer resource */
        BufferHandle buffer {};

        /** @brief Byte offset into the buffer */
        uint64_t offset = 0;

        /** @brief Size of the region to bind, or VK_WHOLE_SIZE */
        uint64_t range = 0;
    };

    /**
     * @brief Information for updating image-type descriptors
     */
    struct ImageInfo
    {
        /** @brief Handle to the texture resource */
        ImageHandle image {};

        /** @brief Optional sampler for combined image samplers */
        SamplerHandle sampler {};

        /**
         * @brief Layout the texture will be in during shader access
         *
         * Commonly needed for explicit APIs like Vulkan.
         */
         ImageLayout layout = ImageLayout::ShaderReadOnly;
    };

}


namespace kege{

    typedef std::vector< kege::BufferViewHandle > BufferViewBindings;
    typedef std::vector< kege::BufferInfo > BufferBindings;
    typedef std::vector< kege::ImageInfo > ImageBindings;

    /**
     * @brief Represents a single resource binding within a descriptor set.
     *
     * Can hold one of several types of resources: buffer, image, or buffer view.
     * The type is indicated by the `type` member, and only the corresponding
     * union member should be accessed.
     */
    struct Uniform
    {
       /*
        * @brief The type of resource bound to this binding.
        */
       enum Type { INVALID, BUFFER, IMAGE, BUFFER_VIEW };

       inline friend bool operator!=( const Uniform& info, kege::ImageHandle handle );
       inline friend bool operator!=( kege::ImageHandle handle, const Uniform& info );
       inline friend bool operator==( const Uniform& info, kege::ImageHandle handle );
       inline friend bool operator==( kege::ImageHandle handle, const Uniform& info );
       inline friend bool operator!=( const Uniform& info, kege::BufferHandle handle );
       inline friend bool operator!=( kege::BufferHandle handle, const Uniform& info );
       inline friend bool operator==( const Uniform& info, kege::BufferHandle handle );
       inline friend bool operator==( kege::BufferHandle handle, const Uniform& info );

       uint32_t count()const
       {
           switch ( type )
           {
               case BUFFER: return static_cast< uint32_t >( buffers.size() );
               case IMAGE: return static_cast< uint32_t >( images.size() );
               case BUFFER_VIEW: return static_cast< uint32_t >( buffer_views.size() );
               case INVALID: break;
           }
           return 0;
       }

        Uniform& operator =( const Uniform& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: buffers = info.buffers; break;
                case IMAGE: images = info.images; break;
                case BUFFER_VIEW: buffer_views = info.buffer_views; break;
                case INVALID: break;
            }
            return *this;
        }

        Uniform& operator =( Uniform&& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: info.buffers.swap( buffers ); break;
                case IMAGE:  info.images.swap( images ); break;
                case BUFFER_VIEW: info.buffer_views.swap( buffer_views ); break;
                case INVALID: break;
            }
            return *this;
        }

       Uniform( const BufferViewBindings& binding )
       :   buffer_views( binding )
       ,   type( BUFFER_VIEW )
       {}

       Uniform( const BufferBindings& binding )
       :   buffers( binding )
       ,   type( BUFFER )
       {}

       Uniform( const ImageBindings& binding )
       :   images( binding )
       ,   type( IMAGE )
       {}

        Uniform( const Uniform& info )
        :   type( info.type )
        {
            this->operator =( info );
        }

        Uniform( Uniform&& info )
        :   type( info.type )
        {
            info.buffers.swap( buffers );
            this->operator =( info );
            info.type = INVALID;
        }

       Uniform()
       :   type( INVALID )
       {
       }

       ~Uniform()
       {
           switch (type)
           {
               case BUFFER: buffers.~vector(); break;
               case IMAGE: images.~vector(); break;
               case BUFFER_VIEW: buffer_views.~vector(); break;
               case INVALID: break;
           }
       }

       union
       {
           std::vector< kege::BufferInfo > buffers;
           std::vector< kege::ImageInfo > images;
           std::vector< kege::BufferViewHandle > buffer_views;
       };

       /*
        * @brief The type of resource bound to this binding.
        */
       Type type;
    };

    struct UniformBinding
    {
        int binding;
        Uniform uniform;
    };

    struct UniformSet
    {
        std::vector< UniformBinding >::const_iterator begin()const;
        std::vector< UniformBinding >::const_iterator end()const;
        
        inline const UniformBinding& operator[](int i) const { return elements[i]; }
        inline UniformBinding& operator[](int i){ return elements[i]; }

        uint32_t size()const{ return static_cast< uint32_t >( elements.size() ); }

        UniformSet& operator =( const UniformSet& u ){
            elements = u.elements;
            return *this;
        }
        UniformSet& operator =( UniformSet&& u ) noexcept {
            u.elements.swap( elements );
            return *this;
        }

        explicit UniformSet( const std::vector< UniformBinding >& v ): elements(v) {}
        UniformSet( const UniformSet& u ): elements( u.elements ){}
        UniformSet( UniformSet&& u ) { u.elements.swap( elements ); }
        UniformSet(){}

        std::vector< UniformBinding > elements;
    };

    struct UniformSets
    {
        std::vector< UniformSet >::const_iterator begin()const;
        std::vector< UniformSet >::const_iterator end()const;

        inline const UniformSet& operator[](int i) const { return sets[i]; }
        inline UniformSet& operator[](int i){ return sets[i]; }

        uint32_t size()const{ return static_cast< uint32_t >( sets.size() ); }

        UniformSets& operator =( const UniformSets& u ){
            sets = u.sets;
            return *this;
        }
        UniformSets& operator =( UniformSets&& u ) noexcept {
            u.sets.swap( sets );
            return *this;
        }

        explicit UniformSets( const std::vector< UniformSet >& v ): sets(v) {}
        UniformSets( const UniformSets& u ): sets( u.sets ){}
        UniformSets( UniformSets&& u ) { u.sets.swap( sets ); }
        UniformSets(){}

        std::vector< UniformSet > sets;
    };

}


namespace kege{

    struct UniformDesc
    {
       /**
        * @brief The descriptor set layout binding name in the shader.
        */
       std::string name = "";

       /**
        * @brief Binding number matching shader declaration.
        * Example: `layout(binding = 0) uniform UniformBufferObject`
        */
       uint32_t binding = 0;

       /**
        * @brief Number of array elements (for array bindings).
        * Use 1 for non-array resources.
        */
       uint32_t count = 1;

       /**
        * @brief Type of resource being bound.
        */
        kege::DescriptorType descriptor_type = kege::DescriptorType::UniformBuffer;

       /**
        * @brief Shader stages that can access this binding.
        */
        kege::ShaderStage stage_flags = kege::ShaderStage::All;

       /**
        * @brief Optional immutable samplers (for sampler/image bindings).
        * When non-empty, overrides any sampler provided in descriptor set.
        */
       std::vector< kege::SamplerHandle > immutable_samplers;
    };


    typedef std::vector< UniformDesc > UniformSetDesc;
    typedef std::vector< UniformSetDesc > UniformSetsDesc;

//    /**
//     * @brief A single descriptor set, containing its layout descriptions and bound resources.
//     */
//    struct CreateUniformSet
//    {
//        UniformDescs descriptions;
//        UniformBindings bindings;
//        std::string name;
//    };
//
//    /**
//     * @brief A collection of multiple descriptor sets, each with their own bindings and resources.
//     */
//    struct CreateUniformMultiSet
//    {
//        UniformMultiSetDesc descriptions;
//        UniformLayouts resources;
//        std::string name;
//    };
//
//    struct UniformHandle{ int id = 0; };
//    //struct UniformSetLayoutHandle{ int id = 0; };
//
//
//
//   struct UpdateUniformParams
//   {
//       UniformHandle* handle;
//       UniformLayouts resources;
//   };

    
}

namespace kege{

    inline bool operator!=( const kege::Uniform& info, kege::ImageHandle handle )
    {
        return ( info.type == kege::Uniform::IMAGE )
        ? info.images[0].image.id != handle.id
        : false;
    }
    inline bool operator!=( kege::ImageHandle handle, const kege::Uniform& info )
    {
        return ( info.type == kege::Uniform::IMAGE )
        ? info.images[0].image.id != handle.id
        : false;
    }
    inline bool operator==( const kege::Uniform& info, kege::ImageHandle handle )
    {
        return ( info.type == kege::Uniform::IMAGE )
        ? info.images[0].image.id != handle.id
        : false;
    }
    inline bool operator==( kege::ImageHandle handle, const kege::Uniform& info )
    {
        return ( info.type == kege::Uniform::IMAGE )
        ? info.images[0].image.id != handle.id
        : false;
    }
    inline bool operator!=( const kege::Uniform& info, kege::BufferHandle handle )
    {
        return ( info.type == kege::Uniform::BUFFER )
        ? info.buffers[0].buffer.id != handle.id
        : false;
    }
    inline bool operator!=( kege::BufferHandle handle, const kege::Uniform& info )
    {
        return ( info.type == kege::Uniform::BUFFER )
        ? info.buffers[0].buffer.id != handle.id
        : false;
    }
    inline bool operator==( const kege::Uniform& info, kege::BufferHandle handle )
    {
        return ( info.type == kege::Uniform::BUFFER )
        ? info.buffers[0].buffer.id != handle.id
        : false;
    }
    inline bool operator==( kege::BufferHandle handle, const kege::Uniform& info )
    {
        return ( info.type == kege::Uniform::BUFFER )
        ? info.buffers[0].buffer.id != handle.id
        : false;
    }


    std::size_t hash( const std::vector< kege::UniformDesc >& bindings );
    std::size_t hash( const std::vector< kege::DescriptorType >& types );
    std::size_t hash( const kege::UniformDesc& binding );

}




namespace std{

    template <> struct hash< std::vector< kege::DescriptorType > >
    {
        std::size_t operator()( const std::vector< kege::DescriptorType >& types ) const
        {
            return kege::hash( types );
        }
    };

    template <> struct hash< std::vector< kege::UniformDesc > >
    {
        std::size_t operator()( const std::vector< kege::UniformDesc >& bindings ) const
        {
            return kege::hash( bindings );
        }
    };

    // Define equality comparison for VkDescriptorSetLayoutBinding
    inline bool operator==(const kege::UniformDesc& a, const kege::UniformDesc& b)
    {
        // Compare relevant members.
        // IMPORTANT: Ignore pImmutableSamplers if your hash function ignores it!
        // Comparing pointer values is usually wrong for value semantics anyway.
        return a.binding         == b.binding &&
               a.descriptor_type  == b.descriptor_type &&
               a.count == b.count &&
               a.stage_flags      == b.stage_flags;
    }

    // Define none equality comparison for VkDescriptorSetLayoutBinding
    inline bool operator!=(const kege::UniformDesc& a, const kege::UniformDesc& b)
    {
        // Compare relevant members.
        // IMPORTANT: Ignore pImmutableSamplers if your hash function ignores it!
        // Comparing pointer values is usually wrong for value semantics anyway.
        return a.binding         != b.binding &&
               a.descriptor_type  != b.descriptor_type &&
               a.count != b.count &&
               a.stage_flags      != b.stage_flags;
    }

    // Define less-than comparison (needed to resolve the compiler error)
    inline bool operator<(const kege::UniformDesc& a, const kege::UniformDesc& b)
    {
        // Compare members lexicographically to establish a strict weak ordering.
        // Use std::tie to make this comparison clean and less error-prone.
        // IMPORTANT: Use the SAME members in the SAME order as operator==, and
        // ignore members ignored by the hash/equality functions (like pImmutableSamplers).
        return std::tie(a.binding, a.descriptor_type, a.count, a.stage_flags) <
               std::tie(b.binding, b.descriptor_type, b.count, b.stage_flags);

        // Manual comparison (alternative to std::tie):
        // if (lhs.binding != rhs.binding) return lhs.binding < rhs.binding;
        // if (lhs.descriptorType != rhs.descriptorType) return lhs.descriptorType < rhs.descriptorType;
        // if (lhs.descriptorCount != rhs.descriptorCount) return lhs.descriptorCount < rhs.descriptorCount;
        // if (lhs.stageFlags != rhs.stageFlags) return lhs.stageFlags < rhs.stageFlags;
        // // If all relevant members are equal, they are not less-than
        // return false;
    }
    inline bool operator==(const std::vector< kege::UniformDesc >& a, const std::vector< kege::UniformDesc >& b)
    {
        if( a.size() != b.size() ) return false;
        for(int i=0; i<a.size(); ++i)
            if( a[i] != b[i] )
                return false;
        return true;
    }
}

#endif /* uniform_resource_info_hpp */
