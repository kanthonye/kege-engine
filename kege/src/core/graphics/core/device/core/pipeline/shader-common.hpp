//
//  shader-binding-set-layout-desc.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#ifndef shader_binding_set_layout_desc_h
#define shader_binding_set_layout_desc_h

#include <string>
#include <vector>
#include <map>

#include "../image/image.hpp"
#include "../image/sampler.hpp"
#include "../buffer/buffer.hpp"

#include "shader.hpp"

namespace kege{

    enum class GlobalSemantic
    {
        None,
        Camera,
        Shadow,
        SceneDepth,
        SceneColor,
        GBufferAlbedo,
        GBufferNormal,
        GBufferPosition,
        GBufferMaterial,
        LightOutput,
        BRDFLut,
        BloomInput,
        BloomOutput,
        EnvironmentCubemap,
        IrradianceCubemap,
        PostprocessHDR,
        PostprocessLDR,
        BRDF_LUT,
        MotionVectors,
        DofNearField,
        DofFarField,
        ViewUniforms,
        LightGrid,
    };
    const char* toString( const kege::GlobalSemantic& semantic );
    kege::GlobalSemantic stringToSemantic( const std::string& name );


    enum struct BindType
    {
        Invalid, Buffer, Image, Sampler,
    };

    using SetIndex = uint32_t;
    using ArrayIndex = uint32_t;
    using BindingIndex = uint32_t;
    using BindingCount = uint32_t;
    using StartingIndex = uint32_t;
    enum class VarType{vec2, vec3, vec4};

    using UniformKey = uint32_t;
    using GlobalUniformKeyTable = std::unordered_map<uint64_t, kege::UniformKey>;

    /**
     * @enum kege::ResrcUsage
     * @brief Describes how a resource bound to a shader is used.
     *
     * Each value specifies the usage pattern of a resource within a descriptor set.
     * This determines how the shader and pipeline interact with the resource.
     *
     * Although similar to Vulkan's descriptor type system, this enum focuses on
     * describing *how* a resource is used rather than *what type* of resource it is.
     *
     * @note For example, a buffer resource may be used as a UniformBuffer or
     * a StorageBuffer depending on its binding usage.
     */
    enum struct BindingUsage
    {
        Invalid,               ///< Invalid or uninitialized usage.
        Sampler,               ///< Standalone sampler object.
        CombinedImageSampler,  ///< Combined image and sampler (typical for textures).
        SampledImage,          ///< Separate read-only sampled image.
        StorageImage,          ///< Writable image accessible in shaders.
        UniformTexelBuffer,    ///< Formatted buffer used as a read-only texel buffer.
        StorageTexelBuffer,    ///< Writable formatted buffer.
        UniformBuffer,         ///< Standard uniform buffer (UBO).
        StorageBuffer,         ///< Writable storage buffer (SSBO).
        UniformBufferDynamic,  ///< UBO with dynamic offset.
        StorageBufferDynamic,  ///< SSBO with dynamic offset.
        InputAttachment        ///< Framebuffer input for subpasses.
    };
    kege::BindingUsage stringToBindingUsage( const std::string& name );

    enum class ArrayType
    {
        None,           // Not an array (exactly 1 descriptor)
        Static,         // Fixed size known at compile time (count matters!)
        Runtime         // Unsized array (count is maximum, but ignored in hash)
    };

    struct StructBlockMemberDesc
    {
        std::string name;
        std::string type;
        uint32_t offset;
        uint32_t size;
        uint32_t count;
    };

    struct StructBlockDesc : public kege::RefCounter
    {
        StructBlockDesc(std::string n, const std::vector< StructBlockMemberDesc >& m)
        : members( m ), name( n ) {}

        StructBlockDesc(){}
        
        std::vector< StructBlockMemberDesc > members;
        std::string name;
    };

    namespace ref{
        using StructBlockDesc = kege::Ref< kege::StructBlockDesc >;
    }
}

namespace kege{

    struct ResourceBindingDesc
    {
        std::string name;

        kege::BindType type;
        kege::BindingUsage usage;
        kege::BindingIndex index = 0;
        kege::BindingCount count = 0;

        kege::ShaderStageFlag stages;
        kege::ArrayType array_type = ArrayType::None;
        kege::GlobalSemantic semantic = kege::GlobalSemantic::None;
    };

