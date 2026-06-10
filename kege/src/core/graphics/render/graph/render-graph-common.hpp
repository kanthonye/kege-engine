//
//  render-graph-common.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/23/26.
//

#ifndef render_graph_common_hpp
#define render_graph_common_hpp

#include <set>
#include <unordered_set>
#include "../../../math/geometry/primitive-3D-shapes.hpp"
#include "../../../graphics/components/camera/camera.hpp"
#include "../../../graphics/components/camera/frustum.hpp"
#include "../../../graphics/core/device/core/device.hpp"

namespace kege{

    class Renderer;

    enum class PushType
    {
        None,
        PushModel,
        PushCamera,
    };

    struct PushBuffer
    {
        char data[256];
    };
    struct PushField
    {
        ShaderStageFlag stages;
        PushType type;
        uint32_t offset;
        uint32_t size;
        PushBuffer buffer;
    };

    /**
     * @brief A draw item represents a single drawable entity in the scene that has passed frustum culling. 
     * It contains the model matrices for the instances of this drawable, as well as the pipeline, material, 
     * and mesh IDs needed for rendering. The render graph will use these draw items to create render batches 
     * that can be efficiently rendered together.
     */
    struct DrawItem
    {
        kege::ModelMatrices matrices;

        uint64_t pipeline_id;
        uint64_t material_id;
        uint64_t mesh_id;

        kege::RenderPassType pass;

        uint32_t first_element = 0;
        uint32_t element_count = 0;
        uint32_t first_instance = 0;
        uint32_t instance_count = 0;
        uint32_t vertex_offset = 0;

        uint32_t render_layer = 0;
    };

    /**
     * @brief A render batch represents a group of draw calls that can be rendered together because 
     * they share the same pipeline, material, and mesh. Each batch contains the necessary information 
     * for rendering, such as the pipeline and material IDs, the mesh ID, the draw parameters 
     * (element count, instance count, etc.), and a list of instance matrices for instanced rendering. 
     * Batches are organized by render pass and are used by the render graph to efficiently execute 
     * draw calls while minimizing state changes.
     */
    struct RenderBatch
    {
        uint64_t pipeline_id;
        uint64_t material_id;
        uint64_t mesh_id;

        kege::RenderPassType pass;

        uint32_t first_element = 0;
        uint32_t element_count = 0;
        uint32_t first_instance = 0;
        uint32_t instance_count = 0;
        uint32_t vertex_offset = 0;

        uint32_t render_layer = 0;

        bool culled = false;

        std::vector< kege::ModelMatrices > instances;
    };

    /**
     * @brief A cache for storing render batches organized by render pass.
     */
    using RenderBatchPtrs = std::vector< kege::RenderBatch* >;

    /**
     * @brief A cache for storing render batches for all render passes. This is used by the render graph to 
     * quickly access the batches that need to be rendered for each pass during execution. Each entry in the
     * array corresponds to a specific render pass type, and contains a list of pointers to the batches that
     * belong to that pass. This allows the render graph to efficiently iterate over the batches for each pass
     * without having to filter through all batches at runtime.
     */
    typedef std::array< kege::RenderBatchPtrs, int(kege::RenderPassType::Count)> RenderPassBatchTable;

    /**
     * @brief A render view represents the view of the scene from a particular camera's perspective. It contains
     * the camera data, view frustum, viewport extent, and a list of render batches that are visible in this view.
     * The render graph will use the render views to determine which batches to render for each pass, and to perform
     * culling based on the view frustum. Each render view corresponds to a camera in the scene, and there may be 
     * multiple render views if there are multiple cameras (e.g. for split-screen or editor views).
     */
    struct RenderView : kege::RefCounter
    {
        kege::RenderPassBatchTable render_pass_batchs;
        std::vector< kege::RenderBatch > batches;

        kege::CameraData camera;
        kege::Frustum frustum;
        Extent2D extent;

        std::set< kege::RenderPassType > pass_types;
    };

    /**
     * @brief A frame of render views.
     */
    using RenderViewFrame = std::vector< kege::Ref< kege::RenderView > >;




    using PassId = uint32_t;

    /**
     * @brief A rrender graph resource type enumeration to categorize the different types of 
     * resources that can be used in the render graph. This helps the render graph manage 
     * resources and apply the correct handling based on the type of resource.
     */
    enum struct RgResrcType
    {
        Invalid,
        Buffer,
        Image,
        Sampler,
        BufferView,
        ShaderResource
    };

    /**
     * @brief A render graph buffer resource description that defines the properties and usage of a 
     * buffer resource within the render graph. This includes the name of the resource, the number 
     * of frames in flight, the size of the buffer, the usage flags that indicate how the buffer will 
     * be used (e.g. vertex buffer, uniform buffer etc.), and the memory usage pattern 
     * (e.g. GPU-only, CPU-to-GPU, etc.). The render graph will use this description to create and 
     * manage the buffer resource appropriately.
     */
    struct RgBufferDesc
    {
        std::string name;

        /** @brief Number of frames in flight for this buffer */
        uint32_t frames_in_flight;

        /** @brief Buffer description */
        uint64_t size = 0;
        kege::BufferUsages usage = kege::BufferUsages::None;
        kege::MemoryUsage memory_usage = kege::MemoryUsage::GpuOnly;
    };

    /**
     * @brief A render graph sampler resource description that defines the properties of a sampler resource
     * within the render graph. This includes the name of the sampler resource, the filtering modes for 
     * magnification and minification, the mipmap mode for level-of-detail sampling, and the address modes 
     * for texture coordinate wrapping. The render graph will use this description to create and manage
     * the sampler resource appropriately, and to bind it to the correct shader stages during rendering.
     */
    struct RgSamplerDesc
    {
        /** @brief Name of the image resource */
        std::string name;
        kege::Filter mag_filter;
        kege::Filter min_filter;
        kege::MipmapMode mipmap_mode;
        kege::AddressMode address_mode_u;
        kege::AddressMode address_mode_v;
        kege::AddressMode address_mode_w;
    };

