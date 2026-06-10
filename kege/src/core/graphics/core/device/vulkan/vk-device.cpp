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

    bool Device::submitAndPresent
    (
        kege::ref::Swapchain& swapchain,
        const std::vector< kege::SubmitInfo >& submits
    )
    {
        bool success = false;
        if (submits.empty())
        {
            success = _queue_manager->submitEmpty( swapchain->vk() );
        }
        else
        {
            success = _queue_manager->submit( swapchain->vk(), submits );
        }
        if ( success )
        {
            success = _queue_manager->present( swapchain->vk() );
            // If present returned OUT_OF_DATE → flag for next frame
            if (!success || swapchain->shouldRecreate())
            {
                // SUBOPTIMAL_KHR is **not** an error — you can still use the swapchain
                // but most people recreate anyway to get crisp scaling
                swapchain->recreate();
            }
        }
        return success;
    }

    int Device::getFrameIndex()const
    {
        return _queue_manager->getFrameIndex();
    }

    bool Device::beginFrame()
    {
        _queue_manager->waitForFences();
        return true;
    }

    void Device::endFrame()
    {
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderResourceBinder Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::ShaderResourceBinder Device::createShaderResourceBinder
    (
        uint32_t size,
        const kege::ShaderSetDesc* descriptors
    )
    {
        if ( _device == VK_NULL_HANDLE ) return {};

        std::vector< kege::ref::ShaderResourceSet > sets;
        for ( uint32_t i = 0; i < size; ++i )
        {
            sets.push_back( createShaderResourceSet( (uint32_t)descriptors[i].size(), descriptors[i].data() ) );
        }
        return _shader_resource_binders.insert( new kege::vk::ShaderResourceBinder( this, sets ) );
    }
    
    void Device::destroyShaderResourceBinder( kege::vk::ShaderResourceBinder* binder )
    {
        if ( binder )
        {
            _shader_resource_binders.remove( binder );
            binder->_device = nullptr;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderResourceSet Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    kege::ref::ShaderResourceSet Device::createShaderResourceSet( uint32_t size, const kege::ShaderSetBindingDesc* bindings )
    {
        return getShaderResourceSetAllocator( size, bindings )->allocate().ref();
    }

    kege::vk::ShaderResourceSetAllocator* Device::getShaderResourceSetAllocator
    (
        uint32_t size,
        const kege::ShaderSetBindingDesc* descs
    )
    {
        uint64_t key = kege::Hasher::hash( size, descs );
        auto i = _shader_resource_set_allocators.find( key );
        if (i == _shader_resource_set_allocators.end())
        {
            vk::ShaderResourceSetPool* pool = getOrCreateShaderResourceSetPool( size, descs );
            vk::ShaderResourceSetLayout* layout = getOrCreateShaderResourceSetLayout( size, descs, key );
            Ref< vk::ShaderResourceSetAllocator > allocator = new vk::ShaderResourceSetAllocator
            (
                layout, pool
            );
            _shader_resource_set_allocators[ key ] = allocator;
            return allocator.ref();
        }
        return i->second.ref();
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderResourceSetPool Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    vk::ShaderResourceSetPool* Device::getOrCreateShaderResourceSetPool( const std::vector< VkDescriptorType >& descriptor_types )
    {
        auto i = _shader_resource_set_pools.find( descriptor_types );
        if( i != _shader_resource_set_pools.end() ) return i->second.ref();

        kege::Ref< vk::ShaderResourceSetPool > pool = new vk::ShaderResourceSetPool( this, descriptor_types );
        _shader_resource_set_pools[ descriptor_types ] = pool;
        return pool.ref();
   }

    vk::ShaderResourceSetPool* Device::getOrCreateShaderResourceSetPool
    (
        uint32_t size,
        const kege::ShaderSetBindingDesc* descs
    )
    {
        std::vector< VkDescriptorType > descriptor_types;
        for ( uint32_t i = 0; i < size; ++i )
        {
            descriptor_types.push_back( vk::toDescriptorType( descs[i].usage ) );
        }
        return getOrCreateShaderResourceSetPool( descriptor_types );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderResourceSetLayout Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    vk::ShaderResourceSetLayout* Device::getOrCreateShaderResourceSetLayout
    (
        uint32_t size,
        const kege::ShaderSetBindingDesc* descs,
        uint64_t key
    )
    {
        if ( key != 0 )
        {
            auto i = _shader_resource_set_layout_lookup.find( key );
            if ( i !=  _shader_resource_set_layout_lookup.end() )
            {
                return i->second;
            }
        }

        key = kege::Hasher::hash( size, descs );
        kege::UniformKey uniform_key = getOrCreateGlobalUniformKey( key );
        vk::ShaderResourceSetLayout* layout = _shader_resource_set_layouts.insert
        ( new vk::ShaderResourceSetLayout( this, uniform_key, size, descs ) );

        _shader_resource_set_layout_lookup[ key ] = layout;
        return layout;
    }

    void Device::destroyShaderResourceSetLayout( vk::ShaderResourceSetLayout* layout )
    {
        if ( layout != nullptr )
        {
            if ( layout->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyDescriptorSetLayout( layout->_handle, nullptr );
                _shader_resource_set_layouts.remove( layout );
                layout->_handle = VK_NULL_HANDLE;
            }
            layout->_device = nullptr;
        }
    }

    kege::UniformKey Device::getOrCreateGlobalUniformKey
    (
        uint64_t key
    )
    {
        auto m = _global_uniform_key_table.find( key );
        if (m == _global_uniform_key_table.end())
        {
            uint32_t key = uint32_t( _global_uniform_key_table.size());
            _global_uniform_key_table[ key ] = key;
            return key;
        }
        else
        {
            return m->second;
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderLayout Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::ShaderPipelineLayout Device::createShaderPipelineLayout
    (
        const kege::ShaderPipelineLayoutDesc& desc
    )
    {
        if ( _device == VK_NULL_HANDLE ) return {};

        uint64_t key = kege::Hasher::hash( desc );
        auto i = _shader_pipeline_layout_lookup.find( key );
        if ( i != _shader_pipeline_layout_lookup.end() )
        {
            return i->second;
        }

        std::vector< kege::ref::ShaderResourceSetLayout > shader_resource_set_layouts( desc.resource_layout_desc.size() );
        for (uint32_t i=0; i<desc.resource_layout_desc.size(); ++i )
        {
            shader_resource_set_layouts[i] = this->getOrCreateShaderResourceSetLayout
            (
                (uint32_t)desc.resource_layout_desc[i].size(),
                desc.resource_layout_desc[i].data()
            );
        }

        kege::Ref< vk::ShaderPipelineLayout > shader_layout = new vk::ShaderPipelineLayout
        (
            this, desc.name,
            desc.push_block_desc,
            shader_resource_set_layouts
        );

        _shader_pipeline_layouts.insert( shader_layout.ref() );
        _shader_pipeline_layout_lookup[ key ] = shader_layout.ref();
        return shader_layout.ref();
    }

    void Device::destroyShaderPipelineLayout( vk::ShaderPipelineLayout* layout )
    {
        if ( layout != nullptr )
        {
            if ( layout->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyPipelineLayout( layout->_handle, nullptr );
                _shader_pipeline_layouts.remove( layout );
            }
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // ShaderPipeline Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::ShaderPipeline Device::createShaderPipeline( const kege::ShaderPipelineDesc& desc )
    {
        ref::ShaderPipelineLayout layout = createShaderPipelineLayout( desc.pipeline_layout );
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
    //
    // Shader Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    ref::Shader Device::createShader( const kege::ShaderDesc& desc )
    {
        kege::Ref< vk::Shader > shader = _shaders.insert( new vk::Shader( this, desc ) );
        if ( shader->handle() == VK_NULL_HANDLE )
        {
            kege::Log::error << "CREATE_FAILED -> vk::Shader" << kege::Log::nl;
            _shaders.remove( shader.ref() );
            shader.clear();
        }
        return shader.ref();
    }

    void Device::destroyShader( vk::Shader* shader )
    {
        if ( shader != nullptr )
        {
            if ( shader->_handle != VK_NULL_HANDLE )
            {
                _manager.destroyShader( shader->_handle );
                shader->_handle = VK_NULL_HANDLE;
            }
            shader->_device = nullptr;
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
        vk::CommandBuffer* command_buffer = new vk::CommandBuffer( this, type, _queue_manager->_command_pools[ type ] );
        _command_buffers.insert( command_buffer );
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

    ref::Image Device::createImage( const kege::ImageDesc& desc )
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
            _manager.destroyBuffer( b->_handle );
            _manager.destroyMemory( b->_memory );
            _buffers.remove( buffer );
            b->_device = nullptr;
        }
    }

    void Device::copyBuffer( Buffer& dst, Buffer& src, uint64_t size )
    {
       VkBufferCopy copy_region{};
       copy_region.srcOffset = 0; // Optional
       copy_region.dstOffset = 0; // Optional
       copy_region.size = size;

       VkCommandBuffer command_buffer = _queue_manager->beginTransferQueueCommandBuffer();
       vkCmdCopyBuffer( command_buffer, dst._handle, src._handle, 1, &copy_region );
       _queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    //
    // Buffer View Resources Creation and Destruction
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

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
        vk::Buffer source = {};
        VkResult result = core().createBuffer
        (
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            size, data, &source._handle, &source._memory
        );
        if ( result != VK_SUCCESS )
        {
            kege::Log::error << vkResultToString( result ) << kege::Log::nl;
            return  false;
        }
        
        copyBufferToImage
        (
            source._handle,
            image, width, height, aspect_flags,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        core().destroyBuffer( source._handle );
        core().destroyMemory( source._memory );
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
            _semaphores.remove(semaphore);
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
            _fences.remove(fence);
        }
    }

//    VkSurfaceKHR Device::surface()
//    {
//        return _surface;
//    }
    
    const VkDevice& Device::handle()const
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

        _shader_pipeline_layout_lookup.clear();
        _shader_resource_set_layout_lookup.clear();
        _shader_resource_set_pools.clear();
        _global_uniform_key_table.clear();

        for (vk::ShaderResourceBinder* s = _shader_resource_binders.head; s != nullptr; s = s->next )
            destroyShaderResourceBinder( s );
        _shader_resource_binders.clear();

        for (vk::Buffer* b = _buffers.head; b != nullptr; b = b->next )
            destroyBuffer( b );
        _buffers.clear();

        for (vk::Image* i = _images.head; i != nullptr; i = i->next )
            destroyImage( i );
        _images.clear();

        for (vk::Sampler* s = _samplers.head; s != nullptr; s = s->next )
            destroySampler( s );
        _samplers.clear();

        for (vk::Shader* s = _shaders.head; s != nullptr; s = s->next )
            destroyShader( s );
        _shaders.clear();

        for (vk::ShaderPipeline* s = _pipelines.head; s != nullptr; s = s->next )
            destroyShaderPipeline( s );
        _pipelines.clear();

        for (vk::ShaderPipelineLayout* s = _shader_pipeline_layouts.head; s != nullptr; s = s->next )
            destroyShaderPipelineLayout( s );
        _shader_pipeline_layouts.clear();

        _shader_resource_set_allocators.clear();
        for (vk::ShaderResourceSetLayout* s = _shader_resource_set_layouts.head; s != nullptr; s = s->next )
            destroyShaderResourceSetLayout( s );
        _shader_resource_set_layouts.clear();

        for (vk::Semaphore* s = _semaphores.head; s != nullptr; s = s->next )
            destroySemaphore( s );
        _semaphores.clear();

        for (vk::Fence* f = _fences.head; f != nullptr; f = f->next )
            destroyFence( f );
        _fences.clear();

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

        _physical_device = nullptr;
        _device = VK_NULL_HANDLE;
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
    {
    }

}
