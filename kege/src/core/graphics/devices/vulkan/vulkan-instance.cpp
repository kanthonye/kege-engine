//
//  vulkan-instance.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#include "vulkan-device.hpp"
#include "vulkan-instance.hpp"

namespace kege::vk{

    Instance* Instance::_self = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT createMessengerInfo
    (
        VkDebugUtilsMessageSeverityFlagsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        PFN_vkDebugUtilsMessengerCallbackEXT callback
    )
    {
        VkDebugUtilsMessengerCreateInfoEXT info{};
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = severity;
        info.messageType = type;
        info.pfnUserCallback = callback;
        info.pUserData = nullptr;
        return info;
    }

    const std::vector< const char* >& Instance::getRequiredDeviceExtensions()const
    {
        return _required_device_extensions;
    }

    GraphicsDevice* Instance::createDevice( kege::PhysicalDevice* physical_device, kege::GraphicsSurface surface )
    {
        PhysicalDevice* vk_physical_device = static_cast< PhysicalDevice* >( physical_device );
        if ( vk_physical_device == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid physical_device pointer in Instance::createDevice()." <<Log::nl;
            return nullptr;
        }

        VkSurfaceKHR vk_surface = static_cast< VkSurfaceKHR >( surface );
        if ( vk_surface == nullptr )
        {
            KEGE_LOG_ERROR << "Invalid surface in Instance::createDevice()." <<Log::nl;
            return nullptr;
        }

        uint32_t device_id = _devices.gen();
        Ref< Device >* device = _devices.get( device_id );
        if ( !device )
        {
            KEGE_LOG_ERROR << "Device creation failed in Instance::createDevice()." <<Log::nl;
            return nullptr;
        }

        (*device) = new Device;
        if ( !(*device)->initialize( vk_physical_device, vk_surface ) )
        {
            KEGE_LOG_ERROR << "Device initalization failed in Instance::createDevice()." <<Log::nl;
            (*device)->shutdown();
            (*device).clear();
            _devices.free( device_id );
            return nullptr;
        }
        (*device)->_id = device_id;
        return device->ref();
    }

    void Instance::destroyDevice( GraphicsDevice* device )
    {
        if ( device )
        {
            Ref< Device >& ref = *_devices.get( device->id() );
            ref->shutdown();

            _devices.free( device->id() );
            ref.clear();
        }
    }

    std::vector< PhysicalDevice >& Instance::getPhysicalDevices()
    {
        return _physical_devices;
    }

    bool Instance::initalize( const DeviceInitializationInfo& info )
    {
        if ( createInstance( info ) )
        {
            uint32_t device_count = 0;
            vkEnumeratePhysicalDevices( _instance, &device_count, nullptr);
            if ( device_count == 0 )
            {
                KEGE_LOG_ERROR << "Failed to find GPUs with Vulkan support!" <<Log::nl;
                return false;
            }
            std::vector< VkPhysicalDevice > devices( device_count );
            vkEnumeratePhysicalDevices( _instance, &device_count, devices.data() );
            KEGE_LOG_INFO << "Found " << device_count << " Vulkan capable device(s):" <<Log::nl;

            _physical_devices.resize( device_count );
            for ( int i=0; i<device_count; ++i )
            {
                _physical_devices[i].operator()( this, devices[i] );
            }
        }
        return true;
    }

