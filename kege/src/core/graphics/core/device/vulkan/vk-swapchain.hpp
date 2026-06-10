//
//  vulkan-swapchain.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#ifndef vulkan_swapchain_hpp
#define vulkan_swapchain_hpp

#include "vk-image.hpp"

namespace kege::vk{

    struct FrameData
    {
        Ref< vk::Image > color;
        Ref< vk::Image > depth;
    };

    struct GpuFrameSync
    {
        /* Signals when the swapchain image is ready to render into (after vkAcquireNextImageKHR).
         * Usually, only the first render pass that writes to the swapchain image should wait on this.
         */
        ref::Semaphore image_available_semaphore;

        /* Render Complete Semaphore:
         * Signals when the final rendering for that image is done (so presentation can occur).
         * The present operation waits on this.
         */
        ref::Semaphore render_complete_semaphore;
    };


    /**
     * @brief Wrapper for Vulkan swapchain resources
     *
     * Encapsulates a VkSwapchainKHR and all its associated resources,
     * including surface, images, views, and synchronization primitives.
     */
    class Swapchain : public kege::Swapchain
    {
    public:

        const ref::Semaphore& getFrameAvailableSemaphore()const;
        const ref::Semaphore& getFrameFinishSemaphore()const;

        std::vector< cref::Image > getColorImages()const;
        std::vector< cref::Image > getDepthImages()const;
        std::vector< ref::Image > getColorImages();
        std::vector< ref::Image > getDepthImages();
        kege::Format getDepthFormat()const ;
        kege::Format getColorFormat()const;
        uint32_t getImageCount()const;
        uint32_t getImageIndex()const;
        Extent2D getExtent()const;

        GpuFrameSync* getGpuFrameSync();

        kege::Result acquireNextImage();

        int getFrameIndex()const;

        void setShouldRecreate( bool state );
        bool shouldRecreate()const;

        Viewport getViewport()const;
        Scissor getScissor()const;

        VkResult create( const SwapchainDesc& desc );
        kege::Result recreate();
        void destroy();

        const VkSwapchainKHR& getHandle()const;
        Swapchain( Device* device );
        ~Swapchain();

    private:

        kege::Result createFrameData( kege::array< vk::FrameData >& frames );
        void destroyFrameData( kege::array< vk::FrameData >& frames );

        VkResult createSwapchain();
        void destroySwapchain(VkSwapchainKHR& swapchain);

        const vk::Swapchain* vk()const{ return this; }
        vk::Swapchain* vk(){ return this; }

    private:

        kege::array< vk::FrameData > _frames;
        kege::array< vk::GpuFrameSync > _gpu_frame_syncs;

        /** @brief Native Vulkan swapchain handle */
        VkSwapchainKHR _swapchain;

        VkExtent2D _extent;

        VkSurfaceFormatKHR _surface_format;
        VkPresentModeKHR _present_mode;
        VkFormat _depth_format;

        /** @brief Index of the currently acquired image */
        uint32_t _image_index;
        uint32_t _image_count;

        uint32_t _frame_index;

        /** @brief Flag indicating if the swapchain needs recreation (e.g., after resize) */
        bool _needs_recreation;


        kege::Viewport _viewport;
        kege::Scissor _scissor;

        vk::Device* _device;

        friend vk::QueueManager;
        friend vk::Device;
        friend vk::List< vk::Swapchain >;
        vk::Swapchain* next;
        vk::Swapchain* prev;
    };

}
#endif /* vulkan_swapchain_hpp */
