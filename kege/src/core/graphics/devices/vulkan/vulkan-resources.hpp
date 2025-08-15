/**
 * @file vulkan_resources.hpp
 * @brief Vulkan-specific resource wrapper structures
 *
 * These structures encapsulate native Vulkan handles and maintain
 * their associated descriptors and metadata for proper resource
 * management, recreation, and debugging.
 */

#ifndef vulkan_resources_hpp
#define vulkan_resources_hpp

#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <map>
#include <atomic>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h> // For extensions like VK_KHR_portability_subset

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdocumentation"
//#pragma clang diagnostic ignored "-Wnullability-completeness"
//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)
//#pragma clang diagnostic pop

#include "graphics-device.hpp"
#include "graphics-window.hpp"
#include "graphics-physical-device.hpp"

namespace kege::vk{
    class Device;
    class Instance;

    /**
     * @brief Wrapper for Vulkan buffer resources
     *
     * Encapsulates a VkBuffer along with its memory allocation and metadata.
     * Uses VMA (Vulkan Memory Allocator) for efficient memory management.
     */
    struct Buffer
    {
        /** @brief Native Vulkan buffer handle */
        VkBuffer buffer = VK_NULL_HANDLE;

        /** @brief The buffer's memory */
        VkDeviceMemory memory = VK_NULL_HANDLE;

        /** @brief VMA allocation that manages the buffer's memory */
        //VmaAllocation allocation = VK_NULL_HANDLE;

        /** @brief Original buffer creation parameters for reference/recreation */
        kege::BufferDesc desc;

        /** @brief Pointer to mapped memory region (only valid if buffer is host-visible) */
        void* mapped_ptr = nullptr;
    };

    /**
     * @brief Wrapper for Vulkan image resources
     *
     * Encapsulates a VkImage along with its default view, memory allocation,
     * and current layout state tracking.
     */
    struct Image
    {
        /** @brief Native Vulkan image handle */
        VkImage image = VK_NULL_HANDLE;

        /** @brief Default image view for the texture */
        VkImageView view = VK_NULL_HANDLE;

        /** @brief The image's memory */
        VkDeviceMemory memory = VK_NULL_HANDLE;

        /** @brief The image's format */
        VkFormat format;

        /** @brief Original texture creation parameters for reference/recreation */
        kege::ImageDesc desc;

        /** @brief Current layout of the image for synchronization tracking */
        VkImageLayout current_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    /**
     * @brief Wrapper for Vulkan sampler resources
     *
     * Encapsulates a VkSampler along with its creation parameters.
     */
    struct Sampler
    {
        /** @brief Native Vulkan sampler handle */
        VkSampler sampler = VK_NULL_HANDLE;

        /** @brief Original sampler creation parameters for reference/recreation */
        kege::SamplerDesc desc;
    };

    /**
     * @brief Wrapper for Vulkan shader module resources
     *
     * Encapsulates a VkShaderModule and its associated metadata.
     */
    struct Shader
    {
        /** @brief Native Vulkan shader module handle */
        VkShaderModule shader_module = VK_NULL_HANDLE;

        /** @brief Original shader creation parameters for reference/recreation */
        kege::ShaderDesc desc;
    };

    /**
     * @brief Wrapper for Vulkan graphics pipeline resources
     *
     * Encapsulates a VkPipeline configured for graphics operations.
     * Could be extended to support pipeline caching.
     */
    struct GraphicsPipeline
    {
        /** @brief Native Vulkan pipeline handle */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /** @brief Original pipeline creation parameters for reference/recreation */
        kege::GraphicsPipelineDesc desc;
        VkPipelineBindPoint bind_point;
    };

    /**
     * @brief Wrapper for Vulkan compute pipeline resources
     *
     * Encapsulates a VkPipeline configured for compute operations.
     */
    struct ComputePipeline
    {
        /** @brief Native Vulkan pipeline handle */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /** @brief Original pipeline creation parameters for reference/recreation */
        kege::ComputePipelineDesc desc;
    };