    /**
     * @struct ShaderSetBindingDesc
     * @brief Describes a single shader binding entry within a ShaderSetDesc.
     *
     * Each ShaderSetBindingDesc object represents one binding point in a
     * ShaderSetDesc. It defines how the resource is used, which
     * shader stages access it, and optionally includes reflected structure
     * metadata for structured bindings.
     *
     * @see ShaderSetBindingDesc
     */
    struct ShaderSetBindingDesc
    {
        std::string name;

        BindType type;
        BindingUsage usage;
        BindingIndex binding_index;
        BindingCount binding_count;
        kege::ShaderStageFlag stages;

        ArrayType array_type = ArrayType::None;

        kege::Ref< kege::StructBlockDesc > struct_desc;
    };

    /**
     * @brief Describes the bindings that make up a single resource set configuration.
     *
     * A ShaderSetDesc defines the layout of a shader uniform set, including
     * all its bindings. It can be shared across multiple pipeline layouts, allowing
     * reusability of common descriptor configurations between different shaders or
     * render passes.
     *
     * @see ShaderSetBindingDesc
     * @see ShaderPipelineSetDesc
     */
    using ShaderSetDesc = std::vector< kege::ShaderSetBindingDesc >;

    /**
     * @typedef ShaderSetDesc
     * @brief Describes the resource set layout configuration for a shader layout.
     *
     * A ShaderSetDesc defines the complete resource set layout for a shader or pipeline.
     * It consists of a list of indexed set configurations that specify how each set
     * is arranged and what bindings it contains.
     *
     * @see ShaderResourceBindingDesc
     */
    using ShaderPipelineSetDesc = std::vector< kege::ShaderSetDesc >;

}


namespace kege{

    /**
     * @brief Describes a single push constant range in a shader.
     *
     * Push constants are small uniform data blocks passed directly to shaders.
     * They are not bound like buffers or images, and their lifetime is limited to
     * the command buffer they are recorded in.
     */
    struct PushBlockDesc
    {
        uint32_t offset;         ///< Byte offset within the push constant range.
        uint32_t size;           ///< Size in bytes of the push constant range.
        uint32_t count;          ///< Count for total array elements
        std::string id;
        std::string name;           ///< Name of the push constant block in the shader.
        kege::ShaderStageFlag stages; ///< Shader stages that can access this push constant.
        kege::Ref< kege::StructBlockDesc > struct_desc; ///< Structured layout of the constant data.
    };

    /**
     * @brief Describes all push constant ranges used in a single shader layout.
     *
     * A shader layout can define one or more push constant blocks, each with their
     * own offset, size, and visibility. Push constant ranges must not overlap.
     *
     * @see kege::PushBlockInfo
     */
    typedef std::vector< PushBlockDesc > PushBlockDescs;

}


namespace kege{