    /**
     * @brief A render graph image resource description that defines the properties and usage of an image resource
     * within the render graph. This includes the name of the image resource, the number of frames in flight, the 
     * usage flags that indicate how the image will be used (e.g. color attachment, sampled image, etc.), the initial 
     * layout of the image, and the image properties such as extent, format, and type. The render graph will use this
     *  description to create and manage the image resource appropriately, and to apply the correct layout transitions 
     * and access patterns during rendering.
     */
    struct RgImageDesc
    {
        /** @brief Name of the image resource */
        std::string name;

        /** @brief Number of frames in flight for this image */
        uint32_t frames_in_flight;

        /** @brief Image usage flags */
        kege::ImageUsage usages = kege::ImageUsage::Undefined;

        /** @brief Initial image layout */
        kege::ImageLayout layout = kege::ImageLayout::Undefined;

        /** @brief Image description and properties */
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t array_layers = 1;
        kege::Format format;
        kege::ImageType type;
        bool is_swapcain_color = false;
        bool is_swapcain_depth = false;
     };

    /**
     * @brief A render graph resource handle that serves as an opaque identifier for resources within the render graph. 
     * This handle can be used to reference resources such as buffers, images, and samplers in a way that abstracts 
     * away the underlying implementation details. The render graph will use these handles to manage resource lifetimes, 
     * track dependencies, and perform lookups when executing render passes. The handle can be extended with additional 
     * metadata if needed, but for now it simply contains a unique identifier.
     */
    struct RgHandle
    {
        inline operator bool()const{ return id != 0; }
        kege::RgResrcType type = kege::RgResrcType::Invalid;
        uint64_t id = 0;
    };






    struct RgResrcUsage
    {
        kege::AccessFlags access = kege::AccessFlags::None;
        kege::PipelineStageFlag stage = kege::PipelineStageFlag::TopOfPipe;
        kege::ImageLayout layout = kege::ImageLayout::Undefined;
        kege::AttachmentLoadOp load_op = kege::AttachmentLoadOp::Clear;
    };

    struct RgReadResrcDesc
    {
        std::string name;

        /**
         * @brief Type of the resource being written to.
         */
        kege::RgResrcType type;

        /**
         * @brief Usage details for this resource within the render pass.
         */
        kege::AccessFlags access = kege::AccessFlags::None;
        kege::ImageLayout layout = kege::ImageLayout::Undefined;
        kege::PipelineStageFlag stage = kege::PipelineStageFlag::TopOfPipe;

        /**
         * @brief Handle to the resource being written to.
         */
        kege::RgHandle resource = {};

        kege::GlobalSemantic semantic;
    };

    struct RgWriteResrcDesc
    {
        std::string name;

        /**
         * @brief Type of the resource being written to.
         */
        kege::RgResrcType type;

        /**
         * @brief Usage details for this resource within the render pass.
         */
        //RgResrcUsage usage;
        kege::AccessFlags access = kege::AccessFlags::None;
        kege::PipelineStageFlag stage = kege::PipelineStageFlag::TopOfPipe;
        kege::ImageLayout layout = kege::ImageLayout::Undefined;
        kege::AttachmentLoadOp load_op = kege::AttachmentLoadOp::Clear;

        /**
         * @brief Clear value to use if the resource is an attachment with loadOp Clear
         */
        kege::ClearValue clear_value;

        /**
         * @brief Handle to the resource being written to.
         */
        kege::RgHandle resource;

        bool is_external = false;
        bool is_root = false;
    };


    struct RgResrcBarrier
    {
        kege::RgHandle resource  = {};

        kege::PipelineStageFlag src_stage = kege::PipelineStageFlag::None;
        kege::PipelineStageFlag dst_stage = kege::PipelineStageFlag::None;
        kege::AccessFlags src_access = kege::AccessFlags::None;
        kege::AccessFlags dst_access = kege::AccessFlags::None;
        kege::ImageLayout old_layout = kege::ImageLayout::Undefined;
        kege::ImageLayout new_layout = kege::ImageLayout::Undefined;

        /// @name Buffer Region Specification
        /// @{
        size_t offset = 0;        ///< Starting byte offset of the affected buffer region
        size_t size = ~0ull;      ///< Size of the affected region in bytes (~0ull means entire buffer)
        /// @}

        /// @name Queue Family Ownership
        /// @{
        kege::QueueType src_queue = kege::QueueType::Graphics; ///< Queue family that previously owned the buffer
        kege::QueueType dst_queue = kege::QueueType::Graphics; ///< Queue family that will own the buffer
        /// @}
        ///
        std::string name;
    };
    using BarrierDescriptions = std::vector< kege::RgResrcBarrier >;


    struct SubmitDesc
    {
        kege::PassId pass_id; // which pass this batch belongs to

        uint32_t submit_id;
        std::vector<kege::PassId> pass_ids; // passes recorded into this cmd buffer
        std::vector<kege::PassId> wait_on_pass_ids; // external dependencies
        kege::QueueType queue_type; // Graphics, Compute, Transfer

        //kege::ref::CommandBuffer command_buffer;
        //kege::ref::Semaphore signal_semaphore;
        //std::vector< kege::ref::Semaphore > wait_semaphores;
    };

}


namespace std{

    template<>
    struct hash<kege::RgHandle>
    {
        size_t operator()(const kege::RgHandle& r) const
        {
            return r.id;
        }
    };

}
#endif /* render_graph_common_hpp */
