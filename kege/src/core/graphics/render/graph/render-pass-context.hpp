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
#include "../../../renderer/core/graphics.hpp"
#include "../../../renderer/core/graphics.hpp"

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
        inline operator int32_t()const{ return index; }
        inline operator bool()const{ return index >= 0; }
        RgResrcType type = RgResrcType::Invalid;
        int32_t index = -1;
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
        uint32_t frames_in_flight;
        BufferDesc info;

        RgResrcHandle handle = {};
        std::vector< kege::BufferHandle > physical_handle;
    };

    struct SamplerDefn
    {
        std::string name;
        kege::SamplerDesc desc;
        RgResrcHandle handle;
        kege::SamplerHandle physical_handle;
    };

    struct ImageDefn
    {
        std::string name;
        uint32_t frames_in_flight;

        ImageUsage usages = ImageUsage::Undefined;
        ImageLayout layout = ImageLayout::Undefined;

        struct Info
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            kege::Format format;
            kege::ImageType type;
        }
        info;
        bool use_swapchain_image_index = false;

        RgResrcHandle handle = {};
        std::vector< kege::ImageHandle > physical_handle;
    };

    struct RgImageLayoutTransition
    {
        std::string name;
        ImageLayout layout = ImageLayout::Undefined;
        ImageAspectFlag image_aspect;
    };

    struct RgShaderResrcInfo
    {
        RgResrcType type;
        std::string name;
        std::string sampler;
    };

    struct RgShaderResrcDesc
    {
        kege::ShaderStage stages = kege::ShaderStage::All;
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
        uint32_t frames_in_flight;
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

        RgResrcHandle handle; // still needed
    };

    struct RgWriteResrcDesc
    {
        std::string name;
        RgResrcType type;
        RgResrcUsage usage;

        std::optional< ClearValue > clear_value;
        
        RgResrcHandle handle;
    };



    struct RenderPassDefn
    {
        std::string name;
        QueueType type;
        RenderPassType pass;
        std::vector< RgReadResrcDesc > reads;
        std::vector< RgWriteResrcDesc > writes;

        /**
         * Supported pipelines for post process render passes, shadow passes, etc.
         * If defined these pipelines are used, instead of the pipelines used by
         * the material. pipelines could be 0 or multiple.
         */
        std::vector< ShaderPipeline > pipelines;
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

//    struct RgResrcHandleHash
//    {
//        std::size_t operator()(const RgResrcHandle& h) const noexcept
//        {
//            return std::hash<int>()(static_cast<int>(h.type)) ^ (std::hash<int>()(h.index) << 1);
//        }
//    };
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
