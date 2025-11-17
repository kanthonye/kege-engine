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


    struct FrameSync
    {
        ref::Semaphore image_available_semaphore;
        /* Signals when the swapchain image is ready to render into (after vkAcquireNextImageKHR).
         * Usually, only the first render pass that writes to the swapchain image should wait on this.
         */

        ref::Semaphore render_complete_semaphore;
        /* Render Complete Semaphore:
         * Signals when the final rendering for that image is done (so presentation can occur).
         * The present operation waits on this.
         */
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

        const ref::Semaphore& getImageAvailableSemaphore( uint32_t image_index )const;
        const ref::Semaphore& getRenderFinishSemaphore( uint32_t image_index )const;

        std::vector< cref::Image > getColorImages()const;
        std::vector< cref::Image > getDepthImages()const;
        std::vector< ref::Image > getColorImages();
        std::vector< ref::Image > getDepthImages();
        kege::Format getDepthFormat()const ;
        kege::Format getColorFormat()const;
        uint32_t getImageCount()const;
        uint32_t getImageIndex()const;
        Extent2D getExtent()const;

        FrameSync& getFrameSync( uint32_t frame_index );
        int32_t acquireNextImage();
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

        std::vector< vk::FrameData > createFrames( uint32_t image_count );
        VkResult createSwapchain( VkSwapchainKHR* swapchain );

        vk::FrameData& getFrame( uint32_t frame_index );

        const vk::Swapchain* vk()const{ return this; }
        vk::Swapchain* vk(){ return this; }

    private:

        std::vector< vk::FrameSync > _frame_syncs;
        /** @b _frame_syncs operators on a MAX_FRAMES_IN_FLIGHT basis.
         *  thus it will be the size of MAX_FRAMES_IN_FLIGHT
         */

        std::vector< vk::FrameData > _frames;
        /** @b _frames operators on a swapchain image_count basis. thus
         *  it will be 3 if MAX_FRAMES_IN_FLIGHT = 2, 2 if MAX_FRAMES_IN_FLIGHT = 1
         */

        /** @brief Native Vulkan swapchain handle */
        VkSwapchainKHR _swapchain;

        VkExtent2D _extent;

        VkSurfaceFormatKHR _surface_format;
        VkPresentModeKHR _present_mode;
        VkFormat _depth_format;

        /** @brief Index of the currently acquired image */
        uint32_t _image_index;
        uint32_t _image_count;
        int32_t _curr_frame_index;

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