    /**
     * @brief Wrapper for Vulkan fence synchronization primitives
     *
     * Encapsulates a VkFence for CPU-GPU synchronization.
     */
    struct Fence
    {
        /** @brief Native Vulkan fence handle */
        VkFence fence = VK_NULL_HANDLE;
    };

    /**
     * @brief Wrapper for Vulkan semaphore synchronization primitives
     *
     * Encapsulates a VkSemaphore for GPU-GPU synchronization.
     */
    struct Semaphore
    {
        /** @brief Native Vulkan semaphore handle */
        VkSemaphore semaphore = VK_NULL_HANDLE;
    };

    /**
     * @struct QueueInfo
     * @brief Holds information about a queue and its family
     */
    struct QueueInfo
    {
        /** @brief Queue handle */
        VkQueue queue = VK_NULL_HANDLE;

        /** @brief Queue family index */
        uint32_t family_index = UINT32_MAX;
    };

    /**
     * @brief Overloads the bitwise OR operator for ImageAspectFlag.
     *
     * This allows combining multiple texture aspects using the | operator.
     *
     * @param lhs The left-hand side ImageAspectFlag.
     * @param rhs The right-hand side ImageAspectFlag.
     * @return A new ImageAspectFlag with the combined aspects.
     */
    inline ImageAspectFlag operator|(ImageAspectFlag lhs, ImageAspectFlag rhs)
    {
        return static_cast<ImageAspectFlag>( static_cast< uint32_t >( lhs ) | static_cast< uint32_t >( rhs ) );
    }

    /**
     * @brief Overloads the bitwise AND operator for ImageAspectFlag.
     *
     * This allows checking if a specific aspect is present in a ImageAspectFlag.
     *
     * @param lhs The left-hand side ImageAspectFlag.
     * @param rhs The right-hand side ImageAspectFlag to check against.
     * @return A new ImageAspectFlag with only the common aspects.
     */
    inline ImageAspectFlag operator&(ImageAspectFlag lhs, ImageAspectFlag rhs)
    {
        return static_cast<ImageAspectFlag>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
    }

