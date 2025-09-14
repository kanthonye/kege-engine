//
//  graphics.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#include "graphics.hpp"

namespace kege{

    CommandBuffer* Graphics::createCommandBuffer( QueueType type )
    {
        return _device->createCommandBuffer( type );
    }

    void Graphics::submitCommands( std::vector< CommandBuffer* > command_buffers )
    {
        if ( !_wait_semaphore )
        {
            _wait_semaphore = _image_available_semaphores[ _current_frame ];
        }

        QueueType type = command_buffers[0]->getQueueType();
        for(int i=0; i<command_buffers.size(); ++i)
        {
            std::vector< CommandBuffer* > commands;
            while ( type == command_buffers[i]->getQueueType() )
            {
                commands.push_back( command_buffers[i] );
                i += 1;
                if (i >= command_buffers.size() )
                    break;
            }
            if (i < command_buffers.size() )
            {
                type = command_buffers[i]->getQueueType();
            }

            if ( _cmb_submit_count >= _cmb_fences[ _current_frame ].size() )
            {
                _cmb_fences[ _current_frame ].push_back( _device->createFence() );
                _cmb_semaphores[ _current_frame ].push_back( _device->createSemaphore() );
            }

            FenceHandle signal_fence = _cmb_fences[ _current_frame ][ _cmb_submit_count ];
            SemaphoreHandle signal_semaphore = _cmb_semaphores[ _current_frame ][ _cmb_submit_count ];

            _device->submitCommands( commands, &signal_fence, &signal_semaphore, &_wait_semaphore );
            _wait_semaphore = signal_semaphore;
            _cmb_submit_count += 1;
        }
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

    void Graphics::insertPipeline( const std::string& name, const PipelineHandle& pipeline )
    {
        _shader_pipeline_manager.set( name, pipeline );
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

    PipelineHandle Graphics::createGraphicsPipeline(const GraphicsPipelineDesc& desc)
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



    UniformSetLayout Graphics::getUniformSetLayout( const UniformLayoutDescription& descriptors )
    {
        return _device->getUniformSetLayout( descriptors, false );
    }

    UniformSetLayout Graphics::createUniformSetLayout( const UniformLayoutDescription& descriptors )
    {
        return _device->createUniformSetLayout( descriptors );
    }

    void Graphics::destroyUniformSetLayout(UniformSetLayout handle)
    {
        _device->destroyUniformSetLayout( handle );
    }


    bool Graphics::allocateShaderResources( const UniformLayoutDescription& descriptors, int quantity, ShaderResource* resource )
    {
        std::size_t key = kege::hash( descriptors );
        auto m = _uniform_set_layouts.find( key );
        if ( m == _uniform_set_layouts.end() )
        {
            return false;
        }
        return allocateShaderResources( m->second, quantity, resource );
    }

    bool Graphics::allocateShaderResource( const UniformDesc& descriptor, ShaderResource& resource )
    {
        std::size_t key = kege::hash( descriptor );
        auto m = _uniform_set_layouts.find( key );
        if ( m == _uniform_set_layouts.end() )
        {
            return false;
        }
        return allocateShaderResources( m->second, 1, &resource );
    }

    bool Graphics::allocateShaderResources( const UniformSetLayout& layout, int quantity, ShaderResource* resource )
    {
        int32_t descriptor_ids[ quantity ];
        if( _device->allocateDescriptors( layout, quantity, descriptor_ids ) )
        {
            _shader_resource_manager.generate( quantity, descriptor_ids, resource );
            return true;
        }
        return false;
    }

    bool Graphics::allocateShaderResource( const UniformSetLayout& layout, ShaderResource& resource )
    {
        return allocateShaderResources( layout, 1, &resource );
    }
    
    bool Graphics::updateShaderResource( kege::ShaderResource& resource, const UniformBindingElements& elements )
    {
        return _device->updateDescriptor( *resource, elements );
    }

    bool Graphics::updateShaderResource( kege::ShaderResource& resource )
    {
        return _device->updateDescriptor( resource, resource.resources() );
    }

    void Graphics::freeShaderResource( int quantity, ShaderResource* resources )
    {
        _shader_resource_manager.free( quantity, resources );
    }

    void Graphics::freeShaderResource( ShaderResource& resources )
    {
        _shader_resource_manager.free( 1, &resources );
    }

    bool Graphics::createShaderResources( const CreateShaderResources& create )
    {
        std::size_t key = kege::hash( create.descriptors );
        auto m = _uniform_set_layouts.find( key );
        if ( m == _uniform_set_layouts.end() )
        {
            return false;
        }

        if( !allocateShaderResources( m->second, create.quantity, create.resources ) )
        {
            return false;
        }

        for (int count=0; count<create.quantity; ++count)
        {
            UniformBindingElements binding_elements;
            for (int i=0; i<create.binding_buffer_sets.size(); ++i)
            {
                const CreateBufferBindingPair& info = create.binding_buffer_sets[i];
                std::vector< BufferInfo > buffer_infos;

                for (int k=0; k<info.create_infos.size(); ++k)
                {
                    BufferHandle buffer = createBuffer( info.create_infos[k].desc );

                    buffer_infos.push_back
                    ({
                        .buffer = buffer,
                        .range = info.create_infos[k].range,
                        .offset = info.create_infos[k].offset
                    });
                }

                binding_elements.push_back
                ({
                    BufferBinding
                    {
                        .binding = info.binding,
                        .buffers = buffer_infos
                    }
                });
            }

            for (int i=0; i<create.binding_buffer_sets.size(); ++i)
            {
                const CreateImageBindingPair& info = create.binding_image_sets[i];
                std::vector< ImageInfo > image_infos;

                for (int k=0; k<info.create_infos.size(); ++k)
                {
                    ImageHandle img = createImage( info.create_infos[k].desc );

                    image_infos.push_back
                    ({
                        .image = img,
                        .sampler = info.create_infos[k].sampler
                    });
                }

                binding_elements.push_back
                ({
                    ImageBindings
                    {
                        .binding = info.binding,
                        .images = image_infos
                    }
                });
            }
            updateShaderResource( create.resources[ count ], binding_elements );
        }
        return true;
    }

    bool Graphics::makeShaderResources( const MakeShaderResources& parameters )
    {
//        std::size_t key = kege::hash( create.descriptors );
//        auto m = _uniform_set_layouts.find( key );
//        if ( m == _uniform_set_layouts.end() )
//        {
//            return false;
//        }
//
//        if( !allocateShaderResources( m->second, create.quantity, create.resources ) )
//        {
//            return false;
//        }
//
//        for (int count=0; count<create.quantity; ++count)
//        {
//            UniformBindingElements binding_elements;
//            for (int i=0; i<create.binding_buffer_sets.size(); ++i)
//            {
//                const std::pair< int, CreateBufferSet >& info = create.binding_buffer_sets[i];
//                std::vector< BufferInfo > buffer_infos;
//
//                for (int k=0; k<info.second.size(); ++k)
//                {
//                    BufferHandle buffer = createBuffer( info.second[k].desc );
//
//                    buffer_infos.push_back
//                    ({
//                        .buffer = buffer,
//                        .range = info.second[k].range,
//                        .offset = info.second[k].offset
//                    });
//                }
//
//                binding_elements.push_back( Uniform( info.first, buffer_infos ) );
//            }
//
//            for (int i=0; i<create.binding_buffer_sets.size(); ++i)
//            {
//                const std::pair< int, CreateImageSet >& info = create.binding_image_sets[i];
//                std::vector< ImageInfo > image_infos;
//
//                for (int k=0; k<info.second.size(); ++k)
//                {
//                    ImageHandle img = createImage( info.second[k].desc );
//
//                    image_infos.push_back
//                    ({
//                        .image = img,
//                        .sampler = info.second[k].sampler
//                    });
//                }
//
//                binding_elements.push_back( Uniform( info.first, image_infos ) );
//            }
//            updateShaderResource( create.resources[ count ], binding_elements );
//        }
        return true;
    }



    FenceHandle Graphics::createFence( bool initially_signaled )
    {
        return _device->createFence( initially_signaled );
    }

    void Graphics::destroyFence(FenceHandle handle)
    {
        _device->destroyFence( handle );
    }

    SemaphoreHandle Graphics::createSemaphore()
    {
        return _device->createSemaphore();
    }

    void Graphics::destroySemaphore(SemaphoreHandle handle)
    {
        _device->destroySemaphore( handle );
    }

    ImageHandle Graphics::getSwapchainColorImage( uint32_t image_index )
    {
        return _device->getSwapchainColorImage( _swapchain, image_index );
    }

    std::vector< ImageHandle > Graphics::getSwapchainColorImages()
    {
        return _device->getSwapchainColorImages( _swapchain );
    }

    ImageHandle Graphics::getSwapchainDepthImage( uint32_t image_index )
    {
        return _device->getSwapchainDepthImage( _swapchain, image_index );
    }

    std::vector< ImageHandle > Graphics::getSwapchainDepthImages()
    {
        return _device->getSwapchainDepthImages( _swapchain );
    }

    uint32_t Graphics::getSwapchainImageIndex()
    {
        return _device->getSwapchainImageIndex( _swapchain );
    }

    uint32_t Graphics::getSwapchainImageCount()
    {
        return _device->getSwapchainImageCount( _swapchain );
    }

    Extent2D Graphics::getSwapchainExtent()
    {
        return _device->getSwapchainExtent( _swapchain );
    }

    Format Graphics::getSwapchainColorFormat()
    {
        return _device->getSwapchainColorFormat( _swapchain );
    }

    Format Graphics::getSwapchainDepthFormat()
    {
        return _device->getSwapchainDepthFormat( _swapchain );
    }

    uint32_t Graphics::getCurrFrameIndex()const
    {
        return _current_frame;
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

    kege::ShaderPipelineManager* Graphics::getShaderPipelineManager()
    {
        return &_shader_pipeline_manager;
    }

    kege::ShaderResourceManager* Graphics::getShaderResourceManager()
    {
        return &_shader_resource_manager;
    }

    int Graphics::beginFrame()
    {
        //_shader_resource_manager.reset();
        
        if ( _cmb_submit_count != 0 )
        {
            _device->waitForFence( _cmb_submit_count, _cmb_fences[ _current_frame ].data(), true, UINT64_MAX );
            _device->resetFence( _cmb_submit_count, _cmb_fences[ _current_frame ].data() ); // Reset fence for the new frame
        }
        _cmb_submit_count = 0;
        _wait_semaphore = {-1};

        if( !_device->acquireNextSwapchainImage( _swapchain, _image_available_semaphores[ _current_frame ], &_image_index ) )
        {
            if( _device->needsRecreation( _swapchain ) )
            {
                _device->waitIdle();
                _swapchain_create_info.old_swapchain = _swapchain;
                _swapchain_create_info.width = _window->getWidth();
                _swapchain_create_info.height = _window->getHeight();
                _swapchain = _device->createSwapchain( _swapchain_create_info );
                if( !_swapchain )
                {
                    return -1;
                }
            }
            else
            {
               // Handle other present error
               return -1;
            }
        }
        return _current_frame;
    }

    bool Graphics::endFrame()
    {
        SemaphoreHandle render_finish = _wait_semaphore;
        // 5. Present the image
        //    *** CRITICAL: Wait on the RENDER FINISHED semaphore ***
        if (!_device->presentSwapchainImage( _swapchain, render_finish, _image_index ) )
        {
            if( _device->needsRecreation( _swapchain ) )
             {
                 _device->waitIdle();
                 _swapchain_create_info.old_swapchain = _swapchain;
                 _swapchain_create_info.width = _window->getWidth();
                 _swapchain_create_info.height = _window->getHeight();
                 _swapchain = _device->createSwapchain( _swapchain_create_info );
                 if( !_swapchain )
                 {
                     return -1;
                 }
             }
             else
             {
                 KEGE_LOG_ERROR << "submission to device queue failed in endFrame()";
                // Handle other present error
                return -1;
             }
        }
        _current_frame = (_current_frame + 1) % kege::MAX_FRAMES_IN_FLIGHT;
        return true;
    }

    bool Graphics::initalize
    (
        kege::Ref< kege::GraphicsWindow > window,
        const kege::DeviceInitializationInfo& instance_info,
        const kege::SwapchainDesc& swapchain_create_info
    )
    {
        _window = window;

        kege::DeviceInitializationInfo device_init_info = {};
        device_init_info.window = window.ref();
        device_init_info.enable_raytracing = false;
        device_init_info.prefer_discrete_gpu = true;
        device_init_info.prefer_higher_api_version = true;
        device_init_info.require_shader_float64 = false;
        device_init_info.engine = "KEGE";
        device_init_info.name = "dev";

        // choose the rendering API
        switch ( device_init_info.preferred_API )
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

        if ( !_instance->initalize( device_init_info ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize GraphicsInstance."<<Log::nl;
            return false;
        }
        KEGE_LOG_INFO << "GraphicsInstance initialized..."<<Log::nl;

        // create the window surface linking the graphics instance to a render window
        kege::GraphicsSurface surface = window->createSurface( _instance.ref() );

        // next select the best graphics card for rendering
        kege::PhysicalDevice* physical_device = _instance->getBestSuitablePhysicalDevice( device_init_info, surface );

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

        // next create synchronization components
        for (int i=0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            //_render_finish_semaphore[i] = _device->createSemaphore();
            _image_available_semaphores[i] = _device->createSemaphore();
            if( _image_available_semaphores[i].id < 0 )
            {
                return false;
            }

            _cmb_submit_count = 0;
            _cmb_fences[i].resize( _initial_submits_per_frame );
            _cmb_semaphores[i].resize( _initial_submits_per_frame );
            for (int k=0; k<_initial_submits_per_frame; ++k)
            {
                _cmb_fences[i][k] = _device->createFence();
                _cmb_semaphores[i][k] = _device->createSemaphore();
            }
        }

        _shader_pipeline_manager.initalize( this );
        _shader_resource_manager.initalize( _device );
        return true;
    }

    bool Graphics::initalize()
    {
//        kege::WindowCreateInfo create_window_info = {};
//        create_window_info.title = _info.title;
//        create_window_info.width = _info.width;
//        create_window_info.height = _info.height;
//        create_window_info.visible = true;
//        create_window_info.resizable = true;
//        create_window_info.fullscreen = _info.fullscreen;
//        create_window_info.maximized = false;
//        create_window_info.decorated = true;
//        create_window_info.vsync = _info.vsync;
//
//        kege::Ref< kege::GraphicsWindow > window = new kege::GlfwWindow();
//        if ( !window->create( create_window_info ) )
//        {
//            KEGE_LOG_ERROR << "Failed to initialize GraphicsWindow."<<Log::nl;
//            return 0;
//        }
//
//        kege::DeviceInitializationInfo device_init_info = {};
//        device_init_info.window = window.ref();
//        device_init_info.preferred_API = _info.api;
//        device_init_info.enable_raytracing = _info.enable_raytracing;
//        device_init_info.prefer_discrete_gpu = true;
//        device_init_info.prefer_higher_api_version = true;
//        device_init_info.require_shader_float64 = _info.require_shader_float64;
//        device_init_info.engine = "KEGE";
//        device_init_info.name = _info.title;
//
//        kege::SwapchainDesc swapchain_create_info = {};
//        swapchain_create_info.debug_name = "swapchain";
//        swapchain_create_info.width = _info.width;
//        swapchain_create_info.height = _info.height;
//        swapchain_create_info.image_count = _info.frames_in_flight;
//        swapchain_create_info.color_format = _info.color_format;
//        swapchain_create_info.depth_format = _info.depth_format;
//        swapchain_create_info.present_mode = kege::PresentMode::Fifo;
//        swapchain_create_info.present_queue_type = kege::QueueType::Graphics;
//        swapchain_create_info.image_usage = kege::ImageUsageFlags::ColorAttachment | kege::ImageUsageFlags::CopyDst;
//
//        return initalize( window, device_init_info, swapchain_create_info );
        return false;
    }

    void Graphics::shutdown()
    {
        _shader_resource_manager.shutdown();
        _shader_pipeline_manager.shutdown();
        
        if ( _device != nullptr )
        {
            for (int i=0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                //_device->destroySemaphore( _render_finish_semaphore[i] );
                _device->destroySemaphore( _image_available_semaphores[i] );

                for (int k=0; k<_initial_submits_per_frame; ++k)
                {
                    _device->destroyFence( _cmb_fences[i][k] );
                    _device->destroySemaphore( _cmb_semaphores[i][k] );
                }
                _cmb_semaphores[i].clear();
                _cmb_fences[i].clear();
            }

            if( !_swapchain )
            {
                _device->destroySwapchain( _swapchain );
                _swapchain = {};
            }
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

//    Graphics::Graphics( GraphicsAPIInfo info )
//    :   _info( info )
//    ,   _device()
//    ,   _window()
//    ,   _cmb_submit_count( 0 )
//    ,   _initial_submits_per_frame( 5 )
//    ,   _current_frame( 0 )
//    {}

    Graphics::Graphics()
    :   _device( nullptr )
    ,   _window()
    ,   _cmb_submit_count( 0 )
    ,   _initial_submits_per_frame( 5 )
    ,   _current_frame( 0 )
    {}

    Graphics::~Graphics()
    {
        shutdown();
    }
}
