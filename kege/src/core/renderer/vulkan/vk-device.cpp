//
//  vulkan-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "vk-device.hpp"
#include "vk-instance.hpp"
#include "vk-swapchain.hpp"
#include "vk-command-buffer.hpp"

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdocumentation"
//#pragma clang diagnostic ignored "-Wnullability-completeness"
//// Define VMA implementation in exactly one cpp file
//#define VMA_IMPLEMENTATION
//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)
//#pragma clang diagnostic pop

namespace kege::vk{

    bool Device::submit( const std::vector< kege::SubmitInfo >& submit_infos, kege::Swapchain* swapchain )
    {
        return _queue_manager->submit( submit_infos, swapchain, _frame_index );
    }

    bool  Device::submit( const kege::SubmitInfo& submit_info )
    {
        return _queue_manager->submit( submit_info );
    }

    bool Device::present( kege::Swapchain* swapchain )
    {
        if( _queue_manager->present( swapchain->vk() ) )
        {
            _frame_index = (_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
            return true;
        }
        return false;
    }

    int Device::getFrameIndex()const
    {
        return _frame_index;
    }

    bool Device::beginSubmit()
    {
        return _queue_manager->beginSubmit( _frame_index );
    }

    void Device::endSubmit()
    {
        _queue_manager->endSubmit();
    }

    kege::Swapchain* Device::createSwapchain( const kege::SwapchainDesc& desc )
    {
        VkResult result;
        vk::Swapchain* swapchain = new vk::Swapchain( this );
        if (( result = swapchain->create( desc ) ) != VK_SUCCESS )
        {
            delete swapchain;
            return nullptr;
        }
        _swapchains.insert( swapchain );
        return swapchain;
    }

    void Device::destroySwapchain( kege::Swapchain* swapchain )
    {
        if ( swapchain != nullptr )
        {
            _swapchains.remove( swapchain->vk() );
            swapchain->vk()->destroy();
            delete swapchain;
        }
    }

    kege::CommandBuffer* Device::createCommandBuffer( kege::QueueType type )
    {
        vk::CommandBuffer* command_buffer = new vk::CommandBuffer;
        _command_buffers.insert( command_buffer );

        command_buffer->_command_pool = _queue_manager->_command_pools[ type ];
        command_buffer->_device = this;
        command_buffer->_is_recording = false;
        command_buffer->_queue_type = type;

        VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = _queue_manager->_command_pools[ type ];
        info.commandBufferCount = 1;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if ( vkAllocateCommandBuffers( _device, &info, &command_buffer->_handle ) != VK_SUCCESS )
        {
            // error
            return nullptr;
        }

        return command_buffer;
    }

    void Device::destroyCommandBuffer( kege::CommandBuffer* cmb )
    {
        if( cmb )
        {
            _command_buffers.remove( cmb->vk() );
            if ( cmb->vk()->handle() != VK_NULL_HANDLE )
            {
                waitIdle();
                vkFreeCommandBuffers( _device, cmb->vk()->_command_pool, 1, &cmb->vk()->handle() );
            }
            delete cmb;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Shader Resource Set Lifecycle
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    int  Device::makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )
    {
        return _pipeline_layout_manager.makeSet( descriptors, resources );
    }

    bool Device::updateSet( int handle, const UniformResourceSet& resources )
    {
        return _pipeline_layout_manager.updateSet( handle, resources );
    }

    int  Device::allocateSet( const UniformDescriptorSet& descriptors )
    {
        return _pipeline_layout_manager.allocateSet( descriptors );
    }

    void Device::freeSet( int set )
    {
        return _pipeline_layout_manager.freeSet( set );
    }

    const vk::DescriptorSet* Device::getSet( int32_t descriptor_id ) const
    {
        return _pipeline_layout_manager.getSet( descriptor_id );
    }
    
    //-------------------------------------------------------------------------
    // Descriptor Set Layout Lifecycle
    //-------------------------------------------------------------------------

    UniformSetLayout Device::getUniformSetLayout( const UniformDescriptors& desc )
    {
        return { _pipeline_layout_manager.getDescriptorSetLayoutID( desc, false ) };
    }

    UniformSetLayout Device::createUniformSetLayout( const UniformDescriptors& desc )
    {
        return { _pipeline_layout_manager.createUniformSetLayout( desc ) };
    }

    void Device::destroyUniformSetLayout( const UniformSetLayout& layout )
    {
        _pipeline_layout_manager.destroyDescriptorSetLayout( layout.id );
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Image / Texture Resources
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //


    kege::ImageHandle Device::createImage( const kege::ImageDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        kege::ImageHandle handle = { _textures.gen() };

        vk::Image* texr = _textures.get( handle.id );
        texr->current_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        texr->aspect = getImageAspectFlags( desc.format );
        texr->desc = desc;

        /** ---- Create Image Handle ---- */

        VkImageCreateFlags flags = ((desc.type == ImageType::TypeCube || desc.type == ImageType::TypeCubeArray))
        ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
        : 0;

        texr->format = convertFormat( desc.format );

        texr->image = _manager.createImage
        (
            convertTextureType( desc.type ),
            texr->format,
            desc.width,
            desc.height,
            desc.depth,
            desc.layers,
            desc.mip_levels,
            convertImageUsage(desc.usage),
            convertSampleCount(desc.sample_count),
            flags
        );

        /** ---- Create Image Memory ---- */

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements( _device, texr->image, &memory_requirements );

        _manager.allocateDeviceMemory( _physical_device->getMemoryProperties(), memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texr->memory );
        if( vkBindImageMemory( _device, texr->image, texr->memory, 0 ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Failed to create image memory in createImage."<<Log::nl;
            destroyImage( handle );
            return {-1};
        }

        /** ---- Create Image View ---- */

        texr->view = _manager.createImageView
        (
            texr->image,
            convertTextureViewType( desc.type ),
            texr->format,
            desc.layers,
            desc.mip_levels,
            texr->aspect
        );
        if ( texr->view == VK_NULL_HANDLE )
        {
            KEGE_LOG_ERROR << "Failed to create image view in createImage."<<Log::nl;
            destroyImage( handle );
            return {-1};
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && !desc.name.empty() )
        {
            _manager.debugSetObjectName( (uint64_t)texr->image, VK_OBJECT_TYPE_IMAGE, desc.name.c_str() );
        }

        if ( desc.data != nullptr )
        {
            /**
             * create a seperate shared buffer between the CPU & GPU that contains the data.
             */
            Buffer source = {};
            size_t size = desc.width * desc.height * desc.depth * sizeOfFormat( desc.format );
            VkResult result = createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                size, desc.data, &source
            );
            if ( result != VK_SUCCESS )
            {
                return {};
            }
            setBufferData( size, desc.data, &source );

            VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
            _manager.copyBufferToTexture
            (
                command_buffer,
                source.buffer,
                texr->image,
                desc.width, desc.height,
                texr->aspect,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
            _queue_manager->endTransferQueueCommandBuffer( command_buffer );
        }

        return handle;
    }

    void Device::destroyImage( ImageHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id < 0 ) return;

        if ( _textures.get( handle.id ) != nullptr )
        {
            waitIdle();
            Image* texture = _textures.get( handle.id );

            if ( texture->memory != VK_NULL_HANDLE )
            {
                vkFreeMemory( _device, texture->memory, nullptr );
                vkDestroyImage( _device, texture->image, nullptr );
                texture->memory = VK_NULL_HANDLE;
                texture->image = VK_NULL_HANDLE;
            }

            if ( texture->view != VK_NULL_HANDLE )
            {
                vkDestroyImageView( _device, texture->view, nullptr );
                texture->view = VK_NULL_HANDLE;
            }
            
            _textures.free( handle.id );
        }
        else
        {
            KEGE_LOG_ERROR << "Trying to destroy invalid ImageHandle: " << handle.id<<Log::nl;
        }
    }

    VkResult Device::createBuffer( VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size, const void* data, vk::Buffer* buffer )
    {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.usage = usage;
        create_info.size = size;
        
        VkResult result = vkCreateBuffer( _device, &create_info, nullptr, &buffer->buffer );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result );
            return result;
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements( _device, buffer->buffer, &memory_requirements );
        result = _manager.allocateDeviceMemory( _physical_device->getMemoryProperties(), memory_requirements, memory_properties, &buffer->memory );
        if( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Could not allocate memory for a buffer in createBuffer()"<<Log::nl;
            return result;
        }
        
        vkBindBufferMemory( _device, buffer->buffer, buffer->memory, 0 );
        buffer->memory_properties = memory_properties;
        buffer->usage = usage;
        buffer->size = size;
        return result;
    }

    void Device::setBufferData( VkDeviceSize size, const void* data, vk::Buffer* buffer )
    {
        if ( data != nullptr && size != 0 )
        {
            vkMapMemory( _device, buffer->memory, 0, size, 0, &buffer->mapped_ptr );
            memcpy( buffer->mapped_ptr, data, size );
            vkUnmapMemory( _device, buffer->memory );
            buffer->mapped_ptr = nullptr;
        }
    }

    kege::BufferHandle Device::createBuffer(const kege::BufferDesc& desc)
    {
        int32_t id = _buffers.gen();
        Buffer* buffer = _buffers.get( id );

        VkResult result = createBuffer
        (
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | convertBufferUsageFlags( desc.usage ),
            convertMemoryPropertyFlags( desc.memory_usage ),
            desc.size, desc.data,
            buffer
        );
        
        if ( result != VK_SUCCESS )
        {
            _buffers.free( id );
            return {};
        }

        /**
         * For debug purposes, assign a name to buffer handle if a name id present.
         */
        if ( _instance->isValidationEnabled() && desc.name )
        {
            _manager.debugSetObjectName( (uint64_t)buffer->buffer, VK_OBJECT_TYPE_BUFFER, desc.name );
        }

        /**
         * For buffer that are shared between the CPU & GPU. The data can be copied directly
         * to the buffer.
         */
        if ( desc.memory_usage == MemoryUsage::CpuToGpu )
        {
            setBufferData( desc.size, desc.data, buffer );
        }

        /**
         * For buffer that only exist on the GPU. You can not copy data to them directly.
         * Thus you must create a seperate shared buffer between the CPU & GPU that contains
         * the data, then use a command buffer to transfer the data to the designated buffer.
         */
        else if ( desc.memory_usage == MemoryUsage::GpuOnly )
        {
            /**
             * create a seperate shared buffer between the CPU & GPU that contains the data.
             */
            Buffer source = {};
            VkResult result = createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                desc.size, desc.data, &source
            );
            if ( result != VK_SUCCESS )
            {
                _buffers.free( id );
                return {};
            }
            setBufferData( desc.size, desc.data, &source );

            /**
             * transfer the data to the designated buffer.
             */
            VkBufferCopy copy_region{};
            copy_region.srcOffset = 0; // Optional
            copy_region.dstOffset = 0; // Optional
            copy_region.size = desc.size;
            VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
            vkCmdCopyBuffer( command_buffer, source.buffer, buffer->buffer, 1, &copy_region );
            _queue_manager->endTransferQueueCommandBuffer( command_buffer );
        }
        return { id };
    }

    bool Device::resizeBuffer( const BufferHandle& handle, uint64_t size )
    {
        vk::Buffer* buffer = _buffers.get( handle.id );
        vk::Buffer new_buffer;

        VkResult result = createBuffer( buffer->usage, buffer->memory_properties, size, nullptr, &new_buffer );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        /**
         * transfer the data to the designated buffer.
         */
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = (buffer->size < size ) ? buffer->size : size;
        VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
        vkCmdCopyBuffer( command_buffer, new_buffer.buffer, buffer->buffer, 1, &copy_region );
        _queue_manager->endTransferQueueCommandBuffer( command_buffer );

        destroyBuffer( buffer );

        buffer->size = new_buffer.size;
        buffer->buffer = new_buffer.buffer;
        buffer->memory = new_buffer.memory;
        buffer->usage = new_buffer.usage;
        buffer->memory_properties = new_buffer.memory_properties;

        return true;
    }

    void Device::updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data )
    {
        vk::Buffer* buffer = _buffers.get( handle.id );
        int mask = (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        if ( (buffer->memory_properties & mask) == mask )
        {
            vkMapMemory( _device, buffer->memory, offset, size, 0, &buffer->mapped_ptr );
            memcpy( buffer->mapped_ptr, data, size );
            vkUnmapMemory( _device, buffer->memory );
            buffer->mapped_ptr = nullptr;
        }
        else
        {
            KEGE_LOG_ERROR << "can not map buffer, MemoryUsage must be set to CpuToGpu.";
        }
    }

    void Device::destroyBuffer( vk::Buffer* buffer )
    {
        if ( buffer != nullptr )
        {
            waitIdle();
            vkFreeMemory( _device, buffer->memory, nullptr );
            vkDestroyBuffer( _device, buffer->buffer, nullptr );
            buffer->buffer = VK_NULL_HANDLE;
            buffer->memory = VK_NULL_HANDLE;
        }
    }
    void Device::destroyBuffer( kege::BufferHandle handle )
    {
        destroyBuffer( _buffers.get( handle.id ) );
    }

    void Device::destroyBufferView(kege::BufferViewHandle handle)
    {
    }

    kege::SamplerHandle Device::createSampler(const kege::SamplerDesc& desc)
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = convertFilter( desc.mag_filter );
        samplerInfo.minFilter = convertFilter( desc.min_filter );
        samplerInfo.mipmapMode = convertMipmapMode(desc.mipmap_mode);
        samplerInfo.addressModeU = convertAddressMode( desc.address_mode_u );
        samplerInfo.addressModeV = convertAddressMode( desc.address_mode_v );
        samplerInfo.addressModeW = convertAddressMode( desc.address_mode_w );
        samplerInfo.mipLodBias = desc.mip_lod_bias;
        samplerInfo.anisotropyEnable = desc.anisotropy_enable ? VK_TRUE : VK_FALSE;

        samplerInfo.maxAnisotropy = ( desc.anisotropy_enable )
        ? std::min(desc.max_anisotropy, _limits.max_sampler_anisotropy)
        : 1.0f; // Clamp to device limits

        samplerInfo.compareEnable = desc.compare_enable ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = convertCompareOp(desc.compare_op);
        samplerInfo.minLod = desc.min_lod;
        samplerInfo.maxLod = desc.max_lod;
        samplerInfo.borderColor = convertBorderColor(desc.border_color);
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // Default

        Sampler sampler;
        sampler.desc = desc;
        if (vkCreateSampler(_device, &samplerInfo, nullptr, &sampler.sampler) != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to create sampler!";
            return {-1};
        }

        kege::SamplerHandle handle = { _samplers.gen() };
        *_samplers.get( handle.id ) = sampler;

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && desc.name )
        {
            _manager.debugSetObjectName( (uint64_t)sampler.sampler, VK_OBJECT_TYPE_SAMPLER, desc.name );
        }

