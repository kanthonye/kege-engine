//
//  vulkan-utility.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/26/25.
//

#ifndef vulkan_utility_hpp
#define vulkan_utility_hpp

#include <vector>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <map>
#include <set>
#include <limits>
#include <string> // For device name comparison if needed, though not strictly used for selection here

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h> // For extensions like VK_KHR_portability_subset

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wdocumentation"
//#pragma clang diagnostic ignored "-Wnullability-completeness"
//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)
//#pragma clang diagnostic pop


#include "../../core/graphics-device.hpp"
#include "vulkan-resources.hpp"


namespace kege{

//    struct VulkanUtil
//    {
//        /**
//         * @brief Set up the debug messenger for validation layers
//         *
//         * Creates a debug messenger for receiving validation layer messages.
//         * Only used when validation layers are enabled.
//         *
//         * @return true on success, false otherwise
//         */
//        bool setupDebugMessenger();
//
//        /**
//         * @brief Populate debug messenger creation info
//         *
//         * Helper function to set up the debug messenger creation parameters.
//         *
//         * @param createInfo Structure to populate
//         */
//        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
//
//        /**
//         * @brief Check if requested validation layers are available
//         * @return true if all requested validation layers are supported, false otherwise
//         */
//        bool checkValidationLayerSupport();
//
//        /**
//         * @brief Get the required instance extensions
//         *
//         * Determines which Vulkan instance extensions are required based on
//         * the initialization information and platform requirements.
//         *
//         * @param info Device initialization information
//         * @return Vector of required extension names
//         */
//        std::vector<const char*> getRequiredInstanceExtensions(const kege::DeviceInitializationInfo& info);
//
//        /**
//         * @brief Check if a physical device supports the required extensions
//         *
//         * @param device Physical device to check
//         * @return true if the device supports all required extensions, false otherwise
//         */
//        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
//
//
//        VkResult allocateDeviceMemory
//        (
//            VkDevice device,
//            const VkPhysicalDeviceMemoryProperties& physical_device_memory_properties,
//            VkMemoryRequirements memory_requirements,
//            VkMemoryPropertyFlags memory_properties,
//            VkDeviceMemory* memory
//        );
//
//        /** @brief List of required device extensions */
//        std::vector<const char*> _required_device_extensions = {
//            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//            // Additional platform-specific extensions
//            #ifdef __APPLE__
//            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
//            #endif
//            // Add other conditional extensions as needed
//        };
//
//        /** @brief List of validation layers to use when enabled */
//        const std::vector<const char*> _validation_layers = {
//            "VK_LAYER_KHRONOS_validation"
//        };
//
//        /** @brief Flag indicating if validation layers are enabled */
//        bool _validation_enabled = false;
//    };

}
#endif /* vulkan_utility_hpp */
