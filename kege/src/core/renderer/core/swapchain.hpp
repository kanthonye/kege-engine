//
//  graphics-swapchain.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/3/25.
//

#ifndef graphics_swapchain_hpp
#define graphics_swapchain_hpp

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Wrapper for Vulkan swapchain resources
     *
     * Encapsulates a VkSwapchainKHR and all its associated resources,
     * including surface, images, views, and synchronization primitives.
     */
    class Swapchain : public kege::RefCounter
    {
    public:

        virtual std::vector< ImageHandle > getColorImages()const = 0;
        virtual std::vector< ImageHandle > getDepthImages()const = 0;
        virtual kege::Format getDepthFormat()const = 0;
        virtual kege::Format getColorFormat()const = 0;
        virtual uint32_t getImageCount()const = 0;
        virtual uint32_t getImageIndex()const = 0;
        virtual Extent2D getExtent()const = 0;
        virtual Viewport getViewport()const = 0;
        virtual Scissor getScissor()const = 0;

        virtual const std::string& name()const{ return _name; }

        virtual const vk::Swapchain* vk()const{ return nullptr; }
        virtual vk::Swapchain* vk(){ return nullptr; }

        virtual ~Swapchain(){}

    protected:

        Swapchain& operator =( const Swapchain& ) = delete;
        void operator =( Swapchain&& ) = delete;

        Swapchain( const Swapchain& ) = delete;
        Swapchain( Swapchain&& ) = delete;

        Swapchain(){}

    private:

        std::string _name;
        friend vk::Device;
    };

}

#endif /* graphics_swapchain_hpp */
