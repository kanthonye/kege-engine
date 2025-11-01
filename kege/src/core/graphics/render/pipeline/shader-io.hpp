//
//  shader-binding-set-layout-desc.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/29/25.
//

#ifndef shader_binding_set_layout_desc_h
#define shader_binding_set_layout_desc_h

#include "shader-struct-block.hpp"

namespace kege{

   enum struct BindingType
   {
       Invalid, Buffer, Image,
   };

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

    /**
     * @struct kege::BindingInfo
     * @brief Describes a single shader binding entry within a descriptor set layout.
     *
     * Each BindingInfo object represents one binding point in a descriptor set.
     * It defines how the resource is used, which shader stages access it, and
     * optionally includes reflected structure metadata for structured bindings.
     *
     * @see kege::ResrcUsage
     * @see kege::SetConfigInfo
     */
    struct BindingInfo
    {
        /**
         * @var count
         * @brief Number of array elements for this binding.
         *
         * Specifies how many descriptors are part of this binding.
         * Use `1` for single bindings, or higher values for arrays.
         */
        uint32_t count;

        /**
         * @var binding
         * @brief The binding index within the descriptor set.
         *
         * This corresponds to the `binding` number declared in the shader.
         */
        uint32_t binding;

        /**
         * @var name
         * @brief Optional human-readable name for the binding.
         */
        std::string name;

        /**
         * @var usage
         * @brief Describes how the bound resource is used.
         *
         * See @ref kege::ResrcUsage for all possible binding usages.
         */
        kege::BindingUsage usage;

        /**
         * @var stages
         * @brief Specifies which shader stages can access this binding.
         */
        kege::ShaderStageFlag stages;

        /**
         * @var block
         * @brief Optional reflection data for structured buffers or uniforms.
         *
         * Contains block metadata extracted from shader reflection for uniform
         * or storage buffer bindings that define a structured layout.
         */
        kege::ref::ShaderStructBlock block;

        /**
         * @var type
         * @brief Describes how the binding resource type.
         *
         * See @ref kege::BindingType for all possible binding type.
         */
        kege::BindingType type;
    };

    /**
     * @brief Describes the bindings that make up a single descriptor set configuration.
     *
     * A SetConfigInfo defines the layout of a descriptor set, including all its bindings.
     * It can be shared across multiple pipeline layouts, allowing reusability of common
     * descriptor configurations between different shaders or render passes.
     *
     * @see kege::BindingInfo
     * @see kege::IndexedSetInfo
     */
    using SetBindings = std::vector< kege::BindingInfo >;


    /**
     * @struct kege::IndexedSetConfig
     * @brief Associates a set configuration with a specific descriptor set index.
     *
     * Multiple shader pipelines may reuse the same set configuration at different indices.
     * This structure allows the same configuration to be assigned to different set
     * indices in various shader layouts.
     *
     * @see kege::SetConfigInfo
     * @see kege::IndexedSetLayouts
     */
    struct IndexedSetConfig
    {
        /**
         * @var index
         * @brief Descriptor set index within the pipeline layout.
         */
        uint32_t index;

        /**
         * @var set
         * @brief Descriptor set configuration assigned to this index.
         */
        SetBindings bindings;
    };

    /**
     * @typedef kege::SetsConfigLayout
     * @brief Describes the descriptor set layout configuration for a shader layout.
     *
     * A IndexedSetLayouts defines the complete descriptor set layout for a shader or pipeline.
     * It consists of a list of indexed set configurations that specify how each set
     * is arranged and what bindings it contains.
     *
     * @see kege::IndexedSetConfig
     */
    using SetsConfigLayout = std::vector< IndexedSetConfig >;

}


namespace kege{

    /**
     * @brief Describes a single push constant range in a shader.
     *
     * Push constants are small uniform data blocks passed directly to shaders.
     * They are not bound like buffers or images, and their lifetime is limited to
     * the command buffer they are recorded in.
     */
    struct PushBlock
    {
        std::string name;           ///< Name of the push constant block in the shader.
        uint32_t    offset;         ///< Byte offset within the push constant range.
        uint32_t    size;           ///< Size in bytes of the push constant range.
        uint32_t    count;          ///< Count for total array elements
        kege::ShaderStageFlag stages; ///< Shader stages that can access this push constant.
        kege::ref::ShaderStructBlock block; ///< Structured layout of the constant data.
    };

    /**
     * @brief Describes all push constant ranges used in a single shader layout.
     *
     * A shader layout can define one or more push constant blocks, each with their
     * own offset, size, and visibility. Push constant ranges must not overlap.
     *
     * @see kege::PushBlockInfo
     */
    typedef std::vector< PushBlock > PushBlockLayout;


    struct ShaderLayoutDesc
    {
        std::string name;
        SetsConfigLayout set_layout_config;
        PushBlockLayout push_block_layout;
    };
    
}




namespace kege{

    struct BindingInfoHash
    {
        std::size_t operator()(const BindingInfo& b) const noexcept
        {
            std::size_t h = 0;

            auto combine = [&](std::size_t v) {
                h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            };

            combine(std::hash<uint32_t>{}(b.count));
            combine(std::hash<uint32_t>{}(b.binding));
            combine(std::hash<std::string>{}(b.name));

            // Enums -> cast to size_t is safe
            combine(std::hash<size_t>{}(static_cast<size_t>(b.usage)));
            combine(std::hash<size_t>{}(static_cast<size_t>(b.stages)));
            combine(std::hash<size_t>{}(static_cast<size_t>(b.type)));

            // Pointer -> hash the pointer value (NOT contents)
            combine(std::hash<const void*>{}(static_cast<const void*>(b.block.ref())));

            return h;
        }
    };

    struct SetBindingsHash
    {
        std::size_t operator()(const SetBindings& bindings) const noexcept
        {
            std::size_t h = 0;
            BindingInfoHash bindingHasher;

            auto combine = [&](std::size_t v) {
                h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            };

            for (const BindingInfo& b : bindings)
            {
                combine(bindingHasher(b));
            }

            return h;
        }
    };

    struct BindingInfoEqual
    {
        bool operator()(const BindingInfo& a, const BindingInfo& b) const noexcept
        {
            return a.count   == b.count &&
                   a.binding == b.binding &&
                   a.name    == b.name &&
                   a.usage   == b.usage &&
                   a.stages  == b.stages &&
                   a.block   == b.block &&   // pointer identity
                   a.type    == b.type;
        }
    };

    struct SetBindingsEqual
    {
        bool operator()(const SetBindings& A, const SetBindings& B) const noexcept
        {
            if (A.size() != B.size()) return false;
            for (size_t i = 0; i < A.size(); i++)
                if (!BindingInfoEqual{}(A[i], B[i]))
                    return false;
            return true;
        }
    };
}
#endif /* shader_binding_set_layout_desc_h */
