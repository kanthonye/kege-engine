//
//  vulkan-utility.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#ifndef vulkan_utility_hpp
#define vulkan_utility_hpp

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

#include "../core/resource-recycler.hpp"
#include "../core/command-recorder.hpp"
#include "../core/graphics-device.hpp"
#include "../core/graphics-window.hpp"
#include "../core/graphics-physical-device.hpp"

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
        int pipeline_layout_id;
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

}



namespace kege::vk{

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

    /**
     * @brief Translate engine attachment load operation to Vulkan load operation
     *
     * Maps AttachmentLoadOp enum to the appropriate VkAttachmentLoadOp value.
     *
     * @param op Engine attachment load operation
     * @return Corresponding VkAttachmentLoadOp value
     */
    VkAttachmentLoadOp convertAttachmentLoadOp( AttachmentLoadOp op );

    /**
     * @brief Translate engine attachment store operation to Vulkan store operation
     *
     * Maps AttachmentStoreOp enum to the appropriate VkAttachmentStoreOp value.
     *
     * @param op Engine attachment store operation
     * @return Corresponding VkAttachmentStoreOp value
     */
    VkAttachmentStoreOp convertAttachmentStoreOp( AttachmentStoreOp op );

    /**
     * @brief Translate engine image aspect to Vulkan image aspect flags
     *
     * Maps ImageAspectFlag enum to the appropriate VkImageAspectFlags value.
     *
     * @param aspect Engine image aspect
     * @return Corresponding VkImageAspectFlags value
     */
    VkImageAspectFlags convertImageAspect(ImageAspectFlag aspect);

    /**
     * @brief Translate Vulkan physical device type to engine physical device type
     *
     * Maps VkPhysicalDeviceType enum to the appropriate PhysicalDeviceType value.
     *
     * @param type Vulkan physical device type
     * @return Corresponding PhysicalDeviceType value
     */
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

    /**
     * @brief Translate VkFormat to engine Format type
     *
     * Maps VkFormat enum to the appropriate Format value.
     *
     * @param format Vulkan VkFormat type
     * @return Corresponding engine Format value
     */
    Format convertVkFormat( VkFormat format );

    /**
     * @brief Get the appropriate VkImageAspectFlags for a given Format
     *
     * Determines the image aspects (color, depth, stencil) based on the format.
     *
     * @param format Engine Format type
     * @return Corresponding VkImageAspectFlags
     */
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

    /**
     * @brief Translate engine shader stage to Vulkan shader stage flag bits
     *
     * Maps ShaderStage enum to the appropriate VkShaderStageFlagBits value.
     * Only supports individual stages, not combinations.
     *
     * @param stage Engine shader stage
     * @return Corresponding VkShaderStageFlagBits value
     */
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

    /**
     * @brief Translate engine image layout to Vulkan image layout
     *
     * Maps ImageLayout enum to the appropriate VkImageLayout value.
     *
     * @param image_layout Engine image layout
     * @return Corresponding VkImageLayout value
     */
    VkImageLayout convertImageLayout( ImageLayout image_layout );

    /**
     * @brief Translate engine access flags to Vulkan access flags
     *
     * Maps AccessFlags bitfield to the appropriate VkAccessFlags.
     * Each bit in the input maps to a specific Vulkan access flag.
     *
     * @param access Engine access flags
     * @return Corresponding VkAccessFlags
     */
    VkAccessFlags convertAccessFlag(AccessFlags access);

    /**
     * @brief Translate engine pipeline stage flags to Vulkan pipeline stage flags
     *
     * Maps PipelineStageFlag bitfield to the appropriate VkPipelineStageFlags.
     * Each bit in the input maps to a specific Vulkan pipeline stage flag.
     *
     * @param stages Engine pipeline stage flags
     * @return Corresponding VkPipelineStageFlags
     */
    VkPipelineStageFlags convertPipelineStageFlag(PipelineStageFlag stages);

    /**
      * @brief Translate engine resource state to Vulkan image layout
      *
      * Maps ResourceState enum to the appropriate VkImageLayout value.
      * Takes into account the format for depth/stencil layouts.
      *
      * @param state Engine resource state
      * @param format Format of the resource (needed for depth/stencil)
      * @return Corresponding VkImageLayout value
      */
    VkImageLayout translatesToVkLayout(ResourceState state, Format format);

