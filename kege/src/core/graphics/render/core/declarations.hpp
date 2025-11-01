//
//  declarations.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef forward_declarations_hpp
#define forward_declarations_hpp

#include <assert.h>
#include "ref.hpp"

namespace kege
{
    class AppWindow;
    
    class Image;
    class Sampler;
    class Buffer;
    class Graphics;
    class GraphicsInstance;
    class GraphicsDevice;
    class CommandBuffer;

    class Shader;
    class ShaderData;
    class ShaderLayout;
    class ShaderPipeline;


    class ShaderSet;
    class SetLayout;

    class PhysicalDevice;

    class Fence;
    class Semaphore;
}

namespace kege::ref
{
    typedef kege::Ref< kege::CommandBuffer > CommandBuffer;
    typedef kege::Ref< kege::PhysicalDevice > PhysicalDevice;

    typedef kege::Ref< kege::Image > Image;
    typedef kege::Ref< kege::Buffer > Buffer;
    typedef kege::Ref< kege::Sampler > Sampler;

    typedef kege::Ref< kege::Fence > Fence;
    typedef kege::Ref< kege::Semaphore > Semaphore;

    typedef kege::Ref< kege::Shader > Shader;
    typedef kege::Ref< kege::ShaderLayout > ShaderLayout;
    typedef kege::Ref< kege::ShaderPipeline > ShaderPipeline;

    typedef kege::Ref< kege::SetLayout > SetLayout;
    typedef kege::Ref< kege::ShaderSet > ShaderSet;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::PhysicalDevice > PhysicalDevice;

    typedef kege::Ref< const kege::Image > Image;
    typedef kege::Ref< const kege::Buffer > Buffer;
    typedef kege::Ref< const kege::Sampler > Sampler;

    typedef kege::Ref< const kege::Fence > Fence;
    typedef kege::Ref< const kege::Semaphore > Semaphore;

    typedef kege::Ref< const kege::ShaderLayout > ShaderLayout;
    typedef kege::Ref< const kege::ShaderPipeline > ShaderPipeline;

}

namespace kege::vk{

    class Device;
    class PhysicalDevice;
    
    class Swapchain;
    class CommandBuffer;

    class Fence;
    class Semaphore;

    class Image;
    class Sampler;
    class Buffer;

    class Shader;
    class ShaderLayout;
    class ShaderPipeline;

    class ShaderSet;
    class SetLayout;
}

#endif /* forward_declarations_hpp */
