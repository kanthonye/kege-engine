//
//  vulkan-instance.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef kege_vulkan_instance_hpp
#define kege_vulkan_instance_hpp

#include "vk-device.hpp"
#include "vk-physical-device.hpp"

namespace kege::vk{

    /**
     * @brief Create the Vulkan instance
     *
     * Sets up the Vulkan instance with required extensions and validation layers.
     */
    class Instance final : public kege::GraphicsInstance
    {
    public:

        void setSurface( );
        
        /**
         * @brief Check if a physical device is suitable for use
         *
         * Evaluates whether a physical device meets all requirements.
         *
         * @param info The DeviceInitializationInfo with the requirements to check for
         * @return a pointer to the physical device, nullptr otherwise
         */
        PhysicalDevice* getBestSuitablePhysicalDevice
        ( const DeviceInitializationInfo& info )override;

        PhysicalDevice* getPhysicalDevice( uint32_t physical_device_index )override;
        uint32_t getPhysicalDeviceCount()override;
        void listPhysicalDevicesInfo()override;


        GraphicsDevice* createDevice( kege::PhysicalDevice* physical_device )override;
        void destroyDevice( GraphicsDevice* device )override;

        /**
         * @brief Get the required instance extensions
         *
         * Determines which Vulkan instance extensions are required based on
         * the initialization information and platform requirements.
         *
         * @return Vector of required extension names
         */
        const std::vector< const char*>& getRequiredDeviceExtensions()const;

        std::vector< PhysicalDevice >& getPhysicalDevices();

        bool initalize( const DeviceInitializationInfo& info )override;
        void shutdown()override;

        const std::vector<const char*>& getValidationLayers()const;
        bool isValidationEnabled()const;

        bool checkValidationLayerSupport();

        GraphicsAPI getGraphicsAPI()override;
        VkInstance getHandle();

        ~Instance()override;
        Instance();

    private:

        static VkBool32 generalDebugCallback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        static VkBool32 performanceDebugCallback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        static VkBool32 validationDebugCallback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        bool getSuitablePhysicalDevices( const DeviceInitializationInfo& info );
        bool createInstance( const DeviceInitializationInfo& info );

        bool checkDeviceExtensionSupport( VkPhysicalDevice device );
        bool checkRayTracingSupport( VkPhysicalDevice device );

    private:

        List< vk::Device > _devices;
        std::vector< PhysicalDevice > _physical_devices;
        //ResourceRecycler< Ref< Device > > _devices;

        /** @brief List of required device extensions */
        std::vector<const char*> _required_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            // Additional platform-specific extensions
            #ifdef __APPLE__
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
            #endif
            // Add other conditional extensions as needed
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        /** @brief List of validation layers to use when enabled */
        const std::vector<const char*> _validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector< VkDebugUtilsMessengerEXT > _debug_messengers;
        VkSurfaceKHR _surface;
        VkInstance _instance;

        /** @brief Flag indicating if validation layers are enabled */
        bool _enable_debug_validation;
        bool _enable_debug_performance;
        bool _enable_debug_general;

        uint32_t _best_physical_device_index;

        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger;
        static Instance* _self;
        friend Swapchain;
    };

}
#endif /* kege_vulkan_instance_hpp */
