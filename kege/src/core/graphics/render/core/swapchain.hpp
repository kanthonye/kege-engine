//
//  graphics-swapchain.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/3/25.
//

#ifndef kege_swapchain_hpp
#define kege_swapchain_hpp

#include "image.hpp"

namespace kege{

    /**
     * @brief Describes the configuration of a swapchain
     *
     * A swapchain manages the presentation of rendered images to a display surface.
     * It handles the queuing and display of frame buffers in a manner appropriate
     * for the platform and selected presentation mode.
     */
    struct SwapchainDesc
    {
        //GraphicsSurface surface = nullptr;

        /**
         * @brief Queue type responsible for presenting frames to screen
         *
         * Most commonly Graphics, but can be set to Present on platforms
         * that support dedicated presentation queues.
         */
        QueueType present_queue_type = QueueType::Graphics;

        /**
         * @brief Width of the swapchain images in pixels
         *
         * Should match window client area dimensions or desired back buffer size.
         */
        uint32_t width = 0;

        /**
         * @brief Height of the swapchain images in pixels
         *
         * Should match window client area dimensions or desired back buffer size.
         */
        uint32_t height = 0;

        /**
         * @brief Number of images in the swapchain queue
         *
         * Common configurations:
         * - 2: Double buffering (one displayed, one being rendered)
         * - 3: Triple buffering (reduced chance of GPU stalls, may increase latency)
         *
         * Higher values can improve throughput at the cost of memory.
         */
        uint32_t image_count = 2;

        /**
         * @brief Pixel format used for swapchain images
         *
         * Common formats:
         * - BGRA8_UNORM_SRGB: 8-bit per channel with sRGB encoding (Windows default)
         * - RGBA8_UNORM_SRGB: 8-bit per channel with sRGB encoding (common on other platforms)
         *
         * Applications should query platform support before finalizing.
         */
        Format color_format = Format::bgra_u8_norm_srbg;

        /**
         * @brief Pixel format used for swapchain images
         *
         * Common formats:
         * - BGRA8_UNORM_SRGB: 8-bit per channel with sRGB encoding (Windows default)
         * - RGBA8_UNORM_SRGB: 8-bit per channel with sRGB encoding (common on other platforms)
         *
         * Applications should query platform support before finalizing.
         */
        Format depth_format = Format::depth_32;

        /**
         * @brief Color space specification for the swapchain
         *
         * Defines how color values are interpreted (sRGB, HDR, etc.)
         * Currently commented out - would need proper enum definition.
         */
        // ColorSpace color_space = ColorSpace::SRGB;

        /**
         * @brief Selected presentation mode for this swapchain
         *
         * Choose based on application needs:
         * - Competitive games: Mailbox for low latency when GPU can maintain framerate
         * - General applications: Fifo for power efficiency and tear-free display
         * - High performance: Immediate for maximum throughput regardless of tearing
         */
        PresentMode present_mode = PresentMode::Fifo;

        /**
         * @brief Flags indicating how swapchain images will be used
         *
         * Common flags:
         * - Color: Required for rendering to swapchain images
         * - CopyDst: Enables copy operations to swapchain images (e.g., for screenshots)
         * - TransferSrc: Enables reading from swapchain images (e.g., for post-processing)
         */
        ImageUsage image_usage = ImageUsage::Color | ImageUsage::TransferDst;

        /**
         * @brief Previous swapchain to recover resources from when recreating
         *
         * Used when resizing window or changing swapchain properties.
         * Helps prevent resource churn and enables smoother transitions.
         */
        //Swapchain old_swapchain;

        /**
         * @brief Debug identifier for graphics debugging tools
         *
         * Provides a meaningful name in graphics debuggers and profilers.
         */
        std::string name = "";
    };

    
    /**
     * @brief Wrapper for Vulkan swapchain resources
     *
     * Encapsulates a VkSwapchainKHR and all its associated resources,
     * including surface, images, views, and synchronization primitives.
     */
    class Swapchain : public kege::RefCounter
    {
    public:

        virtual const ref::Semaphore& getImageAvailableSemaphore( uint32_t image_index )const = 0;
        virtual const ref::Semaphore& getRenderFinishSemaphore( uint32_t image_index )const = 0;

        virtual std::vector< cref::Image > getColorImages()const = 0;
        virtual std::vector< cref::Image > getDepthImages()const = 0;
        virtual std::vector< ref::Image > getColorImages() = 0;
        virtual std::vector< ref::Image > getDepthImages() = 0;
        virtual kege::Format getDepthFormat()const = 0;
        virtual kege::Format getColorFormat()const = 0;
        virtual uint32_t getImageCount()const = 0;
        virtual uint32_t getImageIndex()const = 0;
        virtual kege::Extent2D getExtent()const = 0;
        virtual kege::Viewport getViewport()const = 0;
        virtual kege::Scissor getScissor()const = 0;

        virtual int32_t acquireNextImage() = 0;
        
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
        //friend vk::Device;
    };

}

#endif /* kege_swapchain_hpp */
