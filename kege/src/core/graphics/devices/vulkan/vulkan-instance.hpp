//
//  vulkan-instance.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef vulkan_instance_hpp
#define vulkan_instance_hpp

#include "../../core/graphics-instance.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"

namespace kege::vk{

    /**
     * @brief Create the Vulkan instance
     *
     * Sets up the Vulkan instance with required extensions and validation layers.
     */
    class Instance final : public GraphicsInstance
    {
    public:

        /**
         * @brief Check if a physical device is suitable for use
         *
         * Evaluates whether a physical device meets all requirements.
         *
         * @param info The DeviceInitializationInfo with the requirements to check for
         * @param surface The surface for present capabilities check
         * @return a pointer to the physical device, nullptr otherwise
         */
        PhysicalDevice* getBestSuitablePhysicalDevice
        (
            const DeviceInitializationInfo& info,
            kege::GraphicsSurface surface
        )
        override;

        PhysicalDevice* getPhysicalDevice( uint32_t physical_device_index )override;
        uint32_t getPhysicalDeviceCount()override;
        void listPhysicalDevicesInfo()override;


        GraphicsDevice* createDevice( kege::PhysicalDevice* physical_device, kege::GraphicsSurface surface )override;
        void destroyDevice( GraphicsDevice* device )override;

        inline void destroySurface( VkSurfaceKHR surface, const VkAllocationCallbacks* allocator )
        {
            vkDestroySurfaceKHR( _instance, surface, allocator );
        }

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

        std::vector< PhysicalDevice > _physical_devices;
        ResourceRecycler< Ref< Device > > _devices;

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
        bool _validation_enabled = false;

        uint32_t _best_physical_device_index;

        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger;
        static Instance* _self;
        friend Swapchain;
    };

}
#endif /* vulkan_instance_hpp */