    /**
     * @brief Overloads the bitwise XOR operator for ImageAspectFlag.
     *
     * This allows toggling or finding the differing aspects between two ImageAspectFlag values.
     *
     * @param lhs The left-hand side ImageAspectFlag.
     * @param rhs The right-hand side ImageAspectFlag to XOR with.
     * @return A new ImageAspectFlag with the aspects that are present in one but not both operands.
     */
    inline ImageAspectFlag operator^(ImageAspectFlag lhs, ImageAspectFlag rhs)
    {
        return static_cast<ImageAspectFlag>(
            static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
    }

    /**
     * @brief Overloads the bitwise NOT operator for ImageAspectFlag.
     *
     * This allows getting a ImageAspectFlag with all aspects *not* present in the original.
     *
     * @param flag The ImageAspectFlag to negate.
     * @return A new ImageAspectFlag with the inverse set of aspects.
     */
    inline ImageAspectFlag operator~(ImageAspectFlag flag)
    {
        return static_cast<ImageAspectFlag>(~static_cast<uint32_t>(flag));
    }

    /**
     * @brief Overloads the bitwise OR assignment operator for ImageAspectFlag.
     *
     * This allows combining another texture aspect into an existing ImageAspectFlag.
     *
     * @param lhs The ImageAspectFlag to modify.
     * @param rhs The ImageAspectFlag to OR with.
     * @return A reference to the modified ImageAspectFlag.
     */
    inline ImageAspectFlag& operator|=(ImageAspectFlag& lhs, ImageAspectFlag rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    /**
     * @brief Overloads the bitwise AND assignment operator for ImageAspectFlag.
     *
     * This allows keeping only the common aspects in an existing ImageAspectFlag.
     *
     * @param lhs The ImageAspectFlag to modify.
     * @param rhs The ImageAspectFlag to AND with.
     * @return A reference to the modified ImageAspectFlag.
     */
    inline ImageAspectFlag& operator&=(ImageAspectFlag& lhs, ImageAspectFlag rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    /**
     * @brief Overloads the bitwise XOR assignment operator for ImageAspectFlag.
     *
     * This allows toggling the aspects in an existing ImageAspectFlag based on another.
     *
     * @param lhs The ImageAspectFlag to modify.
     * @param rhs The ImageAspectFlag to XOR with.
     * @return A reference to the modified ImageAspectFlag.
     */
    inline ImageAspectFlag& operator^=(ImageAspectFlag& lhs, ImageAspectFlag rhs)
    {
        lhs = lhs ^ rhs;
        return lhs;
    }


    VkAttachmentLoadOp convertAttachmentLoadOp( AttachmentLoadOp op );
    VkAttachmentStoreOp convertAttachmentStoreOp( AttachmentStoreOp op );

    VkImageAspectFlags convertImageAspect(ImageAspectFlag aspect);

    PhysicalDeviceType convertPhysicalDeviceType( VkPhysicalDeviceType type );
    
    /**
     * @brief Translate engine Format type to VkFormat
     *
     * Maps Format enum to the appropriate VkFormat value.
     *
     * @param format Engine Format type
     * @return Corresponding VkFormat value
     */
    VkFormat convertFormat( Format format );

    Format convertVkFormat( VkFormat format );

    VkImageAspectFlags getImageAspectFlags( Format format );

    /**
     * @brief Translate engine texture type to Vulkan image type
     *
     * Maps ImageType enum to the appropriate VkImageType value.
     *
     * @param type Engine texture type
     * @return Corresponding VkImageType value
     */
    VkImageType convertTextureType(ImageType type);

    /**
     * @brief Translate engine texture type to Vulkan image view type
     *
     * Maps ImageType enum to the appropriate VkImageViewType value.
     * Image views handle array and cubemap configurations.
     *
     * @param type Engine texture type
     * @return Corresponding VkImageViewType value
     */
    VkImageViewType convertTextureViewType(ImageType type);

    /**
     * @brief Translate engine texture usage flags to Vulkan image usage flags
     *
     * Maps ImageUsageFlags bitfield to the appropriate VkImageUsageFlags.
     * Each bit in the input maps to a specific Vulkan usage flag.
     *
     * @param usage Engine texture usage flags
     * @return Corresponding VkImageUsageFlags
     */
    VkImageUsageFlags convertTextureUsage(ImageUsageFlags usage);

    /**
     * @brief Translate engine buffer usage flags to Vulkan buffer usage flags
     *
     * Maps BufferUsage bitfield to the appropriate VkBufferUsageFlags.
     * Each bit in the input maps to a specific Vulkan usage flag.
     *
     * @param usage Engine buffer usage flags
     * @return Corresponding VkBufferUsageFlags
     */
    VkBufferUsageFlags convertBufferUsage(BufferUsage usage);

    /**
     * @brief Translate engine memory usage to VMA memory usage
     *
     * Maps MemoryUsage enum to the appropriate VmaMemoryUsage value.
     * This determines memory allocation strategy in VMA.
     *
     * @param usage Engine memory usage type
     * @return Corresponding VmaMemoryUsage value
     */
    //VmaMemoryUsage convertMemoryUsage(MemoryUsage usage);

    /**
     * @brief Translate engine sample count to Vulkan sample count flags
     *
     * Maps SampleCount enum to the appropriate VkSampleCountFlagBits value.
     *
     * @param count Engine sample count
     * @return Corresponding VkSampleCountFlagBits value
     */
    VkSampleCountFlagBits convertSampleCount(SampleCount count);

    /**
     * @brief Translate engine shader stage to Vulkan shader stage flags
     *
     * Maps ShaderStage enum to the appropriate VkShaderStageFlags value.
     * Supports mapping individual stages and combinations of stages.
     *
     * @param stage Engine shader stage
     * @return Corresponding VkShaderStageFlags value
     */
    VkShaderStageFlags convertShaderStageMask(ShaderStage stage);

    VkShaderStageFlagBits convertShaderStage(ShaderStage stage);

    /**
     * @brief Translate engine filter mode to Vulkan filter mode
     *
     * @param filter Engine filter mode
     * @return Corresponding VkFilter value
     */
    VkFilter convertFilter(Filter filter);

    /**
     * @brief Translate engine mipmap mode to Vulkan mipmap mode
     *
     * Maps MipmapMode enum to the appropriate VkSamplerMipmapMode value.
     *
     * @param filter Engine mipmap mode
     * @return Corresponding VkSamplerMipmapMode value
     */
    VkSamplerMipmapMode convertMipmapMode(MipmapMode filter);

    /**
     * @brief Translate engine address mode to Vulkan address mode
     *
     * Maps AddressMode enum to the appropriate VkSamplerAddressMode value.
     *
     * @param mode Engine address mode
     * @return Corresponding VkSamplerAddressMode value
     */
    VkSamplerAddressMode convertAddressMode(AddressMode mode);

    /**
     * @brief Translate engine compare operation to Vulkan compare op
     *
     * Maps CompareOp enum to the appropriate VkCompareOp value.
     *
     * @param compare Engine compare operation
     * @return Corresponding VkCompareOp value
     */
    VkCompareOp convertCompareOp(CompareOp compare);

    /**
     * @brief Translate engine border color to Vulkan border color
     *
     * Maps BorderColor enum to the appropriate VkBorderColor value.
     *
     * @param color Engine border color
     * @return Corresponding VkBorderColor value
     */
    VkBorderColor convertBorderColor(BorderColor color);

    /**
     * @brief Translate engine descriptor type to Vulkan descriptor type
     *
     * Maps DescriptorType enum to the appropriate VkDescriptorType value.
     *
     * @param type Engine descriptor type
     * @return Corresponding VkDescriptorType value
     */
    VkDescriptorType convertDescriptorType( DescriptorType type );

    VkImageLayout convertImageLayout( ImageLayout image_layout );
    VkAccessFlags convertAccessFlag(AccessFlags access);
    VkPipelineStageFlags convertPipelineStageFlag(PipelineStageFlag stages);
    VkImageLayout translatesToVkLayout(ResourceState state, Format format);



    VkColorComponentFlags convertColorComponentMask( ColorComponentFlags flags );
    VkBlendFactor convertBlendFactor( BlendFactor factor );
    VkBlendOp convertBlendOp( BlendOp op );
    VkCompareOp convertCompareOp(CompareOp compare);
    VkStencilOp convertStencilOp(StencilOp op);
    VkCullModeFlags convertCullMode( CullMode mode );
    VkFrontFace convertFrontFace( FrontFace face );
    VkPolygonMode convertPolygonMode( PolygonMode mode );
    VkVertexInputRate convertVertexInputRate( VertexInputRate input_rate );
    VkPrimitiveTopology convertPrimitiveTopology( kege::PrimitiveTopology topology );

    VkBufferUsageFlags convertBufferUsageFlags( BufferUsage flags );
    VkMemoryPropertyFlags convertMemoryPropertyFlags( MemoryUsage memory_usage );

    /**
     * @brief Find queue families on a physical device
     *
     * Identifies queue families that support the required operations.
     *
     * @param physical_device Physical device to check
     * @param surface Temporary surface for present capabilities check
     * @return Structure containing the queue family indices
     */
    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice physical_device, VkSurfaceKHR surface );
    
    // --- Helper Function for Combining Hashes ---
    // (Often based on Boost's hash_combine)
    template <class T> inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        // Combine the hash of v with the current seed
        // Uses a prime number and bit shifts for better distribution
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    void logVkError(VkResult result, const char* from, const char* where );

}


namespace std{

    template <> struct hash< std::vector< kege::DescriptorType > >
    {
        std::size_t operator()( const std::vector< kege::DescriptorType >& types ) const
        {
            std::size_t seed = 0;

            // Combine hash of the size to quickly differentiate vectors of different lengths
            kege::vk::hash_combine(seed, types.size());

            // Combine the hash of each element in order
            for (const auto& type : types)
            {
                kege::vk::hash_combine( seed, static_cast< int >( type ) );
            }
            return seed;
        }
    };
    
    template <> struct hash< std::vector< kege::DescriptorSetLayoutBinding > >
    {
        std::size_t operator()( const std::vector< kege::DescriptorSetLayoutBinding >& bindings ) const
        {
            std::size_t seed = 0;

            // Combine hash of the size to quickly differentiate vectors of different lengths
            kege::vk::hash_combine(seed, bindings.size());

            // Combine the hash of each element in order
            for (const auto& binding : bindings)
            {
                seed ^= bindingHasher( binding ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                // Note: Using ^= directly here instead of hash_combine(seed, bindingHasher(binding))
                //       is slightly different but still a common pattern for combining element hashes.
                //       Using hash_combine(seed, bindingHasher(binding)) is also perfectly valid.
            }
            return seed;
        }
        
        std::size_t bindingHasher(const kege::DescriptorSetLayoutBinding& binding) const
        {
            std::size_t seed = 0;

            // Combine hashes of the relevant members
            kege::vk::hash_combine(seed, binding.binding);
            // Hash the enum value directly (underlying type is usually int)
            kege::vk::hash_combine(seed, static_cast<int>(binding.descriptor_type));
            kege::vk::hash_combine(seed, binding.count);
            // Hash the flags directly (underlying type is usually int/uint)
            kege::vk::hash_combine(seed, static_cast<uint32_t>(binding.stage_flags));

            // ** Important Caveat: pImmutableSamplers **
            // We generally DO NOT hash pImmutableSamplers for several reasons:
            // 1. It's a pointer: Hashing the address is meaningless for value comparison.
            // 2. VkSampler is an opaque handle: We cannot easily hash the "value"
            //    of the sampler itself in a consistent way across runs or different
            //    sampler objects that might be functionally identical.
            // 3. Complexity: Hashing the pointed-to sampler handles would require
            //    knowing the descriptorCount and iterating, adding complexity.
            //
            // If you *absolutely* need to differentiate layouts based *only* on
            // immutable samplers, you'll need a more complex strategy, potentially
            // hashing the VkSampler handles directly IF AND ONLY IF you guarantee
            // those handles are canonical and stable for equivalent layouts.
            // For most caching purposes, ignoring pImmutableSamplers is acceptable.
            // if (binding.pImmutableSamplers != nullptr) {
            //     // Complex: Potentially hash the handles pointed to, requires care
            //     for (uint32_t i = 0; i < binding.descriptorCount; ++i) {
            //         // Be careful: Hashing handle values might not be stable!
            //         hash_combine(seed, reinterpret_cast<size_t>(binding.pImmutableSamplers[i]));
            //     }
            // }

            return seed;
        }
    };

    // Define equality comparison for VkDescriptorSetLayoutBinding
    inline bool operator==(const kege::DescriptorSetLayoutBinding& a, const kege::DescriptorSetLayoutBinding& b)
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
    inline bool operator!=(const kege::DescriptorSetLayoutBinding& a, const kege::DescriptorSetLayoutBinding& b)
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
    inline bool operator<(const kege::DescriptorSetLayoutBinding& a, const kege::DescriptorSetLayoutBinding& b)
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
    inline bool operator==(const std::vector< kege::DescriptorSetLayoutBinding >& a, const std::vector< kege::DescriptorSetLayoutBinding >& b)
    {
        if( a.size() != b.size() ) return false;
        for(int i=0; i<a.size(); ++i)
            if( a[i] != b[i] )
                return false;
        return true;
    }
}

#endif /* vulkan_resources_hpp */
