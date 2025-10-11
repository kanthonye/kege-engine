//
//  graphics.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#include "graphics.hpp"

namespace kege{

    bool Graphics::submit( const std::vector< kege::SubmitInfo >& submit_infos, kege::Swapchain* swapchain )
    {
        return _device->submit( submit_infos, swapchain );
    }

    bool Graphics::submit( const kege::SubmitInfo& submit_info )
    {
        return _device->submit( submit_info );
    }

    bool Graphics::present( kege::Swapchain* swapchain )
    {
        return _device->present( swapchain );
    }

    CommandBuffer* Graphics::createCommandBuffer( QueueType type )
    {
        return _device->createCommandBuffer( type );
    }

    void Graphics::destroyCommandBuffer(CommandBuffer* command_buffer)
    {
        _device->destroyCommandBuffer( command_buffer );
    }

    ImageHandle Graphics::createImage(const ImageDesc& desc)
    {
        return _device->createImage( desc );
    }

    void Graphics::destroyImage(ImageHandle handle)
    {
        _device->destroyImage( handle );
    }

    BufferHandle Graphics::createBuffer(const BufferDesc& desc)
    {
        return _device->createBuffer( desc );
    }

    void Graphics::destroyBuffer(BufferHandle handle)
    {
        _device->destroyBuffer( handle );
    }
    void Graphics::destroyBufferView( BufferViewHandle handle )
    {
        _device->destroyBufferView( handle );
    }

    void Graphics::updateBuffer( const BufferHandle& handle, uint64_t offset, uint64_t size, const void* data )
    {
        _device->updateBuffer( handle, offset, size, data );
    }
    
    void Graphics::unmapBuffer( const BufferHandle& handle )
    {
        _device->unmapBuffer( handle );
    }

    void* Graphics::mapBuffer( const BufferHandle& handle, size_t offset, size_t size )
    {
        return _device->mapBuffer( handle, offset, size );
    }

    bool Graphics::resizeBuffer( const BufferHandle& handle, uint64_t size )
    {
        return _device->resizeBuffer( handle, size );
    }

    size_t Graphics::bufferSize( const BufferHandle& handle )
    {
        return _device->bufferSize( handle );
    }

    SamplerHandle Graphics::createSampler(const SamplerDesc& desc)
    {
        return _device->createSampler( desc );
    }

    void Graphics::destroySampler(SamplerHandle handle)
    {
        _device->destroySampler( handle );
    }

    ShaderHandle Graphics::createShader(const ShaderDesc& desc)
    {
        return _device->createShader( desc );
    }

    void Graphics::destroyShader(ShaderHandle handle)
    {
        _device->destroyShader( handle );
    }

    PipelineLayoutHandle Graphics::createPipelineLayout(const PipelineLayoutDesc& desc)
    {
        return _device->createPipelineLayout( desc );
    }

    void Graphics::destroyPipelineLayout(PipelineLayoutHandle handle)
    {
        _device->destroyPipelineLayout( handle );
    }

    std::vector< PipelineHandle > Graphics::createGraphicsPipeline( const CreateShaderPipelineInfo& desc )
    {
        return _device->createGraphicsPipeline( desc );
    }

    void Graphics::destroyGraphicsPipeline(PipelineHandle handle)
    {
        _device->destroyGraphicsPipeline( handle );
    }

    PipelineHandle Graphics::createComputePipeline(const ComputePipelineDesc& desc)
    {
        return _device->createComputePipeline( desc );
    }

    void Graphics::destroyComputePipeline(PipelineHandle handle)
    {
        _device->destroyComputePipeline( handle );
    }


    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    // UniformSetLayout
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

    UniformSetLayout Graphics::createUniformSetLayout( const UniformDescriptors& descriptors )
    {
        return _device->createUniformSetLayout( descriptors );
    }

    UniformSetLayout Graphics::getUniformSetLayout( const UniformDescriptors& descriptors )
    {
        return _device->getUniformSetLayout( descriptors );
    }

