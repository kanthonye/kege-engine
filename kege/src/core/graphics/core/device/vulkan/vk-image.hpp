//
//  vk-image.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef vk_image_hpp
#define vk_image_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan image resources
     *
     * Encapsulates a VkImage along with its default view, memory allocation,
     * and current layout state tracking.
     */
    class Image : public  kege::Image
    {
    public:


        /** @brief The image's memory */
        const VkDeviceMemory& memory()const{ return _memory; }

        /** @brief Default image view for the texture */
        const VkImageView& view()const{ return _view; }

        /** @brief Native Vulkan image handle */
        const VkImage& handle()const{ return _image; }

        const vk::Image* vk() const { return this; }
        vk::Image* vk() { return this; }

        Image( vk::Device* device, const kege::ImageDesc& desc );
        Image();
        ~Image();


        /** @brief Native Vulkan image handle */
        VkImage _image;

        /** @brief Default image view for the texture */
        VkImageView _view;

        /** @brief The image's memory */
        VkDeviceMemory _memory;

        /** @brief The image's format */
        VkFormat _vkformat;

        /** @brief Current layout of the image for synchronization tracking */
        VkImageLayout _current_layout;

        VkImageAspectFlags _aspect;

        vk::Device* _device;

        bool _swapchain_owned;
        friend List< vk::Image >;
        vk::Image* prev;
        vk::Image* next;
    };

    /**
     * @brief Wrapper for Vulkan sampler resources
     *
     * Encapsulates a VkSampler along with its creation parameters.
     */
    class Sampler : public kege::Sampler
    {
    public:

        const VkSampler& handle()const{ return _sampler; }

        const vk::Sampler* vk() const { return this; }
        vk::Sampler* vk() { return this; }

        Sampler( vk::Device* device, const kege::SamplerDesc& desc );
        ~Sampler();

        /** @brief Native Vulkan sampler handle */
        VkSampler _sampler;

        vk::Device* _device;

        friend List< vk::Sampler >;
        vk::Sampler* prev;
        vk::Sampler* next;
    };

}
#endif /* vk_image_hpp */
