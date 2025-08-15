//
//  vulkan-physical-device.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//

#ifndef vulkan_physical_device_hpp
#define vulkan_physical_device_hpp

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "vulkan-resources.hpp"
#include "../../core/graphics-physical-device.hpp"

namespace kege::vk{

    class Instance; // Forward declaration

    class PhysicalDevice : public kege::PhysicalDevice
    {
    public:

        bool checkDeviceExtensionSupport
        (
            VkPhysicalDevice device,
            const std::vector<const char*>& required_extensions
        );

        std::vector< VkPresentModeKHR > getPhysicalDeviceSurfacePresentModes( VkSurfaceKHR surface );
        std::vector< VkSurfaceFormatKHR > getPhysicalDeviceSurfaceFormats( VkSurfaceKHR surface );
        VkSurfaceCapabilitiesKHR getPhysicalDeviceSurfaceCapabilities( VkSurfaceKHR surface );
        
        const VkPhysicalDeviceMemoryProperties& getMemoryProperties()const;
        const VkPhysicalDeviceProperties& getProperties() const;
        const VkPhysicalDeviceFeatures& getFeatures() const;

        /**
         * @brief Check if a physical device supports the required extensions
         *
         * @param device Physical device to check
         * @return true if the device supports all required extensions, false otherwise
         */
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        /**
         * @brief Find the memory type index for the specified requirements
         *
         * Identifies a suitable memory type based on the requirements.
         *
         * @param typeFilter Bit field of allowed memory types
         * @param properties Required memory properties
         * @return Index of a suitable memory type
         */
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


        const VkPhysicalDevice getHandle()const;
        VkPhysicalDevice getHandle();

        Instance* getInstance();
        
        void operator()( vk::Instance* instance, VkPhysicalDevice device );

        PhysicalDevice();
        
    private:

        VkPhysicalDeviceMemoryProperties _memory_properties;
        VkPhysicalDeviceProperties _device_properties;
        VkPhysicalDeviceFeatures _device_features;

        VkPhysicalDevice _physical_device;
        vk::Instance* _instance;
        friend Instance;
    };

    struct VulkanDeviceFeatures2
    {
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features;
        VkPhysicalDeviceVulkan11Features features11;
        VkPhysicalDeviceVulkan12Features features12;
        VkPhysicalDeviceVulkan13Features features13;
        VkPhysicalDeviceFeatures2 features2;
    };
    void getRequiredFeatures( VkPhysicalDevice physical_device, VulkanDeviceFeatures2& feature );
}
#endif /* vulkan_physical_device_hpp */