    void Graphics::destroyUniformSetLayout( const UniformSetLayout& layout )
    {
        _device->destroyUniformSetLayout( layout );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
    // Shader Resource Set Lifecycle
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

    int  Graphics::makeSet( const UniformDescriptorSet& descriptors, const UniformResourceSet& resources )
    {
        return _device->makeSet( descriptors, resources );
    }

    bool Graphics::updateSet( int handle, const UniformResourceSet& resources )
    {
        return _device->updateSet( handle, resources );
    }

    int  Graphics::allocateSet( const UniformDescriptorSet& descriptors )
    {
        return _device->allocateSet( descriptors );
    }
    
    void Graphics::freeSet( int set )
    {
        return _device->freeSet( set );
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    //
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

    kege::Ref< kege::Fence > Graphics::createFence( bool initially_signaled )
    {
        return _device->createFence( initially_signaled );
    }

    void Graphics::destroyFence( kege::Fence* fence )
    {
        _device->destroyFence( fence );
    }

    kege::Ref< kege::Semaphore > Graphics::createSemaphore()
    {
        return _device->createSemaphore();
    }

    void Graphics::destroySemaphore( kege::Semaphore* semaphore )
    {
        _device->destroySemaphore( semaphore );
    }

    kege::ShaderPipelineManager* Graphics::getShaderPipelineManager()
    {
        return &_shader_pipeline_manager;
    }

    kege::Swapchain* Graphics::getSwapchain()
    {
        return _swapchain;
    }

    kege::GraphicsWindow* Graphics::getWindow()
    {
        return _window.ref();
    }

    int32_t Graphics::windowHeight()const
    {
        return _window->getHeight();
    }
    
    int32_t Graphics::windowWidth()const
    {
        return _window->getWidth();
    }

    void Graphics::pollWindowEvents()
    {
        _window->pollEvents();
    }

    bool Graphics::windowIsOpen()const
    {
        return !_window->shouldClose();
    }

    bool Graphics::beginFrame()
    {
        return _device->beginSubmit();
    }

    void Graphics::endFrame()
    {
        _device->endSubmit();
    }

    int32_t Graphics::getFrameIndex()const
    {
        return _device->getFrameIndex();
    }

    bool Graphics::initalize
    (
        kege::Ref< kege::GraphicsWindow > window,
        const kege::DeviceInitializationInfo& instance_info,
        const kege::SwapchainDesc& swapchain_create_info
    )
    {
        _window = window;

        // choose the rendering API
        switch ( instance_info.preferred_API )
        {
            case GraphicsAPI::Vulkan:
            {
                _instance = new kege::vk::Instance;
                break;
            }
            case GraphicsAPI::Metal:
            {
                break;
            }
            case GraphicsAPI::D3D12:
            {
                break;
            }
            default:
            {
                break;
            }
        }

        if ( !_instance->initalize( instance_info ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize GraphicsInstance."<<Log::nl;
            return false;
        }
        KEGE_LOG_INFO << "GraphicsInstance initialized..."<<Log::nl;

        // create the window surface linking the graphics instance to a render window
        kege::GraphicsSurface surface = window->createSurface( _instance.ref() );

        // next select the best graphics card for rendering
        kege::PhysicalDevice* physical_device = _instance->getBestSuitablePhysicalDevice( instance_info, surface );

        // with the physical device selected next create the logical device which is the main device the user interact with.
        _device = _instance->createDevice( physical_device, surface );
        if( !_device )
        {
            KEGE_LOG_ERROR << "Failed to create GraphicsDevice."<<Log::nl;
            return false;
        }
        KEGE_LOG_INFO << "GraphicsDevice initialized..."<<Log::nl;

        // with the logical device created, create the device swapchain
        _swapchain_create_info = swapchain_create_info;
        _swapchain = _device->createSwapchain( _swapchain_create_info );
        if( !_swapchain )
        {
            return false;
        }

        _shader_pipeline_manager.initalize( this );
        return true;
    }

    void Graphics::shutdown()
    {
        _shader_pipeline_manager.shutdown();
        
        if ( _device != nullptr )
        {
            _device->shutdown();
            _device = nullptr;
        }

        if ( _instance )
        {
            _instance->shutdown();
            _instance.clear();
        }
        if ( _window )
        {
            _window->destroy();
            _window.clear();
        }
    }

    Graphics::Graphics()
    :   _device( nullptr )
    ,   _window()
    {}

    Graphics::~Graphics()
    {
        shutdown();
    }
}