        return handle;
    }

    void Device::destroySampler(kege::SamplerHandle handle)
    {
         if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

         if ( _samplers.get( handle.id ) != nullptr )
         {
             waitIdle();
             vkDestroySampler( _device, _samplers.get( handle.id )->sampler, nullptr );
             _samplers.free( handle.id );
         }
    }

    kege::ShaderHandle Device::createShader( const kege::ShaderDesc& desc )
    {
         if ( _device == VK_NULL_HANDLE || desc.byte_code.empty() ) return {-1};

        kege::ShaderHandle handle = { _shaders.gen() };
        vk::Shader* shader = _shaders.get( handle.id );

        shader->desc = desc; // Store original desc if needed later

        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = desc.byte_code.size() * sizeof( desc.byte_code[0] );
        create_info.pCode = desc.byte_code.data();
        VkResult result;
        if ((result = vkCreateShaderModule( _device, &create_info, nullptr, &shader->shader_module ) ) != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "Failed to create shader module!" <<Log::nl;
            return {-1};
        }

        // Set Debug Name (requires VK_EXT_debug_utils)
        if ( _instance->isValidationEnabled() && !desc.name.empty() )
        {
            _manager.debugSetObjectName( (uint64_t)shader->shader_module, VK_OBJECT_TYPE_SHADER_MODULE, shader->desc.name.data() );
        }

        return handle;
    }

    void Device::destroyShader( kege::ShaderHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _shaders.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyShaderModule(_device, _shaders.get( handle.id )->shader_module, nullptr);
            _shaders.free( handle.id );
        }
    }

    kege::PipelineLayoutHandle Device::createPipelineLayout( const kege::PipelineLayoutDesc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {-1};
        return {};// { _pipeline_layout_manager.createPipelineLayout( desc.name.data(), desc.descriptor_set_layouts, desc.push_constant_ranges ) };
    }

    const vk::PipelineLayout* Device::getPipelineLayout( int32_t pipeline_layout_id ) const
    {
        return _pipeline_layout_manager.getPipelineLayout( pipeline_layout_id );
    }

    void Device::destroyPipelineLayout( kege::PipelineLayoutHandle handle )
    {
        return _pipeline_layout_manager.destroyPipelineLayout( handle.id );
    }

    /**
     * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
     * @brief Create a graphics pipeline
     *
     * Creates a VkPipeline for graphics rendering based on the provided description.
     * Uses pipeline caching for improved performance when creating similar pipelines.
     *
     * @param desc Description of the graphics pipeline to create
     *  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
     */

    std::vector< PipelineHandle > Device::createGraphicsPipeline( const CreateShaderPipelineInfo& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {};
        std::vector< PipelineHandle > pipelines;

        for (int i=0; i<desc.pipelines.size(); ++i)
        {
            const kege::PipelineInfo& info = desc.pipelines[i];

            int pipeline_layout_handle = _pipeline_layout_manager.createPipelineLayout
            ( info.name.data(), info.layouts, info.push_constants );

            if ( pipeline_layout_handle < 0 )
            {
                KEGE_LOG_ERROR << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
                return {};
            }

            // --- Translate Desc to Vulkan Structures ---
            VkGraphicsPipelineCreateInfo graphics_create = {};
            graphics_create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 1. Shader Stages
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
            for (int k=0; k<info.stages.size(); ++k)
            {
                Shader* shader = _shaders.get( desc.stages[ info.stages[k] ].id );

                shader_stages.push_back
                ({
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
                    convertShaderStage( shader->desc.stage ),
                    shader->shader_module,
                    shader->desc.entry_point.data()
                });
            }
            // Add other stages (Geometry, Tessellation) if present in desc...
            graphics_create.stageCount = static_cast< uint32_t >( shader_stages.size() );
            graphics_create.pStages = shader_stages.data();

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 2. Vertex Input State
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            std::vector< VkVertexInputBindingDescription > vertex_bindings;
            std::vector< VkVertexInputAttributeDescription > vertex_attributes;
            for (int i=0; i<info.vertex_input.attributes.size(); ++i)
            {
                vertex_attributes.push_back
                ({
                    .location = info.vertex_input.attributes[i].location,
                    .offset   = info.vertex_input.attributes[i].offset,
                    .format   = convertFormat( info.vertex_input.attributes[i].format ),
                    .binding  = info.vertex_input.attributes[i].binding
                });
            }
            for (int i=0; i<info.vertex_input.bindings.size(); ++i)
            {
                vertex_bindings.push_back
                ({
                    .stride    = info.vertex_input.bindings[i].stride,
                    .inputRate = convertVertexInputRate( info.vertex_input.bindings[i].input_rate ),
                    .binding   = info.vertex_input.bindings[i].binding
                });
            }
            VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size());
            vertexInputInfo.pVertexBindingDescriptions = vertex_bindings.empty() ? nullptr : vertex_bindings.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attributes.size());
            vertexInputInfo.pVertexAttributeDescriptions = vertex_attributes.empty() ? nullptr : vertex_attributes.data();
            graphics_create.pVertexInputState = &vertexInputInfo;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 3. Input Assembly
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
            input_assembly_info.primitiveRestartEnable = info.states.input_assembly.primitive_restart_enable;
            input_assembly_info.topology = convertPrimitiveTopology( info.states.input_assembly.topology );
            graphics_create.pInputAssemblyState = &input_assembly_info;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 4. Viewport State (can be dynamic)
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // Assuming non-dynamic for now
            VkPipelineViewportStateCreateInfo viewport_state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
            viewport_state.viewportCount = 1;
            viewport_state.pViewports = nullptr; // Set via dynamic state or provide dummy
            viewport_state.scissorCount = 1;
            viewport_state.pScissors = nullptr; // Set via dynamic state or provide dummy
            graphics_create.pViewportState = &viewport_state;

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // 5. Rasterization State
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            VkPipelineRasterizationStateCreateInfo rasterization_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
            rasterization_info.lineWidth = info.states.rasterization.line_width;
            rasterization_info.rasterizerDiscardEnable = info.states.rasterization.rasterizer_disable;
            rasterization_info.depthClampEnable = info.states.rasterization.depth_clamp_enable;
            rasterization_info.depthBiasEnable = info.states.rasterization.depth_bias_enable;
            rasterization_info.depthBiasClamp = info.states.rasterization.depth_bias_clamp;
            rasterization_info.cullMode = VK_CULL_MODE_NONE;///convertCullMode( desc.rasterization_state.cull_mode );
            rasterization_info.frontFace = convertFrontFace( info.states.rasterization.front_face );
            rasterization_info.depthBiasConstantFactor = info.states.rasterization.depth_bias_constant_factor;
            rasterization_info.depthBiasSlopeFactor = info.states.rasterization.depth_bias_slope_factor;
            rasterization_info.polygonMode = convertPolygonMode( info.states.rasterization.polygon_mode );
            graphics_create.pRasterizationState = &rasterization_info;

            // 6. Multisample State
            VkPipelineMultisampleStateCreateInfo multisample_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
            multisample_info.sampleShadingEnable = info.states.multisample.sample_shading_enable;
            multisample_info.rasterizationSamples = convertSampleCount( info.states.multisample.rasterization_samples );// VK_SAMPLE_COUNT_1_BIT;
            multisample_info.minSampleShading = info.states.multisample.min_sample_shading;
            multisample_info.pSampleMask = 0;
            multisample_info.alphaToCoverageEnable = info.states.multisample.alpha_to_coverage_enable;
            multisample_info.alphaToOneEnable = info.states.multisample.alpha_to_one_enable;
            multisample_info.flags = 0;
            graphics_create.pMultisampleState = &multisample_info;

            // 7. Depth Stencil State
            VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
            depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depth_stencil_info.depthTestEnable       = info.states.depth_stencil.depth_test_enable;
            depth_stencil_info.depthWriteEnable      = info.states.depth_stencil.depth_write_enable;
            depth_stencil_info.depthCompareOp        = convertCompareOp( info.states.depth_stencil.depth_compare_op );

            depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
            depth_stencil_info.minDepthBounds       = 0.0;
            depth_stencil_info.maxDepthBounds       = 1.0;

            if ( info.states.depth_stencil.stencil_test_enable )
            {
                depth_stencil_info.stencilTestEnable    = VK_TRUE;
                depth_stencil_info.front.compareMask    = info.states.depth_stencil.front_op.compare_mask;
                depth_stencil_info.front.compareOp      = convertCompareOp( info.states.depth_stencil.front_op.compare_op );
                depth_stencil_info.front.depthFailOp    = convertStencilOp( info.states.depth_stencil.front_op.depth_fail_op );
                depth_stencil_info.front.failOp         = convertStencilOp( info.states.depth_stencil.front_op.fail_op );
                depth_stencil_info.front.passOp         = convertStencilOp( info.states.depth_stencil.front_op.pass_op );
                depth_stencil_info.front.reference      = info.states.depth_stencil.front_op.reference;
                depth_stencil_info.front.writeMask      = info.states.depth_stencil.front_op.write_mask;
                depth_stencil_info.back.depthFailOp     = convertStencilOp( info.states.depth_stencil.back_op.depth_fail_op );
                depth_stencil_info.back.compareOp       = convertCompareOp( info.states.depth_stencil.back_op.compare_op );
                depth_stencil_info.back.failOp          = convertStencilOp( info.states.depth_stencil.back_op.fail_op );
                depth_stencil_info.back.passOp          = convertStencilOp( info.states.depth_stencil.back_op.pass_op );
                depth_stencil_info.back.reference       = info.states.depth_stencil.back_op.reference;
                depth_stencil_info.back.writeMask       = info.states.depth_stencil.back_op.write_mask;
            }
            else
            {
                depth_stencil_info.stencilTestEnable = VK_FALSE;
                depth_stencil_info.front = {};
                depth_stencil_info.back = {};
            }
            graphics_create.pDepthStencilState      = &depth_stencil_info;

            // 8. Color Blend State
            std::vector< VkPipelineColorBlendAttachmentState > color_blend_attachment_states;
            for ( int i=0; i<info.states.color_blend.attachments.size(); i++ )
            {
                VkPipelineColorBlendAttachmentState attachment = {};
                attachment.blendEnable         = (info.states.color_blend.attachments[i].blend_enable)? VK_TRUE : VK_FALSE;

                attachment.colorBlendOp        = convertBlendOp( info.states.color_blend.attachments[i].color_blend_op );
                attachment.srcColorBlendFactor = convertBlendFactor( info. states.color_blend.attachments[i].src_color_blend_factor );
                attachment.dstColorBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].dst_color_blend_factor );

                attachment.alphaBlendOp        = convertBlendOp( info.states.color_blend.attachments[i].alpha_blend_op );
                attachment.dstAlphaBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].dst_alpha_blend_factor );
                attachment.srcAlphaBlendFactor = convertBlendFactor( info.states.color_blend.attachments[i].src_alpha_blend_factor );

                attachment.colorWriteMask      = convertColorComponentMask( info.states.color_blend.attachments[i].color_write_mask );
                color_blend_attachment_states.push_back( attachment );
            }
            VkPipelineColorBlendStateCreateInfo color_blend_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
            color_blend_info.attachmentCount = static_cast< int >( color_blend_attachment_states.size() );
            color_blend_info.pAttachments = color_blend_attachment_states.data();
            color_blend_info.logicOpEnable = (info.states.color_blend.logic_op_enable) ? VK_TRUE : VK_FALSE;
            color_blend_info.logicOp = vk::convertLogicOp( info.states.color_blend.logic_op );
            graphics_create.pColorBlendState = &color_blend_info;

            // 9. Dynamic State (Optional but common for viewport/scissor)
            std::vector< VkDynamicState > dynamic_states = std::vector< VkDynamicState >
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
                //VK_DYNAMIC_STATE_LINE_WIDTH
            };
            VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
            dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_state_create_info.dynamicStateCount = static_cast< uint32_t >( dynamic_states.size() );
            dynamic_state_create_info.pDynamicStates = dynamic_states.data();
            graphics_create.pDynamicState = &dynamic_state_create_info;

            // 10. Pipeline Layout
            graphics_create.layout = _pipeline_layout_manager.getPipelineLayout( pipeline_layout_handle )->layout;

            // 11. Render Pass / Rendering Info (Crucial!)
            // We'll use Dynamic Rendering (VK_KHR_dynamic_rendering) as it's simpler for a render graph
            // Check if dynamic rendering is supported first!
            std::vector< VkFormat > color_attachmen_formats;
            for(Format fmt : info.outputs.color_attachment_formats )
            {
                VkFormat vk_fmt = convertFormat(fmt);
                if (vk_fmt == VK_FORMAT_UNDEFINED)
                {
                    KEGE_LOG_ERROR << "Invalid format passed for color attachment!"<<Log::nl;
                    _pipeline_layout_manager.destroyPipelineLayout( pipeline_layout_handle );
                    for (int k=0; k<info.stages.size(); ++k)
                    {
                        destroyShader( desc.stages[ info.stages[k] ] );
                    }
                    return {};
                }
                color_attachmen_formats.push_back( vk_fmt );
            }
            VkPipelineRenderingCreateInfoKHR rendering_info{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
            rendering_info.stencilAttachmentFormat = convertFormat(info.outputs.stencil_attachment_format);
            rendering_info.depthAttachmentFormat = convertFormat(info.outputs.depth_attachment_format);
            rendering_info.colorAttachmentCount = static_cast< uint32_t >( color_attachmen_formats.size() );
            rendering_info.pColorAttachmentFormats = ( !color_attachmen_formats.empty() ) ? color_attachmen_formats.data() : nullptr;
            // Chain this to pipelineInfo.pNext
            graphics_create.pNext = &rendering_info;
            graphics_create.renderPass = VK_NULL_HANDLE; // Must be NULL for dynamic rendering

            // 12. Base Pipeline (for derivatives, ignore for now)
            graphics_create.basePipelineHandle = VK_NULL_HANDLE;
            graphics_create.basePipelineIndex = -1;

            // --- Create Pipeline ---
            VkPipeline handle;
            VkResult result = vkCreateGraphicsPipelines( _device, VK_NULL_HANDLE, 1, &graphics_create, nullptr, &handle );
            if ( result != VK_SUCCESS )
            {
                kege::Log::error <<vkResultToString( result ) <<Log::nl;
                _pipeline_layout_manager.destroyPipelineLayout( pipeline_layout_handle );
                for (int k=0; k<info.stages.size(); ++k)
                {
                    destroyShader( desc.stages[ info.stages[k] ] );
                }
                return {};
            }

            // Add to runtime map and cache
            int id = _graphics_pipelines.gen();
            {
                vk::GraphicsPipeline* pipeline = _graphics_pipelines.get( id );
                pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
                pipeline->pipeline = handle;
                pipeline->pipeline_layout_id = pipeline_layout_handle;
            }

            if ( _instance->isValidationEnabled() && !info.name.empty() )
            {
                _manager.debugSetObjectName( (uint64_t)handle, VK_OBJECT_TYPE_PIPELINE, info.name.c_str() );
            }
            pipelines.push_back({ id });
        }
        for (int k=0; k<desc.stages.size(); ++k)
        {
            destroyShader({ desc.stages[k] });
        }
        return pipelines;
    }

    void Device::destroyGraphicsPipeline(PipelineHandle handle)
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _graphics_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _graphics_pipelines.get( handle.id )->pipeline, nullptr );
            _graphics_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _graphics_pipelines.free( handle.id );
        }
    }

    kege::PipelineHandle Device::createComputePipeline( const kege::ComputePipelineDesc& desc )
    {
        const vk::PipelineLayout* layout = _pipeline_layout_manager.getPipelineLayout( desc.pipeline_layout.id );
        if ( layout == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid pipeline-layout in createGraphicsPipeline!"<<Log::nl;
            return {-1};
        }

        Shader* shader = _shaders.get( desc.compute_shader.id );

        VkPipelineShaderStageCreateInfo create_shader_info = {};
        create_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_shader_info.stage = convertShaderStage( shader->desc.stage );
        create_shader_info.module = shader->shader_module;
        create_shader_info.pName = shader->desc.entry_point.data();

        VkComputePipelineCreateInfo create_pipeline_info{};
        create_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_pipeline_info.layout = layout->layout;
        create_pipeline_info.stage = create_shader_info;

        VkPipeline pipeline;
        VkResult result = vkCreateComputePipelines
        (
            _device,
            VK_NULL_HANDLE,
            1,
            &create_pipeline_info,
            nullptr,
            &pipeline
        );

        if ( result != VK_SUCCESS )
        {
            KEGE_LOG_ERROR << "createComputePipeline" <<Log::nl;
            return {-1};
        }

        int32_t id = _compute_pipelines.gen();
        ComputePipeline* compute = _compute_pipelines.get( id );
        compute->pipeline = pipeline;
        compute->desc = desc;
        return { id };
    }

    void Device::destroyComputePipeline( kege::PipelineHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        if ( _compute_pipelines.get( handle.id ) != nullptr )
        {
            waitIdle();
            vkDestroyPipeline( _device, _compute_pipelines.get( handle.id )->pipeline, nullptr );
            _compute_pipelines.get( handle.id )->pipeline = VK_NULL_HANDLE;
            _compute_pipelines.free( handle.id );
        }
    }
    

    const vk::DescriptorSetLayout* Device::getDescriptorSetLayout( int32_t layout )
    {
        return _pipeline_layout_manager.getDescriptorSetLayout( layout );
    }

    //-------------------------------------------------------------------------
    // Descriptor Set Lifecycle
    //-------------------------------------------------------------------------

    bool Device::updateUniformSets( const std::vector< int >& handles, const UniformSets& resource_sets )
    {
        return 0;//_pipeline_layout_manager.updateUniformSets( handles, resource_sets );
    }

    bool Device::updateUniformSet( int handle, const UniformSet& resource_set )
    {
        return 0;//_pipeline_layout_manager.updateUniformSet( handle, resource_set );
    }

    std::vector< int > Device::allocateUniformSets( const UniformSetsDesc& desc )
    {
        return {};//_pipeline_layout_manager.allocateUniformSets( desc );
    }

    int Device::allocateUniformSet( const UniformSetDesc& desc )
    {
        return 0;//_pipeline_layout_manager.allocateUniformSet( desc );
    }

    // --- Wait Idle ---
    void Device::waitIdle()
    {
        if ( _device == VK_NULL_HANDLE || _device == VK_NULL_HANDLE) return;
        vkDeviceWaitIdle(_device);
    }

    // --- Buffer Mapping ---
    void* Device::mapBuffer( kege::BufferHandle handle, size_t offset, size_t size )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return nullptr;

        Buffer* buffer = _buffers.get( handle.id );
        if ( buffer == nullptr )
        {
            KEGE_LOG_ERROR << "Attempting to map a buffer with incompatible memory usage!" <<Log::nl;
            return nullptr;
        }

        VkMemoryPropertyFlags cpu_to_gpu =
        (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        VkMemoryPropertyFlags gpu_to_cpu =
        (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // MUST be mappable
         VK_MEMORY_PROPERTY_HOST_CACHED_BIT);  // Request cached memory for faster CPU reads

        // Only map if memory allows it and not already mapped
        if
        (
            buffer->mapped_ptr == nullptr &&
            (buffer->memory_properties == cpu_to_gpu || buffer->memory_properties == gpu_to_cpu)
        )
        {
            if ( size == 0 )
            {
                size = buffer->size - offset;
            }

            VkResult result = vkMapMemory( _device, buffer->memory, offset, size, 0, &buffer->mapped_ptr );
            if (result == VK_SUCCESS)
            {
                // Return pointer adjusted by offset
                return static_cast< uint8_t* >( buffer->mapped_ptr ) + offset;
            }
            else
            {
                switch ( result )
                {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        KEGE_LOG_ERROR << "VK_ERROR_OUT_OF_HOST_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        KEGE_LOG_ERROR << "VK_ERROR_OUT_OF_DEVICE_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_MEMORY_MAP_FAILED:
                        KEGE_LOG_ERROR << "VK_ERROR_MEMORY_MAP_FAILED in mapBuffer()"<<Log::nl;
                        break;
                    default:
                        KEGE_LOG_ERROR << "unablde to map device memory in mapBuffer()"<<Log::nl;
                        break;
                }
                return nullptr;
            }
        }
        else if ( buffer->mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( buffer->mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void Device::unmapBuffer( kege::BufferHandle handle )
    {
        if ( _device == VK_NULL_HANDLE || handle.id == 0 ) return;

        Buffer* buffer = _buffers.get( handle.id );
        if ( buffer != nullptr )
        {
             if (buffer->mapped_ptr != nullptr)
             {
                 vkUnmapMemory( _device, buffer->memory );
                 buffer->mapped_ptr = nullptr;
             }
        }
    }

    size_t Device::bufferSize( const BufferHandle& handle )
    {
        return ( _device == VK_NULL_HANDLE || handle.id == 0 )
        ? 0 : _buffers.get( handle.id )->size;
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * @brief Create a semaphore synchronization object
     * Creates a VkSemaphore that can be used to synchronize GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */

    kege::Ref< kege::Semaphore > Device::createSemaphore()
    {
        return _semaphores.insert( new vk::Semaphore( _manager.createSemaphore(), this ) );
    }

    void Device::destroySemaphore( kege::Semaphore* semaphore )
    {
        if ( semaphore != nullptr )
        {
            vk::Semaphore* sem = static_cast< vk::Semaphore* >( semaphore );
            if ( sem->device != nullptr )
            {
                _manager.destroySemaphore( sem->handle );
                sem->device = nullptr;
            }
        }
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * @brief Create a fence synchronization object
     * Creates a VkFence that can be used to synchronize CPU and GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */
    
    kege::Ref< kege::Fence > Device::createFence( bool initially_signaled )
    {
        return _fences.insert( new vk::Fence( _manager.createFence( initially_signaled ), this ) );
    }
    
    void Device::destroyFence( kege::Fence* fence )
    {
        if ( fence != nullptr )
        {
            vk::Fence* f = static_cast< vk::Fence* >( fence );
            if ( f->device != nullptr )
            {
                _manager.destroyFence( f->handle );
                f->device = nullptr;
            }
        }
    }

    const Buffer* Device::getBuffer(BufferHandle handle) const
    {
        return _buffers.get( handle.id );
    }

    Buffer* Device::getBuffer(BufferHandle handle)
    {
        return _buffers.get( handle.id );
    }

    const Image* Device::getImage(ImageHandle handle) const
    {
        return _textures.get( handle.id );
    }

    const Sampler* Device::getSampler(SamplerHandle handle) const
    {
        return _samplers.get( handle.id );
    }

    const Shader* Device::getShader(ShaderHandle handle) const
    {
        return _shaders.get( handle.id );
    }

    const GraphicsPipeline* Device::getGraphicsPipeline(PipelineHandle handle) const
    {
        return _graphics_pipelines.get( handle.id );
    }

    const ComputePipeline* Device::getComputePipeline(PipelineHandle handle) const
    {
        return _compute_pipelines.get( handle.id );
    }

    VkSurfaceKHR Device::surface()
    {
        return _surface;
    }
    
    VkDevice Device::handle()
    {
        return _device;
    }

    bool Device::initialize( vk::PhysicalDevice* physical_device, VkSurfaceKHR surface )
    {
        _surface = surface;
        _physical_device = physical_device;
        _instance = physical_device->getInstance();
        if ( physical_device->getHandle() == VK_NULL_HANDLE )
        {
            KEGE_LOG_ERROR << "Invalid PhysicalDevice passed to createLogicalDevice."<<Log::nl;
            return false;
        }

        _queue_family_indices = findQueueFamilies( physical_device->getHandle(), _surface );
        if ( !_queue_family_indices.graphics_family.has_value() ) // graphics is always required
        {
            KEGE_LOG_ERROR << "Failed to find a suitable graphics queue family on the selected device."<<Log::nl;
             return false;
        }

        _device = _manager.createDevice( physical_device, surface, _queue_family_indices );
        if ( _device == VK_NULL_HANDLE )
        {
            return false;
        }

        _queue_manager = new QueueManager;
        _queue_manager->initialize( this, _queue_family_indices );

        KEGE_LOG_INFO <<"- " << "Vulkan Logical Device created." <<"\n";

        _pipeline_layout_manager.initialize( _instance, this );
        return true;
    }

    void Device::shutdown()
    {
        if ( _device == VK_NULL_HANDLE ) return;

        waitIdle(); // Ensure GPU is idle before destroying anything

        for ( int32_t i = 0; i < _buffers.count(); ++i )
        {
            if ( _buffers.get( i ) != nullptr )
            {
                destroyBuffer({ i });
            };
        }
        for ( int32_t i = 0; i < _textures.count(); ++i )
        {
            if ( _textures.get( i ) != nullptr )
            {
                destroyImage({ i });
            };
        }
        for ( int32_t i = 0; i < _samplers.count(); ++i )
        {
            if ( _samplers.get( i ) != nullptr )
            {
                destroySampler({ i });
            };
        }

        _fences.clear();
        _semaphores.clear();

        for ( int32_t i = 0; i < _shaders.count(); ++i )
        {
            if ( _shaders.get( i ) != nullptr )
            {
                destroyShader({ i });
            };
        }

        for ( int32_t i = 0; i < _graphics_pipelines.count(); ++i )
        {
            if ( _graphics_pipelines.get( i ) != nullptr )
            {
                destroyGraphicsPipeline({ i });
            };
        }
        for ( int32_t i = 0; i < _compute_pipelines.count(); ++i )
        {
            if ( _compute_pipelines.get( i ) != nullptr )
            {
                destroyComputePipeline({ i });
            };
        }
        // Destroy user-created resources first
        _pipeline_layout_manager.shutdown();

        for (Swapchain* sc = _swapchains.head; sc != nullptr; sc = sc->next )
            destroySwapchain( sc );
        _swapchains.clear();

        if ( _queue_manager )
        {
            _queue_manager->shutdown();
            delete _queue_manager;
            _queue_manager = nullptr;
        }
        
        // Destroy Surface
        if ( _surface != VK_NULL_HANDLE )
        {
            _instance->destroySurface( _surface, nullptr );
            _surface = VK_NULL_HANDLE;
        }

        // Destroy Device
        if (_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(_device, nullptr);
            _device = VK_NULL_HANDLE;
        }

        KEGE_LOG_INFO << "Device Shutdown Complete."<<Log::nl;
    }

    Device::~Device()
    {
        if ( _device != VK_NULL_HANDLE )
        {
            shutdown(); // Ensure shutdown is called if not explicitly done
        }
    }

    Device::Device()
    :   vkSetDebugUtilsObjectName( nullptr )
    ,   _frame_index( 0 )
    {
        //TODO:  Initialize maps, etc. if needed (often done implicitly)
    }

}