    /**
     * @brief Translate engine color component flags to Vulkan color component flags
     *
     * Maps ColorComponentFlags bitfield to the appropriate VkColorComponentFlags.
     * Each bit in the input maps to a specific Vulkan color component flag.
     *
     * @param flags Engine color component flags
     * @return Corresponding VkColorComponentFlags
     */
    VkColorComponentFlags convertColorComponentMask( ColorComponentFlags flags );

    /**
     * @brief Translate engine blend factor to Vulkan blend factor
     *
     * Maps BlendFactor enum to the appropriate VkBlendFactor value.
     *
     * @param factor Engine blend factor
     * @return Corresponding VkBlendFactor value
     */
    VkBlendFactor convertBlendFactor( BlendFactor factor );

    /**
     * @brief Translate engine blend operation to Vulkan blend operation
     *
     * Maps BlendOp enum to the appropriate VkBlendOp value.
     *
     * @param op Engine blend operation
     * @return Corresponding VkBlendOp value
     */
    VkBlendOp convertBlendOp( BlendOp op );

    /**
     * @brief Translate engine compare operation to Vulkan compare operation
     *
     * Maps CompareOp enum to the appropriate VkCompareOp value.
     *
     * @param compare Engine compare operation
     * @return Corresponding VkCompareOp value
     */
    VkCompareOp convertCompareOp(CompareOp compare);


    VkLogicOp convertLogicOp( ColorBlendLogicOp op );
    
    /**
     * @brief Translate engine stencil operation to Vulkan stencil operation
     *
     * Maps StencilOp enum to the appropriate VkStencilOp value.
     *
     * @param op Engine stencil operation
     * @return Corresponding VkStencilOp value
     */
    VkStencilOp convertStencilOp(StencilOp op);

    /**
     * @brief Translate engine cull mode to Vulkan cull mode flags
     *
     * Maps CullMode enum to the appropriate VkCullModeFlags value.
     *
     * @param mode Engine cull mode
     * @return Corresponding VkCullModeFlags value
     */
    VkCullModeFlags convertCullMode( CullMode mode );

    /**
     * @brief Translate engine front face to Vulkan front face
     *
     * Maps FrontFace enum to the appropriate VkFrontFace value.
     *
     * @param face Engine front face
     * @return Corresponding VkFrontFace value
     */
    VkFrontFace convertFrontFace( FrontFace face );

    /**
     * @brief Translate engine polygon mode to Vulkan polygon mode
     *
     * Maps PolygonMode enum to the appropriate VkPolygonMode value.
     *
     * @param mode Engine polygon mode
     * @return Corresponding VkPolygonMode value
     */
    VkPolygonMode convertPolygonMode( PolygonMode mode );

    /**
     * @brief Translate engine vertex input rate to Vulkan vertex input rate
     *
     * Maps VertexInputRate enum to the appropriate VkVertexInputRate value.
     *
     * @param input_rate Engine vertex input rate
     * @return Corresponding VkVertexInputRate value
     */
    VkVertexInputRate convertVertexInputRate( VertexInputRate input_rate );

    /**
     * @brief Translate engine primitive topology to Vulkan primitive topology
     *
     * Maps PrimitiveTopology enum to the appropriate VkPrimitiveTopology value.
     *
     * @param topology Engine primitive topology
     * @return Corresponding VkPrimitiveTopology value
     */
    VkPrimitiveTopology convertPrimitiveTopology( kege::PrimitiveTopology topology );

    /**
     * @brief Translate engine buffer usage flags to Vulkan buffer usage flags
     *
     * Maps BufferUsage bitfield to the appropriate VkBufferUsageFlags.
     * Each bit in the input maps to a specific Vulkan usage flag.
     *
     * @param flags Engine buffer usage flags
     * @return Corresponding VkBufferUsageFlags
     */
    VkBufferUsageFlags convertBufferUsageFlags( BufferUsage flags );

    /**
     * @brief Translate engine memory usage to Vulkan memory property flags
     *
     * Maps MemoryUsage enum to the appropriate VkMemoryPropertyFlags.
     *
     * @param memory_usage Engine memory usage
     * @return Corresponding VkMemoryPropertyFlags
     */
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

    const char* vkResultToString(VkResult result);;
}
#endif /* vulkan_utility_hpp */
