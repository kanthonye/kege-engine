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

    bool Device::submit
    (
        const std::vector< kege::SubmitInfo >& submit_infos,
        const ref::Semaphore& image_available,
        const ref::Semaphore& render_complete
    )
    {
        return _queue_manager->submit( submit_infos, image_available->vk(), render_complete->vk(), _frame_index );
    }

    bool  Device::submit( const kege::SubmitInfo& submit_info )
    {
        return _queue_manager->submit( submit_info );
    }

    bool Device::present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem )
    {
        if( _queue_manager->present( swapchain->vk(), wait_sem->vk() ) )
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

    //-------------------------------------------------------------------------
    // SetAllocator
    //-------------------------------------------------------------------------

    vk::SetAllocator* Device::getDescriptorAllocator( const std::vector< VkDescriptorType >& descriptor_types )
    {
        auto i = _descriptor_allocators.find( descriptor_types );
        if( i != _descriptor_allocators.end() ) return i->second.ref();

        kege::Ref< vk::SetAllocator > allocator = new vk::SetAllocator( this, descriptor_types );
        _descriptor_allocators[ descriptor_types ] = allocator;
        return allocator.ref();
   }

    //-------------------------------------------------------------------------
    // SetLayout
    //-------------------------------------------------------------------------

    ref::SetLayout Device::createSetLayout( const SetBindings& bindings )
    {
        auto i = _set_layout_library.find( bindings );
        if( i != _set_layout_library.end() ) return i->second.ref();

        kege::Ref< vk::SetLayout > layout = _set_layouts.insert( new vk::SetLayout( this, bindings ) );
        if ( layout->_handle == VK_NULL_HANDLE )
        {
            _set_layouts.remove( layout.ref() );
            layout.clear();
        }
        return layout.ref();
    }

    void Device::destroySetLayout( vk::SetLayout* layout )
    {
        if ( layout != nullptr )
        {
            if ( layout->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyDescriptorSetLayout( layout->_handle, nullptr );
                _set_layouts.remove( layout );
            }
        }
    }

    //-------------------------------------------------------------------------
    // ShaderLayout
    //-------------------------------------------------------------------------

    ref::ShaderLayout Device::createShaderLayout( const kege::ShaderLayoutDesc& description )
    {
        if ( _device == VK_NULL_HANDLE ) return {};

        std::vector< VkDescriptorSetLayout > descriptor_set_layouts;
        descriptor_set_layouts.reserve( description.set_layout_config.size() );

        kege::IndexedSetLayouts indexed_set_layouts;
        indexed_set_layouts.reserve( description.set_layout_config.size() );
        for (const IndexedSetConfig& config : description.set_layout_config )
        {
            ref::SetLayout set_layout = createSetLayout( config.bindings );
            descriptor_set_layouts.push_back( set_layout->vk()->handle() );
            indexed_set_layouts.push_back({ .index = config.index, .set = set_layout });
        }

        auto i = _shader_layout_lookup.find( descriptor_set_layouts );
        if ( i != _shader_layout_lookup.end() )
        {
            return i->second.ref();
        }

        kege::Ref< vk::ShaderLayout > shader_layout = _shader_layouts.insert
        (
            new vk::ShaderLayout
            (
                this, description.name,
                indexed_set_layouts, description.push_block_layout
            )
        );

        if ( shader_layout->handle() == VK_NULL_HANDLE )
        {
            destroyShaderLayout( shader_layout.ref() );
            shader_layout.clear();
        }
        else
        {
            _shader_layout_lookup[ descriptor_set_layouts ] = shader_layout;
        }
        return shader_layout.ref();
    }

    void Device::destroyShaderLayout( vk::ShaderLayout* layout )
    {
        if ( layout != nullptr )
        {
            if ( layout->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyPipelineLayout( layout->_handle, nullptr );
                _shader_layouts.remove( layout );
            }
        }
    }

    //-------------------------------------------------------------------------
    // ShaderPipeline
    //-------------------------------------------------------------------------

    ref::ShaderPipeline Device::createShaderPipeline( const PipelineCreateInfo& desc )
    {
        ref::ShaderLayout layout = createShaderLayout( desc.shader_layout );
        if ( !layout ) return {};

        Ref< vk::ShaderPipeline > pipeline = _pipelines.insert( new vk::ShaderPipeline( this, desc, layout ) );
        if ( pipeline->_handle == VK_NULL_HANDLE )
        {
            _manager.destroyPipeline( pipeline->_handle, nullptr );
            _pipelines.remove( pipeline.ref() );
            return {};
        }
        return pipeline.ref();
    }

    void Device::destroyShaderPipeline( vk::ShaderPipeline* pipeline )
    {
        if ( pipeline != nullptr )
        {
            if ( pipeline->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyPipeline( pipeline->_handle, nullptr );
                _pipelines.remove( pipeline );
            }
            pipeline->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Shader Resources Creation and Destruction
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Shader Device::createShader( const kege::ShaderDesc& desc )
    {
        kege::Ref< vk::Shader > shader = _shaders.insert( new vk::Shader( this, desc ) );
        if ( shader->handle() == VK_NULL_HANDLE )
        {
            _shaders.remove( shader.ref() );
            shader.clear();
        }
        return shader.ref();
    }

    void Device::destroyShader( vk::Shader* shader )
    {
        if ( shader != nullptr )
        {
            if ( shader->_handle == VK_NULL_HANDLE )
            {
                _manager.destroyShader( shader->_handle );
                shader->_handle = VK_NULL_HANDLE;
                shader->_device = nullptr;
            }
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Swapchain Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Swapchain Device::createSwapchain( const kege::SwapchainDesc& desc )
    {
        VkResult result;
        kege::Ref< vk::Swapchain > swapchain = _swapchains.insert( new vk::Swapchain( this ) );
        if (( result = swapchain->create( desc ) ) != VK_SUCCESS )
        {
            _swapchains.remove( swapchain.ref() );
            swapchain.clear();
            return nullptr;
        }
        return swapchain.ref();
    }

    void Device::destroySwapchain( vk::Swapchain* swapchain )
    {
        if ( swapchain != nullptr )
        {
            _swapchains.remove( swapchain );
            swapchain->destroy();
            swapchain->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // CommandBuffer Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::CommandBuffer Device::createCommandBuffer( kege::QueueType type )
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

    void Device::destroyCommandBuffer( vk::CommandBuffer* cmb )
    {
        if( cmb )
        {
            if ( cmb->handle() != VK_NULL_HANDLE )
            {
                waitIdle();
                vkFreeCommandBuffers( _device, cmb->_command_pool, 1, &cmb->vk()->handle() );
            }
            cmb->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Image Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Image Device::createImage( const kege::Image::Desc& desc )
    {
        if ( _device == VK_NULL_HANDLE ) return {};
        return _images.insert( new vk::Image( this, desc ) );
    }

    void Device::destroyImage( vk::Image* image )
    {
        if ( image == nullptr ) return;

        waitIdle();

        if ( image->_image != VK_NULL_HANDLE && !image->_swapchain_owned )
        {
            _manager.destroyImage( image->_image );
        }
        if ( image->_memory != VK_NULL_HANDLE )
        {
            _manager.destroyMemory( image->_memory );
        }

        if ( image->_view != VK_NULL_HANDLE )
        {
            _manager.destroyImageView( image->_view );
        }

        image->_device = nullptr;
        _images.remove( image );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Sampler Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Sampler Device::createSampler( const kege::SamplerDesc& desc )
    {
        return _samplers.insert( new vk::Sampler( this, desc ) );
    }

    void Device::destroySampler( vk::Sampler* sampler )
    {
        if ( sampler == nullptr ) return;
        if ( sampler->_sampler != VK_NULL_HANDLE )
        {
            _manager.destroySampler( sampler->_sampler );
            sampler->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Buffer Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Buffer Device::createBuffer(const kege::BufferDesc& desc)
    {
        vk::Buffer* buffer = new vk::Buffer( this, desc );
        return _buffers.insert( buffer );
    }

    void Device::destroyBuffer( vk::Buffer* buffer )
    {
        if ( buffer != nullptr )
        {
            vk::Buffer* b = buffer->vk();
            for( int i = 0; i < b->_frames; ++i )
            {
                _manager.destroyBuffer( b->_sources[ i ].handle );
                _manager.destroyMemory( b->_sources[ i ].memory );
            }
            _buffers.remove( buffer );
            b->_device = nullptr;
        }
    }

    void Device::copyBuffer( Buffer::Source& dst, Buffer::Source& src, uint64_t size )
    {
       VkBufferCopy copy_region{};
       copy_region.srcOffset = 0; // Optional
       copy_region.dstOffset = 0; // Optional
       copy_region.size = size;

       VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
       vkCmdCopyBuffer( command_buffer, dst.handle, src.handle, 1, &copy_region );
       _queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Buffer View Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

//    void Device::destroyBufferView(kege::BufferViewHandle handle)
//    {
//    }

    void Device::copyBufferToImage
    (
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkImageAspectFlags aspect_flags,
        VkImageLayout old_layout,
        VkImageLayout new_layout
    )
    {
        VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
        _manager.copyBufferToImage
        (
            command_buffer,
            buffer,
            image,
            width, height,
            aspect_flags,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        _queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    bool Device::copyBufferToImage
    (
        uint64_t size,
        const void* data,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkImageAspectFlags aspect_flags,
        VkImageLayout old_layout,
        VkImageLayout new_layout
    )
    {
        /**
         * create a seperate shared buffer between the CPU & GPU that contains the data.
         */
        vk::Buffer::Source source = {};
        VkResult result = core().createBuffer
        (
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            size, data, &source.handle, &source.memory
        );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result ) << kege::Log::nl;
            return  false;
        }
        
        copyBufferToImage
        (
            source.handle,
            image, width, height, aspect_flags,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        core().destroyBuffer( source.handle );
        core().destroyMemory( source.memory );
        return true;
    }

    // --- Wait Idle ---
    void Device::waitIdle()
    {
        if ( _device == VK_NULL_HANDLE || _device == VK_NULL_HANDLE) return;
        vkDeviceWaitIdle(_device);
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * Create Semaphore
     * Creates a VkSemaphore that can be used to synchronize GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */

    ref::Semaphore Device::createSemaphore()
    {
        return _semaphores.insert( new vk::Semaphore( _manager.createSemaphore(), this ) );
    }

    void Device::destroySemaphore( vk::Semaphore* semaphore )
    {
        if ( semaphore != nullptr )
        {
            if ( semaphore->device != nullptr )
            {
                _manager.destroySemaphore( semaphore->handle );
                semaphore->device = nullptr;
            }
        }
    }

    /*
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     * Create Fence
     * Creates a VkFence that can be used to synchronize CPU and GPU operations.
     * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
     */
    
    ref::Fence Device::createFence( bool initially_signaled )
    {
        return _fences.insert( new vk::Fence( _manager.createFence( initially_signaled ), this ) );
    }
    
    void Device::destroyFence( vk::Fence* fence )
    {
        if ( fence != nullptr )
        {
            if ( fence->device != nullptr )
            {
                _manager.destroyFence( fence->handle );
                fence->device = nullptr;
            }
        }
    }

//    VkSurfaceKHR Device::surface()
//    {
//        return _surface;
//    }
    
    VkDevice Device::handle()
    {
        return _device;
    }

    bool Device::initialize( vk::PhysicalDevice* physical_device )
    {
        _surface = physical_device->getSurface();
        _physical_device = physical_device;
        _instance = physical_device->getInstance();
        if ( physical_device->getHandle() == VK_NULL_HANDLE )
        {
            kege::Log::error << "Invalid PhysicalDevice passed to createLogicalDevice."<<Log::nl;
            return false;
        }

        _queue_family_indices = findQueueFamilies( physical_device->getHandle(), _surface );
        if ( !_queue_family_indices.graphics_family.has_value() ) // graphics is always required
        {
            kege::Log::error << "Failed to find a suitable graphics queue family on the selected device."<<Log::nl;
             return false;
        }

        _device = _manager.createDevice( _physical_device, _queue_family_indices );
        if ( _device == VK_NULL_HANDLE )
        {
            return false;
        }

        _queue_manager = new QueueManager;
        _queue_manager->initialize( this, _queue_family_indices );

        KEGE_LOG_INFO <<"- " << "Vulkan Logical Device created." <<"\n";
        return true;
    }

    void Device::shutdown()
    {
        if ( _device == VK_NULL_HANDLE ) return;

        waitIdle(); // Ensure GPU is idle before destroying anything

        for (vk::Buffer* b = _buffers.head; b != nullptr; b = b->next )
            destroyBuffer( b );
        _buffers.clear();

        for (vk::Image* i = _images.head; i != nullptr; i = i->next )
            destroyImage( i );
        _images.clear();

        for (vk::Sampler* s = _samplers.head; s != nullptr; s = s->next )
            destroySampler( s );
        _samplers.clear();

        for (vk::ShaderLayout* s = _shader_layouts.head; s != nullptr; s = s->next )
            destroyShaderLayout( s );
        _shader_layouts.clear();

        for (vk::Semaphore* s = _semaphores.head; s != nullptr; s = s->next )
            destroySemaphore( s );
        _semaphores.clear();

        for (vk::Fence* f = _fences.head; f != nullptr; f = f->next )
            destroyFence( f );
        _fences.clear();

        for (vk::Shader* s = _shaders.head; s != nullptr; s = s->next )
            destroyShader( s );
        _shaders.clear();

        for (vk::ShaderPipeline* s = _pipelines.head; s != nullptr; s = s->next )
            destroyShaderPipeline( s );
        _pipelines.clear();

        for (Swapchain* sc = _swapchains.head; sc != nullptr; sc = sc->next )
            destroySwapchain( sc );
        _swapchains.clear();

        if ( _queue_manager )
        {
            _queue_manager->shutdown();
            delete _queue_manager;
            _queue_manager = nullptr;
        }

        // Destroy Device
        if (_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(_device, nullptr);
            _device = VK_NULL_HANDLE;
        }
        
        _device = VK_NULL_HANDLE;
        _physical_device = nullptr;
        _instance = nullptr;
        KEGE_LOG_INFO << "Device Shutdown Complete."<<Log::nl;
    }

    vk::Instance* Device::instance()
    {
        return _instance;
    }
    
    vk::Manager& Device::core()
    {
        return _manager;
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
    }

}
