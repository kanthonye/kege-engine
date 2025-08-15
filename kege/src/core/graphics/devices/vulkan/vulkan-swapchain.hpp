//
//  vulkan-swapchain.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#ifndef vulkan_swapchain_hpp
#define vulkan_swapchain_hpp

#include "vulkan-resources.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan swapchain resources
     *
     * Encapsulates a VkSwapchainKHR and all its associated resources,
     * including surface, images, views, and synchronization primitives.
     */
    class Swapchain
    {
    public:

        /**
         * @brief Acquire the next available swapchain image
         *
         * Retrieves the next available image from the swapchain for rendering.
         *
         * @param signal_semaphore Handle to a semaphore that will be signaled when the image is available
         * @param out_image_ndex Reference to store the index of the acquired image
         * @return true on success, false if the swapchain is out of date
         */
        bool acquireNextImage( SemaphoreHandle signal_semaphore, uint32_t* out_image_ndex );

        /**
         * @brief Queue a swapchain image for presentation
         *
         * Submits a present operation to display the rendered image.
         *
         * @param wait_semaphore Handle to a semaphore to wait on before presenting
         * @param image_ndex Index of the image to present
         * @return true on success, false if the swapchain is out of date
         */
        bool present( SemaphoreHandle wait_semaphore, uint32_t image_ndex );

        /**
         * @brief Get a texture handle for a swapchain image
         *
         * Returns a handle to the texture representing the specified swapchain image.
         *
         * @param image_ndex Index of the swapchain image
         * @return Handle to the texture representing the swapchain image
         */
        ImageHandle getColorImage(uint32_t image_ndex);
        ImageHandle getDepthImage(uint32_t image_ndex);

        VkResult create( Device* device, const SwapchainDesc& desc );

        void destroy();

    private:

        VkResult createSwapchain
        (
            Device* device,
            const kege::SwapchainDesc& desc,
            VkExtent2D extent,
            VkSurfaceFormatKHR surface_format,
            VkPresentModeKHR present_mode,
            VkSurfaceCapabilitiesKHR capabilities,
            uint32_t image_count
        );

        VkResult createSwapchainImages
        (
            Device* device,
            const kege::SwapchainDesc& desc,
            VkFormat format,
            uint32_t image_count,
            VkSwapchainKHR swapchain
        );

    private:

        /** @brief Semaphores for image acquisition synchronization */
        std::vector< kege::ImageHandle > _color_images;
        std::vector< kege::ImageHandle > _depth_images;

        /** @brief Native Vulkan swapchain handle */
        VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

        /** @brief Native Vulkan surface handle associated with this swapchain */
        VkSurfaceKHR _surface = VK_NULL_HANDLE;

        /** @brief Semaphores for image acquisition synchronization */
        std::vector< kege::SemaphoreHandle > _image_available_semaphores;

        /** @brief Original swapchain creation parameters for reference/recreation */
        kege::SwapchainDesc _desc;

        /** @brief Native format of the swapchain images */
        kege::Format _color_format;
        kege::Format _depth_format;

        /** @brief Dimensions of the swapchain images */
        kege::Extent2D _extent;

        /** @brief Index of the currently acquired image */
        uint32_t _curr_image_index = 0;

        /** @brief Flag indicating if the swapchain needs recreation (e.g., after resize) */
        bool _needs_recreation = false;

        uint32_t _image_count = 0;
        uint32_t _id;

        vk::Device* _device;
        friend Device;
    };

}
#endif /* vulkan_swapchain_hpp */