    PhysicalDevice* Instance::getBestSuitablePhysicalDevice
    (
        const DeviceInitializationInfo& info,
        kege::GraphicsSurface surface
    )
    {
        // Define weights for scoring preferences (adjust as needed)
        const int SCORE_WEIGHT_DISCRETE_GPU = 4000;
        const int SCORE_WEIGHT_INTEGRATED_GPU = 500; // Base score if not discrete
        const int SCORE_WEIGHT_PER_GB_VRAM = 25;    // Score per GB of dedicated VRAM
        const int SCORE_WEIGHT_SHADER_FLOAT64 = 1000;
        const int SCORE_WEIGHT_GEOMETRY_SHADER = 30;
        const int SCORE_WEIGHT_TESSELLATION_SHADER = 60;
        const int SCORE_WEIGHT_COMPUTE_INVOCATIONS = 1; // Score per 128 max invocations
        const int SCORE_WEIGHT_API_VERSION_MAJOR = 200;
        const int SCORE_WEIGHT_API_VERSION_MINOR = 20;

        int32_t best_device = -1;
        int32_t best_preference_score = 0;
        VkSurfaceKHR vk_surface = static_cast< VkSurfaceKHR >( surface );
        for ( int device_index = 0; device_index < _physical_devices.size(); ++device_index )
        {
            PhysicalDevice& device = _physical_devices[ device_index ];
            uint32_t preference_score = 0;

            KEGE_LOG_INFO << "Evaluating: " << device._device_properties.deviceName;
            KEGE_LOG_INFO << " (API: " << VK_API_VERSION_MAJOR( device._device_properties.apiVersion ) << "."
                     << VK_API_VERSION_MINOR( device._device_properties.apiVersion ) << "."
                     << VK_API_VERSION_PATCH( device._device_properties.apiVersion )
                     << ", Type: " << device._device_properties.deviceType << ")" <<Log::nl;

            // --- 1. Minimum Requirements & Feature Checks ---

            // Check Minimum Feature Requirements Here!
            // Example: Must support dynamic rendering
            if ( !device._features.dynamic_rendering )
            {
                KEGE_LOG_INFO <<"    - Reason: Does not support required Dynamic Rendering feature.\n";
                return nullptr;
            }

            if ( !device._features.sampler_anisotropy )
            {
                KEGE_LOG_INFO << "    - Reason: Does not support required Sampler Anisotropy feature.\n";
                return nullptr;
            }

            // Queue Families
            QueueFamilyIndices queue_family_indices = findQueueFamilies( device.getHandle(), vk_surface );
            if ( !queue_family_indices.isComplete() )
            {
                KEGE_LOG_INFO << "    - Skipping: Missing required queue families.\n";
                 return nullptr;
            }

            // Required Core Features Check
            if ( info.require_shader_float64 && !device._device_features.shaderFloat64 )
            {
                KEGE_LOG_INFO << "    - Skipping: Missing required feature: shaderFloat64\n";
                return nullptr;
            }

            if ( info.require_geometry_shader && !device._device_features.geometryShader )
            {
                KEGE_LOG_INFO << "    - Skipping: Missing required feature: geometryShader\n";
                return nullptr;
            }

            if (info.require_tessellation_shader && !device._device_features.tessellationShader)
            {
                KEGE_LOG_INFO << "    - Skipping: Missing required feature: tessellationShader\n";
                return nullptr;
            }

            // Add more required core feature checks here...
            // Ray Tracing Check (if required)
            if ( info.enable_raytracing )
            {
                // Note: Requires VK_KHR_get_physical_device_properties2 INSTANCE extension enabled!
                const std::vector<const char*> rtExtensions =
                {
                    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME
                };

                if ( !checkDeviceExtensionSupport( device._physical_device ) )
                {
                    KEGE_LOG_INFO << "    - Skipping: Missing required ray tracing extensions.\n";
                    return nullptr;
                }

                if ( !checkRayTracingSupport( device._physical_device ) )
                { // Checks actual features
                    KEGE_LOG_INFO << "    - Skipping: Missing required ray tracing features.\n";
                    return nullptr;
                }
                KEGE_LOG_INFO << "    - Ray Tracing Supported (Required).\n";
            }

            // Minimum Dedicated VRAM Check
            device._dedicated_video_memory_mb = 0;
            for (uint32_t i = 0; i < device._memory_properties.memoryHeapCount; ++i)
            {
                if (( device._memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ))
                {
                     // Simplification: sum all device-local heaps. A more complex check
                     // could verify the types using this heap are primarily device-local.
                    device._dedicated_video_memory_mb += device._memory_properties.memoryHeaps[i].size;
                }
            }

            uint32_t dedicated_vram_gb = static_cast<uint32_t>( device._dedicated_video_memory_mb / (1000 * 1000 * 1000) );
            if ( info.min_dedicated_video_memory > 0 && dedicated_vram_gb < info.min_dedicated_video_memory )
            {
                KEGE_LOG_INFO << "    - Skipping: Insufficient dedicated VRAM ("
                         << dedicated_vram_gb << "MB available, "
                         << info.min_dedicated_video_memory << "-GB required).\n";
                return nullptr;
            }
            KEGE_LOG_INFO << "    - Dedicated VRAM: " << dedicated_vram_gb << "-GB\n";

            // --- 2. Scoring Based on Preferences ---
            preference_score = 0;

            // Device Type Preference
            if ( info.prefer_discrete_gpu )
            {
                if ( device._device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
                {
                    preference_score += SCORE_WEIGHT_DISCRETE_GPU;
                    KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_DISCRETE_GPU << " (Discrete GPU preferred and found)\n";
                }
                else if ( device._device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
                {
                     // Give integrated GPU a lower base score if discrete is preferred but not found
                    preference_score += SCORE_WEIGHT_INTEGRATED_GPU / 2;
                    KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_INTEGRATED_GPU / 2 << " (Integrated GPU, Discrete preferred)\n";
                }
            }
            else
            {
                 // If discrete is not specifically preferred, still score it higher than integrated
                 if ( device._device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
                 {
                     preference_score += SCORE_WEIGHT_DISCRETE_GPU / 2; // Lower score if not explicitly preferred
                     KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_DISCRETE_GPU / 2 << " (Discrete GPU)\n";
                 } else if ( device._device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
                 {
                     preference_score += SCORE_WEIGHT_INTEGRATED_GPU;
                     KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_INTEGRATED_GPU << " (Integrated GPU)\n";
                 }
            }

            // Other device types (Virtual, CPU) get low/no score based on this criterion
            // VRAM Score (Maximize)
            if ( info.prefer_max_vram )
            {
                int vram_score = dedicated_vram_gb * SCORE_WEIGHT_PER_GB_VRAM;
                preference_score += vram_score;
                KEGE_LOG_INFO << "    - Score +" << vram_score << " (VRAM Preference: " << dedicated_vram_gb << " MB)\n";
            }

            // Preferred Features Score
            if ( info.prefer_shader_float64 && device._device_features.shaderFloat64 )
            {
                preference_score += SCORE_WEIGHT_SHADER_FLOAT64;
                KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_SHADER_FLOAT64 << " (Preferred feature: shaderFloat64)\n";
            }

            if ( info.prefer_geometry_shader && device._device_features.geometryShader )
            {
                preference_score += SCORE_WEIGHT_GEOMETRY_SHADER;
                KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_GEOMETRY_SHADER << " (Preferred feature: geometryShader)\n";
            }

            if ( info.prefer_tessellation_shader && device._device_features.tessellationShader )
            {
                preference_score += SCORE_WEIGHT_TESSELLATION_SHADER;
                KEGE_LOG_INFO << "    - Score +" << SCORE_WEIGHT_TESSELLATION_SHADER << " (Preferred feature: tessellationShader)\n";
            }

            // Add scoring for other preferred features...
            // "Processing Power" Proxies Score
            if ( info.prefer_high_compute_limits )
            {
                // Using maxComputeWorkGroupInvocations as a proxy
                int compute_score = ( device._device_properties.limits.maxComputeWorkGroupInvocations / 128 ) * SCORE_WEIGHT_COMPUTE_INVOCATIONS;
                preference_score += compute_score;
                KEGE_LOG_INFO << "    - Score +" << compute_score << " (Compute Limit Preference: "
                         << device._device_properties.limits.maxComputeWorkGroupInvocations << " max invocations)\n";
                // Could also add scoring based on maxComputeWorkGroupSize[0] etc.
            }

            // API Version Score
            if ( info.prefer_higher_api_version )
            {
                int api_score = VK_API_VERSION_MAJOR( device._device_properties.apiVersion ) * SCORE_WEIGHT_API_VERSION_MAJOR +
                                VK_API_VERSION_MINOR( device._device_properties.apiVersion ) * SCORE_WEIGHT_API_VERSION_MINOR;
                preference_score += api_score;

                KEGE_LOG_INFO << "    - Score +" << api_score << " (API Version Preference: "
                         << VK_API_VERSION_MAJOR( device._device_properties.apiVersion ) << "."
                         << VK_API_VERSION_MINOR( device._device_properties.apiVersion ) << ")\n";
            }
            KEGE_LOG_INFO << "    - total score: " << preference_score <<Log::nl;

            if (best_preference_score < preference_score)
            {
                best_preference_score = preference_score;
                best_device = device_index;
            }
        }

        return &_physical_devices[ best_device ];
    }

    PhysicalDevice* Instance::getPhysicalDevice( uint32_t physical_device_index )
    {
        return &_physical_devices[ physical_device_index ];
    }

    uint32_t Instance::getPhysicalDeviceCount()
    {
        return static_cast<int>( _physical_devices.size() );
    }

    void Instance::listPhysicalDevicesInfo()
    {
    }

    bool Instance::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> available_extensionSet;
        for (const auto& extension : available_extensions)
        {
            available_extensionSet.insert(extension.extensionName);
        }

        for (const char* requiredExt : _required_device_extensions)
        {
            if (available_extensionSet.find(requiredExt) == available_extensionSet.end())
            {
                std::cerr << "    - Missing required device extension: " << requiredExt << "\n";
                return false;
            }
        }

        return true;
    }

    // Requires VK_KHR_get_physical_device_properties2 instance extension to be enabled!
    bool Instance::checkRayTracingSupport( VkPhysicalDevice device )
    {
        // Check if the function pointer is available (depends on instance extension)
        PFN_vkGetPhysicalDeviceFeatures2KHR pfnGetPhysicalDeviceFeatures2KHR =
        (PFN_vkGetPhysicalDeviceFeatures2KHR)vkGetInstanceProcAddr(_instance, "vkGetPhysicalDeviceFeatures2KHR");
        // Fallback for core Vulkan 1.1 version
        PFN_vkGetPhysicalDeviceFeatures2 pfnGetPhysicalDeviceFeatures2 =
        (PFN_vkGetPhysicalDeviceFeatures2)vkGetInstanceProcAddr(_instance, "vkGetPhysicalDeviceFeatures2");

        if (!pfnGetPhysicalDeviceFeatures2KHR && !pfnGetPhysicalDeviceFeatures2)
        {
            KEGE_LOG_WARN << "    - Cannot check RT features. vkGetPhysicalDeviceFeatures2(KHR) not found."
            " Make sure VK_KHR_get_physical_device_properties2 instance extension is enabled.\n";
            // Tentatively return true if extensions were found, but features are unknown.
            // A stricter implementation might return false here.
            return true; // Or false, depending on desired strictness
        }

        // Prepare structures to query RT features
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures = {};
        accelFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR pipeline_features = {};
        pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        pipeline_features.pNext = &accelFeatures; // Chain the structures

        // Use VkPhysicalDeviceFeatures2 to query
        VkPhysicalDeviceFeatures2 device_features2 = {};
        device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        device_features2.pNext = &pipeline_features; // Head of the chain

        if(pfnGetPhysicalDeviceFeatures2KHR)
            pfnGetPhysicalDeviceFeatures2KHR(device, &device_features2);
        else if (pfnGetPhysicalDeviceFeatures2) // Vulkan 1.1+ core version
            pfnGetPhysicalDeviceFeatures2(device, &device_features2);


        // Check the specific features required
        bool hasSupport = true;
        if (!accelFeatures.accelerationStructure)
        {
            KEGE_LOG_ERROR << "    - Missing feature: accelerationStructure\n";
            hasSupport = false;
        }
        if (!pipeline_features.rayTracingPipeline)
        {
            KEGE_LOG_ERROR << "    - Missing feature: rayTracingPipeline\n";
            hasSupport = false;
        }
        // Add checks for other RT features if needed (e.g., rayQuery, rayTraversalPrimitiveCulling)

        return hasSupport;
    }

    bool Instance::createInstance( const DeviceInitializationInfo& info )
    {
        bool enable_validation = info.request_validation;
        if ( enable_validation && !checkValidationLayerSupport())
        {
            KEGE_LOG_WARN << "Validation layers requested but not fully supported. Disabling validation."<<Log::nl;
            enable_validation = false; // Proceed without validation? Or fail?
        }
        _validation_enabled = enable_validation;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = info.name.data();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = info.engine.data();
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4; // Or 1.1, 1.3 depending on features used

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &appInfo;

        std::vector< const char* > required_extensions = info.window->getRequiredInstanceExtensions();
        // --- Add Portability Extension for MoltenVK/macOS ---
        #ifdef __APPLE__
            required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            // This flag MUST be set on macOS for MoltenVK >= 1.3.216 if portability is used
            // Handle VK_KHR_portability_enumeration (needed for MoltenVK on macOS/iOS)
            create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            KEGE_LOG_INFO << "Enabling Portability Enumeration for macOS.\n";
        #endif

        VkDebugUtilsMessengerCreateInfoEXT general_messenger_info{};
        VkDebugUtilsMessengerCreateInfoEXT performance_messenger_info{};
        VkDebugUtilsMessengerCreateInfoEXT validation_messenger_info{};
        if ( _validation_enabled )
        {
            // --- Add Debug Utils Extension IF validation is enabled ---
            required_extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
            KEGE_LOG_INFO << "Enabling VK_EXT_debug_utils extension for validation.\n";

            create_info.enabledLayerCount = static_cast<uint32_t>( _validation_layers.size() );
            create_info.ppEnabledLayerNames = _validation_layers.data();

            general_messenger_info = createMessengerInfo
            (
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                generalDebugCallback // your function
            );

            performance_messenger_info = createMessengerInfo
            (
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                performanceDebugCallback // your function
            );

            validation_messenger_info = createMessengerInfo
            (
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                validationDebugCallback // your function
            );

            general_messenger_info.pNext = &performance_messenger_info;
            performance_messenger_info.pNext = &validation_messenger_info;
            create_info.pNext = &general_messenger_info;
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        create_info.ppEnabledExtensionNames = required_extensions.data();

        if ( vkCreateInstance( &create_info, nullptr, &_instance ) != VK_SUCCESS)
        {
            KEGE_LOG_ERROR << "Failed to create Vulkan instance!" <<Log::nl;
            return false;
        }

        if ( _validation_enabled )
        {
            auto vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr( _instance, "vkCreateDebugUtilsMessengerEXT" );
            if ( vkCreateDebugUtilsMessenger != nullptr )
            {
                _debug_messengers.resize(3);
                vkCreateDebugUtilsMessenger( _instance, &general_messenger_info, nullptr, &_debug_messengers[0] );
                vkCreateDebugUtilsMessenger( _instance, &performance_messenger_info, nullptr, &_debug_messengers[1] );
                vkCreateDebugUtilsMessenger( _instance, &performance_messenger_info, nullptr, &_debug_messengers[2] );
            }

            vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr
            ( _instance, "vkDestroyDebugUtilsMessengerEXT" );
        }
        return true;
    }

    bool Instance::checkValidationLayerSupport()
    {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties( &layer_count, nullptr );

        if (layer_count == 0)
            return false;

        std::vector< VkLayerProperties > available_layers( layer_count );
        vkEnumerateInstanceLayerProperties( &layer_count, available_layers.data() );

        std::map< std::string, VkLayerProperties > layers;
        for ( const VkLayerProperties& layer : available_layers )
        {
            layers[ layer.layerName ] = layer;
        }

        for ( const char* layer_name : _validation_layers )
        {
            if ( layers.find( layer_name ) == layers.end() )
                return false;
        }
        return true;
    }

    const std::vector<const char*>& Instance::getValidationLayers()const
    {
        return _validation_layers;
    }

    bool Instance::isValidationEnabled()const
    {
        return _validation_enabled;
    }

    VkInstance Instance::getHandle()
    {
        return _instance;
    }

    void Instance::shutdown()
    {
        for ( int32_t i = 0; i < _devices.count(); ++i )
        {
            if ( _devices.get( i ) != nullptr )
            {
                (*_devices.get( i ))->shutdown();
                (*_devices.get( i )).clear();
            };
        }
        _devices.clear();

        if ( vkDestroyDebugUtilsMessenger != nullptr )
        {
            for (int i=0; i<_debug_messengers.size(); ++i)
            {
                if (_validation_enabled && _debug_messengers[i] != VK_NULL_HANDLE)
                {
                    vkDestroyDebugUtilsMessenger( _instance, _debug_messengers[i], nullptr );
                    _debug_messengers[i] = VK_NULL_HANDLE;
                }
            }
            _debug_messengers.clear();
            vkDestroyDebugUtilsMessenger = nullptr;
        }

        if ( _instance != VK_NULL_HANDLE )
        {
            vkDestroyInstance( _instance, nullptr );
            _instance = VK_NULL_HANDLE;
        }
    }

    Instance::~Instance()
    {
        shutdown();
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::generalDebugCallback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* pUserData
    )
    {
        switch ( message_severity )
        {
            /**
             * Indicates messages with the lowest severity level. These
             * messages provide detailed information for debugging but
             * are not typically relevant for most development scenarios.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                //kegeLog[ "general" ] << callback_data->pMessage;
                break;
            }

            /**
             * Indicates informational messages that can help you understand
             * the behavior of the Vulkan API and your application.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                //kegeLog[ "info" ][ "general" ] << callback_data->pMessage;
                break;
            }

            /**
             * Indicates warning messages that point out potential issues that
             * could lead to unexpected behavior or suboptimal performance.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                //kegeLog[ "warning" ][ "general" ] << callback_data->pMessage <<"\n";
                break;
            }

            /**
             * Indicates error messages that signal problems in your code
             * that need to be addressed. These messages often indicate
             * incorrect API usage or other issues that might cause your
             * application to misbehave or crash.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                //kegeLog[ "error" ][ "general" ] << callback_data->pMessage <<"\n";
                break;
            }

            default: break;
        }
        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::performanceDebugCallback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* pUserData
    )
    {
        switch ( message_severity )
        {
            /**
             * Indicates messages with the lowest severity level. These
             * messages provide detailed information for debugging but
             * are not typically relevant for most development scenarios.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                KEGE_LOG_DEBUG << callback_data->pMessage;
                break;
            }

            /**
             * Indicates informational messages that can help you understand
             * the behavior of the Vulkan API and your application.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                KEGE_LOG_INFO << callback_data->pMessage <<Log::nl;
                break;
            }

            /**
             * Indicates warning messages that point out potential issues that
             * could lead to unexpected behavior or suboptimal performance.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                KEGE_LOG_WARN << callback_data->pMessage <<"\n";
                break;
            }

            /**
             * Indicates error messages that signal problems in your code
             * that need to be addressed. These messages often indicate
             * incorrect API usage or other issues that might cause your
             * application to misbehave or crash.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                KEGE_LOG_ERROR << callback_data->pMessage <<"\n";
                break;
            }

            default: break;
        }
        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::validationDebugCallback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* pUserData
    )
    {
        switch ( message_severity )
        {
            /**
             * Indicates messages with the lowest severity level. These
             * messages provide detailed information for debugging but
             * are not typically relevant for most development scenarios.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                KEGE_LOG_DEBUG << callback_data->pMessage;
                break;
            }

            /**
             * Indicates informational messages that can help you understand
             * the behavior of the Vulkan API and your application.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                KEGE_LOG_INFO << callback_data->pMessage;
                break;
            }

            /**
             * Indicates warning messages that point out potential issues that
             * could lead to unexpected behavior or suboptimal performance.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                KEGE_LOG_WARN << callback_data->pMessage <<"\n";
                break;
            }

            /**
             * Indicates error messages that signal problems in your code
             * that need to be addressed. These messages often indicate
             * incorrect API usage or other issues that might cause your
             * application to misbehave or crash.
             */
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                KEGE_LOG_ERROR << callback_data->pMessage <<"\n";
                break;
            }

            default: break;
        }
        return VK_FALSE;
    }

    GraphicsAPI Instance::getGraphicsAPI()
    {
        return GraphicsAPI::Vulkan;
    }

    Instance::Instance()
    :   _surface( nullptr )
    ,   _instance( VK_NULL_HANDLE )
    ,   _best_physical_device_index( 0 )
    {
        _self = this;
    }
    
}