    struct ShaderPipelineLayoutDesc
    {
        kege::ShaderPipelineSetDesc resource_layout_desc;
        kege::PushBlockDescs push_block_desc;
        std::string name;
    };

}



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
    struct Binding 
    {
        // Initializer list constructors
        Binding (std::initializer_list<kege::BufferBindInfo> list)
            : type(BindType::Buffer)
            , _count(static_cast<int>(list.size()))
        {
            buffers = new kege::BufferBindInfo[ _count ];
            std::copy(list.begin(), list.end(), buffers);
        }

        Binding (std::initializer_list<kege::ImageBindInfo> list)
            : type(BindType::Image)
            , _count(static_cast<int>(list.size()))
        {
            images = new kege::ImageBindInfo[ _count ];
            std::copy(list.begin(), list.end(), images);
        }

        // Array + size constructor (for when you already have data)
        Binding (const kege::BufferBindInfo* data, int size)
            : type(BindType::Buffer)
            , _count(size)
        {
            buffers = new kege::BufferBindInfo[ _count ];
            std::copy(data, data + _count, buffers);
        }

        Binding (const kege::ImageBindInfo* data, int size)
            : type(BindType::Image)
            , _count(size)
        {
            images = new kege::ImageBindInfo[ _count ];
            std::copy(data, data + _count, images);
        }

        // Vector constructors (convenience)
        Binding (const std::vector<kege::BufferBindInfo>& vec)
            : Binding (vec.data(), static_cast<int>(vec.size()))
        {
            type = BindType::Buffer;
        }

        Binding (const std::vector<kege::ImageBindInfo>& vec)
            : Binding (vec.data(), static_cast<int>(vec.size()))
        {
            type = BindType::Image;
        }

        Binding (BindType type, int count)
        :   type( type )
        ,   _count( count )
        {
            if (type == BindType::Buffer)
            {
                buffers = new kege::BufferBindInfo[ _count ];
            }
            else if (type == BindType::Image || type == BindType::Sampler)
            {
                images = new kege::ImageBindInfo[ _count ];
            }
        }

        // Default constructor
        Binding ()
            : buffers(nullptr)
            , type(BindType::Invalid)
            , _count(0)
        {}

        // Destructor
        ~Binding ()
        {
            cleanup();
        }

        // Move constructor
        Binding (Binding && other) noexcept
            : type(other.type)
            , _count(other._count)
        {
            if (type == BindType::Buffer) {
                buffers = other.buffers;
                other.buffers = nullptr;
            }
            else if (type == BindType::Image || type == BindType::Sampler) {
                images = other.images;
                other.images = nullptr;
            }
            other._count = 0;
            other.type = BindType::Invalid;
        }

        // Copy constructor
        Binding (const Binding & other)
            : type(other.type)
            , _count(other._count)
        {
            if (_count == 0) {
                buffers = nullptr;
                return;
            }

            if (type == BindType::Buffer) {
                buffers = new kege::BufferBindInfo[ _count ];
                std::copy(other.buffers, other.buffers + _count, buffers);
            }
            else if (type == BindType::Image || type == BindType::Sampler) {
                images = new kege::ImageBindInfo[ _count ];
                std::copy(other.images, other.images + _count, images);
            }
        }

        // Move assignment
        Binding & operator=(Binding && other) noexcept
        {
            if (this != &other) {
                cleanup();

                type = other.type;
                _count = other._count;

                if (type == BindType::Buffer) {
                    buffers = other.buffers;
                    other.buffers = nullptr;
                }
                else if (type == BindType::Image || type == BindType::Sampler) {
                    images = other.images;
                    other.images = nullptr;
                }

                other._count = 0;
                other.type = BindType::Invalid;
            }
            return *this;
        }

        // Copy assignment
        Binding & operator=(const Binding & other)
        {
            if (this != &other)
            {
                cleanup();

                type = other.type;
                _count = other._count;

                if (_count == 0)
                {
                    buffers = nullptr;
                    return *this;
                }

                if (type == BindType::Buffer)
                {
                    buffers = new kege::BufferBindInfo[ _count ];
                    std::copy(other.buffers, other.buffers + _count, buffers);
                }
                else if (type == BindType::Image || type == BindType::Sampler)
                {
                    images = new kege::ImageBindInfo[ _count ];
                    std::copy(other.images, other.images + _count, images);
                }
            }
            return *this;
        }

        // Accessors
        uint32_t count() const { return static_cast<uint32_t>( _count ); }

        // Array access (with bounds checking in debug)
        kege::BufferBindInfo& bufferAt(int index)
        {
            assert(type == BindType::Buffer && index >= 0 && index < _count);
            return buffers[index];
        }

        const kege::BufferBindInfo& bufferAt(int index) const
        {
            assert(type == BindType::Buffer && index >= 0 && index < _count);
            return buffers[index];
        }

        kege::ImageBindInfo& imageAt(int index)
        {
            assert(type == BindType::Image && index >= 0 && index < _count);
            return images[index];
        }

        const kege::ImageBindInfo& imageAt(int index) const
        {
            assert(type == BindType::Image && index >= 0 && index < _count);
            return images[index];
        }

        kege::ImageBindInfo& samplerAt(int index)
        {
            assert(type == BindType::Sampler && index >= 0 && index < _count);
            return images[index];
        }

        const kege::ImageBindInfo& samplerAt(int index) const
        {
            assert(type == BindType::Sampler && index >= 0 && index < _count);
            return images[index];
        }

        friend bool operator ==(const Binding& lh, const Binding& rh)
        {
            if( lh.count() != rh.count() || lh.buffers == nullptr || rh.buffers == nullptr)
                return false;
            if (lh.type == BindType::Buffer && rh.type == BindType::Buffer)
            {
                for (int i=0; i<lh.count(); ++i)
                {
                    if(lh.buffers[i].buffer != rh.buffers[i].buffer)
                        return false;
                }
                return true;
            }
            else if (lh.type == BindType::Image && rh.type == BindType::Image)
            {
                for (int i=0; i<lh.count(); ++i)
                {
                    if(lh.images[i].image != rh.images[i].image)
                        return false;
                }
                return true;
            }
            else if (lh.type == BindType::Sampler && rh.type == BindType::Sampler)
            {
                for (int i=0; i<lh.count(); ++i)
                {
                    if(lh.images[i].sampler != rh.images[i].sampler)
                        return false;
                }
                return true;
            }
            return false;
        }

        friend bool operator !=(const Binding& lh, const Binding& rh)
        {
            return !(operator ==(lh, rh));
        }
        
    private:

        void cleanup()
        {
            if (_count == 0) return;

            if (type == BindType::Buffer)
            {
                delete[] buffers;
            }
            else if (type == BindType::Image || type == BindType::Sampler)
            {
                delete[] images;
            }

            type = BindType::Invalid;
            _count = 0;
        }

    public:

        union
        {
            kege::BufferBindInfo* buffers;
            kege::ImageBindInfo* images;
        };
        BindType type;
        int _count;
    };

}



