//
//  render-pass-context.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#ifndef render_pass_context_hpp
#define render_pass_context_hpp

#include <unordered_set>
#include "../../../utils/array.hpp"
#include "../../../utils/communication.hpp"
#include "../../../utils/json-parser.hpp"
#include "../../render/core/graphics.hpp"

namespace kege{

    struct RenderStage;
    struct RenderGraph;

    enum class RenderPassType
    {
        BarrierTransition,   // Full-screen post-processing effects
        DepthPrePass,        // Z-prepass for depth filling
        ShadowMap,           // Shadow mapping pass
        Geometry,            // G-buffer generation (deferred)
        Lighting,            // G-buffer generation (deferred)
        ForwardOpaque,       // Forward rendering for opaque objects
        ForwardTransparent,  // Forward rendering for transparent objects
        PostProcess,         // Full-screen post-processing effects
        UI,                  // User interface rendering
        Count,
        // Custom passes can be added as needed
    };

    enum struct RgResrcType{ Invalid, Buffer, Image, Sampler, BufferView, ShaderResource };
    enum struct RgPassType{ Invalid, Graphics, Compute };

    struct RgResrcHandle
    {
        //inline operator int32_t()const{ return index; }
        inline operator size_t()const{ return index; }
        inline operator uint64_t()const{ return index; }
        inline operator bool()const{ return index != 0; }
        RgResrcType type = RgResrcType::Invalid;
        uint64_t index = 0;
    };

    constexpr inline bool operator==(const kege::RgResrcHandle& a, const kege::RgResrcHandle& b)
    {
        return a.index == b.index;
    }

    constexpr inline bool operator!=(const kege::RgResrcHandle& a, const kege::RgResrcHandle& b)
    {
        return a.index != b.index;
    }
    constexpr inline bool operator<(const kege::RgResrcHandle& a, const kege::RgResrcHandle& b)
    {
        return a.index < b.index;
    }



    struct BufferDefn
    {
        std::string name;

        /** @brief Number of frames in flight for this buffer */
        uint32_t frames;

        /** @brief Buffer description */
        BufferDesc info;

        /** @brief Handle to the resource in the render graph */
        RgResrcHandle handle = {};

        /** @brief Physical buffer handles for each frame in flight */
        std::vector< ref::Buffer > physical_handle;
    };

    struct SamplerDefn
    {
        /** @brief Name of the sampler resource */
        std::string name;

        /** @brief Sampler description */
        kege::SamplerDesc desc;

        /** @brief Handle to the resource in the render graph */
        RgResrcHandle handle;

        /** @brief Physical sampler handle */
        ref::Sampler physical_handle;
    };

    struct ImageDefn
    {
        /** @brief Name of the image resource */
        std::string name;

        /** @brief Number of frames in flight for this image */
        uint32_t frames;

        /** @brief Image usage flags */
        ImageUsage usages = ImageUsage::Undefined;

        /** @brief Initial image layout */
        ImageLayout layout = ImageLayout::Undefined;

        /** @brief Image description and properties */
        struct Info
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            kege::Format format;
            kege::ImageType type;
        }
        info;

        /** @brief Indicates if the image uses the swapchain image at the current index */
        bool use_swapchain_image_index = false;
        
        /** @brief Handle to the resource in the render graph */
        std::vector< ref::Image > physical_handle;

        /** @brief Handle to the resource in the render graph */
        RgResrcHandle handle = {};
     };





    struct RgShaderResrcInfo
    {
        RgResrcType type;
        std::string name;
        std::string sampler;
    };

    struct RgShaderResrcDesc
    {
        kege::ShaderStageFlag stages = kege::ShaderStageFlag::All;
        DescriptorType type;
        std::string name;
        uint32_t binding;
        uint32_t count;
        std::vector< RgShaderResrcInfo > targets;
    };

    typedef std::vector< kege::Ref< ShaderResrc > > ShaderResrcs;
    struct RgShaderResrcDefn
    {
        std::string name;
        uint32_t set_index;
        uint32_t frames;
        std::vector< RgShaderResrcDesc > bindings;

        RgResrcHandle handle = {};
        ShaderResrcs physical_handles;
    };




    struct RgResrcUsage
    {
        AccessFlags access = AccessFlags::None;
        PipelineStageFlag stage = PipelineStageFlag::TopOfPipe;
        ImageLayout layout = ImageLayout::Undefined;
        AttachmentLoadOp load_op = AttachmentLoadOp::Clear;
    };

    struct RgReadResrcDesc
    {
        std::string name;
        RgResrcType type;
        RgResrcUsage usage;

        RgResrcHandle handle;
    };

    struct RgWriteResrcDesc
    {
        std::string name;
        
        /**
         * @brief Type of the resource being written to.
         */
        RgResrcType type;

        /**
         * @brief Usage details for this resource within the render pass.
         */
        RgResrcUsage usage;

        /**
         * @brief Clear value to use if the resource is an attachment with loadOp Clear
         */
        ClearValue clear_value;
        
        /**
         * @brief Handle to the resource being written to.
         */
        RgResrcHandle handle;
    };



    struct RgImageLayoutTransition
    {
        /** @brief Name of the image resource */
        std::string name;

        /** @brief Handle to the resource in the render graph */
        ImageLayout layout = ImageLayout::Undefined;

        /** @brief Aspect of the image to transition */
        ImageAspectFlag image_aspect;
    };

    struct RgResrcBarrierInfo
    {
        kege::RgResrcHandle resource_handle  = {};

        PipelineStageFlag src_stage_mask = PipelineStageFlag::None;
        PipelineStageFlag dst_stage_mask = PipelineStageFlag::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
        ImageLayout old_layout = ImageLayout::Undefined;
        ImageLayout new_layout = ImageLayout::Undefined;

        /// @name Buffer Region Specification
        /// @{
        size_t offset = 0;        ///< Starting byte offset of the affected buffer region
        size_t size = ~0ull;      ///< Size of the affected region in bytes (~0ull means entire buffer)
        /// @}

        /// @name Queue Family Ownership
        /// @{
        QueueType src_queue = QueueType::Graphics; ///< Queue family that previously owned the buffer
        QueueType dst_queue = QueueType::Graphics; ///< Queue family that will own the buffer
        /// @}
        ///
        std::string name;
    };

    typedef std::vector< RgResrcBarrierInfo > BarrierDescriptions;



    struct RenderPassDefn
    {
        std::string name;
        QueueType type;
        RenderPassType pass;

        /**
         * Resources read/consumed by this render pass.
         * These resources are produced by previous passes.
         */
        std::vector< RgReadResrcDesc > reads;

        /**
         * Resources written/produced by this render pass.
         * These resources can be used as inputs for subsequent passes.
         */
        std::vector< RgWriteResrcDesc > writes;

        /**
         * Supported pipelines for post process render passes, shadow passes, etc.
         * If defined these pipelines are used, instead of the pipelines used by
         * the material. pipelines could be 0 or multiple.
         */
        std::vector< ShaderPipeline > pipelines;

        /**
         * Barriers to apply at the start of this render pass.
         * These ensure proper resource state transitions before usage.
         * Node: Populating barriers is optional. If left empty then the barriers 
         * will be automatically generated by the render graph.
         */
        std::vector< RgResrcBarrierInfo > barriers;
    };

}

namespace std{

    template <> struct hash< kege::RgResrcHandle >
    {
        std::size_t operator()( const kege::RgResrcHandle& handle ) const
        {
            return std::hash<int>()(static_cast<int>( handle.type )) ^ (std::hash<int>()( handle.index ) << 1);
        }
    };
}
#endif /* render_pass_context_hpp */
