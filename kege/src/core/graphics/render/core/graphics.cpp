//
//  graphics.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#include "shaderc/shaderc.hpp"
#include "graphics.hpp"
#include "../vulkan/vk-instance.hpp"
#include "../vulkan/vk-device.hpp"

namespace kege{

    std::vector< uint32_t > Graphics::compileGlslToSpv
    (
        ShaderStageFlag shader_stage,
        const char* shader_name,
        const char* source,
        size_t source_size
    )
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderc_shader_type;

        switch ( shader_stage )
        {
            case kege::ShaderStageFlag::Compute:
            {
                shaderc_shader_type = shaderc_compute_shader;
                break;
            }
            case kege::ShaderStageFlag::Vertex:
            {
                shaderc_shader_type = shaderc_vertex_shader;
                break;
            }
            case kege::ShaderStageFlag::Fragment:
            {
                shaderc_shader_type = shaderc_fragment_shader;
                break;
            }
            case kege::ShaderStageFlag::Geometry:
            {
                shaderc_shader_type = shaderc_geometry_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationEvaluation:
            {
                shaderc_shader_type = shaderc_tess_evaluation_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationControl:
            {
                shaderc_shader_type = shaderc_tess_control_shader;
                break;
            }
            default:
            {
                kege::Log::error <<"Invalid shader state in compileGlslTextToByteCode()" << Log::nl;
                return {};
            }
        }
        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv
        (
            source,
            source_size - 1,
            shaderc_shader_type,
            shader_name,
            options
        );
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            kege::Log::error <<"error in compileGlslTextToByteCode(): "<< result.GetErrorMessage().data() << Log::nl;
            return {};
        }
        return std::vector< uint32_t >(result.cbegin(), result.cend());
    }

    bool Graphics::submit
    (
        const std::vector< kege::SubmitInfo >& submit_infos,
        const ref::Semaphore& image_available,
        const ref::Semaphore& render_complete
    )
    {
        return _device->submit( submit_infos, image_available, render_complete );
    }

    bool Graphics::submit( const kege::SubmitInfo& submit_info )
    {
        return _device->submit( submit_info );
    }

    bool Graphics::present( kege::Swapchain* swapchain, const ref::Semaphore& wait_sem )
    {
        return _device->present( swapchain, wait_sem );
    }

    ref::CommandBuffer Graphics::createCommandBuffer( QueueType type )
    {
        return _device->createCommandBuffer( type );
    }

    ref::Image Graphics::createImage(const Image::Desc& desc)
    {
        return _device->createImage( desc );
    }

    ref::Buffer Graphics::createBuffer(const BufferDesc& desc)
    {
        return _device->createBuffer( desc );
    }

    ref::Sampler Graphics::createSampler(const SamplerDesc& desc)
    {
        return _device->createSampler( desc );
    }

    ref::Shader Graphics::createShader(const ShaderDesc& desc)
    {
        return _device->createShader( desc );
    }

    ref::ShaderLayout Graphics::createShaderLayout(const ShaderLayoutDesc& desc)
    {
        return _device->createShaderLayout( desc );
    }

    ref::ShaderPipeline Graphics::createShaderPipeline(const kege::ShaderPipelineDesc& desc)
    {
        return _device->createShaderPipeline( desc );
    }

    ref::Fence Graphics::createFence( bool initially_signaled )
    {
        return _device->createFence( initially_signaled );
    }

    ref::Semaphore Graphics::createSemaphore()
    {
        return _device->createSemaphore();
    }

    kege::Swapchain* Graphics::getSwapchain()
    {
        return _swapchain.ref();
    }

    kege::AppWindow* Graphics::getWindow()
    {
        return _window.ref();
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
        const kege::DeviceInitializationInfo& info,
        const kege::SwapchainDesc& swapchain_create_info
    )
    {
        _window = info.window;

        // choose the rendering API
        switch ( info.preferred_API )
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

        if ( !_instance->initalize( info ) )
        {
            kege::Log::error << "Failed to initialize GraphicsInstance."<<Log::nl;
            return false;
        }
        KEGE_LOG_INFO << "GraphicsInstance initialized..."<<Log::nl;

        // create the window surface linking the graphics instance to a render window
        //kege::GraphicsSurface surface = window->createSurface( _instance.ref() );

        // with the physical device selected next create the logical device which is the main device the user interact with.
        _device = _instance->createDevice( _instance->getBestSuitablePhysicalDevice( info ) );
        if( !_device )
        {
            kege::Log::error << "Failed to create GraphicsDevice."<<Log::nl;
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

        return true;
    }

    void Graphics::shutdown()
    {
        _swapchain.clear();
        _device.clear();
        _instance.clear();
        _window.clear();
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