namespace kege{

    struct Hasher
    {
        static void combine(std::size_t& h, std::size_t v)
        {
            h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        };

        static bool equal(const kege::ShaderSetBindingDesc& a, const kege::ShaderSetBindingDesc& b)
        {
            return a.binding_count == b.binding_count &&
                   a.type == b.type &&
                   a.usage == b.usage &&
                   a.binding_index == b.binding_index &&
                   a.binding_count == b.binding_count &&
                   a.stages == b.stages &&
                   a.struct_desc == b.struct_desc;
        }

        static size_t hash(const kege::ShaderSetBindingDesc& desc )
        {
            size_t seed = 0;

            combine( seed, static_cast< uint32_t >( desc.usage ) );
            combine( seed, static_cast< uint32_t >( desc.type ) );
            combine( seed, static_cast< uint32_t >( desc.binding_index ) );
            combine( seed, static_cast< uint32_t >( desc.array_type ) );
            combine( seed, static_cast< uint32_t >( desc.stages ) );
            //combine( seed, static_cast< uint32_t >( desc.type ) );
            //combine( seed, static_cast< uint32_t >( desc.starting_index ) );

            // Only hash count for static arrays
            if (desc.array_type == ArrayType::Static)
            {
                combine( seed, desc.binding_count);
            }
            return seed;
        }

        static size_t hash(const kege::PushBlockDesc& desc )
        {
            size_t seed = 0;

            combine( seed, static_cast< uint32_t >( desc.offset ) );
            combine( seed, static_cast< uint32_t >( desc.size ) );
            combine( seed, static_cast< uint32_t >( desc.count ) );
            combine( seed, (uintptr_t)desc.struct_desc.ref() );

            return seed;
        }

        static size_t hash
        (
            uint32_t size,
            const kege::ShaderSetBindingDesc* descs
        )
        {
            size_t seed = 0;
            for (int i = 0; i < size; ++i)
            {
                seed = seed ^ hash( descs[i] );
            }
            return seed;
        }

        static size_t hash(const kege::ShaderPipelineSetDesc& desc )
        {
            size_t seed = 0xcbf29ce484222325ULL; // FNV-1a offset basis
            for (const auto& d : desc)
            {
                seed = seed ^ hash( (uint32_t)d.size(), d.data() );
                seed = seed * 0x100000001b3ULL;
            }
            return seed;
        }

        static size_t hash(const kege::ShaderPipelineLayoutDesc& desc )
        {
            size_t seed = 0xcbf29ce484222325ULL; // FNV-1a offset basis
            for (const auto& d : desc.resource_layout_desc)
            {
                seed = seed ^ hash( (uint32_t)d.size(), d.data() );
                seed = seed * 0x100000001b3ULL;
            }
            for (const auto& d : desc.push_block_desc)
            {
                seed = seed ^ hash(d);
                seed = seed * 0x100000001b3ULL;
            }
            return seed;
        }
    };

}
#endif /* shader_binding_set_layout_desc_h */
