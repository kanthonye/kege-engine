//
//  render-pass-context.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/15/25.
//

#ifndef render_pass_context_hpp
#define render_pass_context_hpp

#include <unordered_set>
#include "../../utils/array.hpp"
#include "../../graphics/core/graphics-device.hpp"
#include "../../graphics/core/graphics.hpp"

namespace kege{

    struct RenderPass;
    struct RenderGraph;
    struct RenderPassContext;

    enum struct RgResrcType{ Invalid, Buffer, Image, Sampler, BufferView, ShaderResource };

    struct RgResrcHandle
    {
        //inline operator int32_t()const{ return index; }
        inline operator size_t()const{ return index; }
        inline operator int32_t()const{ return index; }
        inline operator bool()const{ return index >= 0; }
        RgResrcType type = RgResrcType::Invalid;
        int32_t index = -1;
    };

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
        RgResrcHandle handle;
        kege::SamplerHandle physical_handle;
    };

    struct ImageDefn
    {
        std::string name;
        uint32_t frames_in_flight;
        ImageUsageFlags usages = ImageUsageFlags::None;
        
        struct Info
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            kege::Format format;
            kege::ImageType type;
        }
        info;

        RgResrcHandle handle = {};
        std::vector< kege::ImageHandle > physical_handle;
    };




    /**
     * @brief Information for updating buffer-type descriptors
     */
    struct RgBufferInfo
    {
        RgResrcHandle buffer {};
        uint64_t offset = 0;
        uint64_t range = 0;
    };

    /**
     * @brief Information for updating image-type descriptors
     */
    struct RgImageInfo
    {
        RgResrcHandle image {};
        RgResrcHandle sampler {};
        ImageLayout layout = ImageLayout::ShaderReadOnly;
    };

    struct RgSamplerInfo
    {
        RgResrcHandle sampler {};
    };

    struct RgBufferViewInfo
    {
        RgResrcHandle view {};
    };
    
    struct RgFetchResInfo
    {
        std::string name;
        RgResrcType type;
    };

    struct RgShaderResource
    {
        enum Type { INVALID, BUFFER, IMAGE, SAMPLER, BUFFER_VIEW, FETCH_RESOURCE } type;

        RgShaderResource& operator =( const RgShaderResource& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: buffers = info.buffers; break;
                case IMAGE: images = info.images; break;
                case SAMPLER: samplers = info.samplers; break;
                case BUFFER_VIEW: views = info.views; break;
                case FETCH_RESOURCE: fetchs = info.fetchs; break;
                case INVALID: break;
            }
            return *this;
        }

        RgShaderResource( const std::vector< kege::RgFetchResInfo >& info )
        :   fetchs( info )
        ,   type( FETCH_RESOURCE )
        {}

        RgShaderResource( const std::vector< kege::RgBufferViewInfo >& info )
        :   views( info )
        ,   type( BUFFER_VIEW )
        {}

        RgShaderResource( const std::vector< kege::RgBufferInfo >& info )
        :   buffers( info )
        ,   type( BUFFER )
        {}

        RgShaderResource( const std::vector< kege::RgImageInfo >& info )
        :   images( info )
        ,   type( IMAGE )
        {}

        RgShaderResource( const std::vector< kege::RgSamplerInfo >& info )
        :   samplers( info )
        ,   type( SAMPLER )
        {}

        RgShaderResource( const RgShaderResource& info )
        :   type( info.type )
        {
            this->operator =( info );
        }

        RgShaderResource()
        :   type( INVALID )
        {
        }

        ~RgShaderResource()
        {
            switch (type)
            {
                case BUFFER: buffers.~vector(); break;
                case IMAGE: images.~vector(); break;
                case SAMPLER: samplers.~vector(); break;
                case BUFFER_VIEW: views.~vector(); break;
                case FETCH_RESOURCE: fetchs.~vector(); break;
                case INVALID: break;
            }
        }

        union
        {
            std::vector< kege::RgImageInfo >      images;
            std::vector< kege::RgBufferInfo >     buffers;
            std::vector< kege::RgBufferViewInfo > views;
            std::vector< kege::RgSamplerInfo >    samplers;
            std::vector< kege::RgFetchResInfo >   fetchs;
        };

    };

    struct RgShaderResourceBinding
    {
        std::string name;
        uint32_t binding;
        DescriptorType descriptor_type;
        RgShaderResource resource;
    };

    struct RgShaderResourceBindingSet
    {
        kege::array< kege::RgShaderResourceBinding > bindings;
        kege::DescriptorSetHandle descriptor_sets;
    };

    struct RgShaderResourceDefn
    {
        kege::array< kege::RgShaderResourceBindingSet > resource_sets;
        RgResrcHandle resouce_handle;
    };



    /**
     * @brief Describes a single binding in a descriptor set layout.
     */
    struct RgShaderResourceSetBinding
    {
        /**
         * @brief The descriptor set layout binding name in the shader.
         */
        std::string name = "";

        /**
         * @brief Binding number matching shader declaration.
         *
         * Example: `layout(binding = 0) uniform UniformBufferObject`
         */
        uint32_t binding = 0;

        /**
         * @brief Number of array elements (for array bindings).
         *
         * Use 1 for non-array resources.
         */
        uint32_t count = 1;

        /**
         * @brief Type of resource being bound.
         */
        DescriptorType descriptor_type = DescriptorType::UniformBuffer;

        /**
         * @brief Shader stages that can access this binding.
         */
        ShaderStage stage_flags = ShaderStage::All;

        /**
         * @brief Optional immutable samplers (for sampler/image bindings).
         *
         * When non-empty, overrides any sampler provided in descriptor set.
         */
        std::vector< SamplerHandle > immutable_samplers;
    };



    struct RgResrcBarrierInfo
    {
        kege::ImageHandle  image_handle  = {};
        kege::BufferHandle buffer_handle = {};

        PipelineStageFlag src_stage_mask = PipelineStageFlag::None;
        PipelineStageFlag dst_stage_mask = PipelineStageFlag::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
        ImageLayout layout;
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
    };

    using RenderPassExecuteCallback = std::function< void( RenderPassContext* ) >;




    struct RgReadResrcDesc
    {
        std::string name;
        RgResrcType type;
        AccessFlags access = AccessFlags::None; // HOW is it used?
        PipelineStageFlag stage = PipelineStageFlag::None; // WHEN is it used?
        RgResrcHandle handle;
    };

    struct RgWriteResrcDesc
    {
        std::string name;
        RgResrcType type;
        AccessFlags access = AccessFlags::None; // HOW is it used?
        PipelineStageFlag stage = PipelineStageFlag::None; // WHEN is it used?
        ClearValue clear_value;
        RgResrcHandle handle;
    };

    struct RgResrcUsage
    {
        int pass_index = -1;
        AccessFlags access = AccessFlags::None;
        ImageLayout layout = ImageLayout::Undefined;
        PipelineStageFlag stage = PipelineStageFlag::TopOfPipe;
    };

    struct RenderPassDefn
    {
        std::string name;
        std::vector< RgReadResrcDesc > reads;
        std::vector< RgWriteResrcDesc > writes;
        RenderPassExecuteCallback execute;
    };

    struct BarrierDescription
    {
        std::vector< RgResrcBarrierInfo > resource_barriers;
    };
    
    class RenderPassContext
    {
    public:

        kege::DescriptorSetHandle getPhysicalDescriptorSet( const RgResrcHandle& handle );
        kege::DescriptorSetHandle getPhysicalDescriptorSet( const std::string& name );


        const std::vector< kege::SamplerHandle >* getSampler( const std::string& name )const;
        const std::vector< kege::BufferHandle >* getBuffers( const std::string& name )const;
        const std::vector< kege::ImageHandle >* getImages( const std::string& name )const;

        kege::BufferHandle getBuffer( const std::string& name );
        kege::ImageHandle getImage( const std::string& name );

        CommandEncoder* getCommandEncoder();
        CommandBuffer* getCommandBuffer();
        Rect2D getRenderArea()const;

        kege::Graphics* getGraphics();

        const std::string& name();

    private:

        std::unordered_map< std::string, RgResrcHandle > _buffer_defn_map;
        std::unordered_map< std::string, RgResrcHandle > _image_defn_map;
        CommandBuffer* _command_buffer;
        kege::Graphics* _graphics;
        RenderPass* _pass;
        Rect2D _render_area;

        std::string _name;

        friend RenderGraph;
        friend RenderPass;
    };

}
#endif /* render_pass_context_hpp */
