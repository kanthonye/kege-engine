//
//  vulkan-physical-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//
#include <map>
#include "vulkan-instance.hpp"
#include "vulkan-physical-device.hpp"

namespace kege::vk{

    void chain( void** next, VkPhysicalDeviceVulkan11Features& feature )
    {
        *next = &feature;
        next = &feature.pNext;
    }

    void PhysicalDevice::operator()( vk::Instance* instance, VkPhysicalDevice device )
    {
        _instance = instance;
        _physical_device = device;
        vkGetPhysicalDeviceProperties( _physical_device, &_device_properties );
        vkGetPhysicalDeviceFeatures( _physical_device, &_device_features );
        vkGetPhysicalDeviceMemoryProperties( _physical_device, &_memory_properties );

        VulkanDeviceFeatures2 feature;
        getRequiredFeatures( _physical_device, feature );

        _device_id = _device_properties.deviceID;
        _name = _device_properties.deviceName;
        _api_version = _device_properties.apiVersion;
        _device_type = convertPhysicalDeviceType( _device_properties.deviceType );
        _driver_version = _device_properties.driverVersion;

        _features.independent_blend = _device_features.independentBlend;
        _features.geometry_shader = _device_features.geometryShader;
        _features.tessellation_shader = _device_features.tessellationShader;
        _features.multi_viewport = _device_features.multiViewport;
        _features.texture_compression_bc = _device_features.textureCompressionBC;
        _features.texture_compression_etc2 = _device_features.textureCompressionETC2;
        _features.texture_compression_astc_ldr = _device_features.textureCompressionASTC_LDR;
        _features.sampler_anisotropy = _device_features.samplerAnisotropy;
        _features.depth_clamp = _device_features.depthClamp;
        _features.depth_bias_clamp = _device_features.depthBiasClamp;
        _features.fill_mode_non_solid = _device_features.fillModeNonSolid;
        _features.wide_lines = _device_features.wideLines;
        _features.large_points = _device_features.largePoints;
        _features.shader_float64 = _device_features.shaderFloat64;
        _features.shader_int64 = _device_features.shaderInt64;
        _features.pipeline_statistics_query = _device_features.pipelineStatisticsQuery;
        _features.dynamic_rendering = feature.dynamic_rendering_features.dynamicRendering;

        _limits.min_uniform_buffer_offset_alignment = _device_properties.limits.minUniformBufferOffsetAlignment;
        _limits.min_storage_buffer_offset_alignment = _device_properties.limits.minStorageBufferOffsetAlignment;
        _limits.framebuffer_color_sample_counts = (SampleCount) _device_properties.limits.framebufferColorSampleCounts;
        _limits.framebuffer_depth_sample_counts = (SampleCount) _device_properties.limits.framebufferDepthSampleCounts;
        _limits.max_image_dimension_1d = _device_properties.limits.maxImageDimension1D;
        _limits.max_image_dimension_2d = _device_properties.limits.maxImageDimension2D;
        _limits.max_image_dimension_3d = _device_properties.limits.maxImageDimension3D;
        _limits.max_image_dimension_cube = _device_properties.limits.maxImageDimensionCube;
        _limits.max_image_array_layers = _device_properties.limits.maxImageArrayLayers;
        _limits.max_texel_buffer_elements = _device_properties.limits.maxTexelBufferElements;
        _limits.max_uniform_buffer_range = _device_properties.limits.maxUniformBufferRange;
        _limits.max_storage_buffer_range = _device_properties.limits.maxStorageBufferRange;
        _limits.max_push_constants_size = _device_properties.limits.maxPushConstantsSize;
        _limits.max_memory_allocation_count = _device_properties.limits.maxMemoryAllocationCount;
        _limits.max_sampler_allocation_count = _device_properties.limits.maxSamplerAllocationCount;
        _limits.max_sampler_anisotropy = _device_properties.limits.maxSamplerAnisotropy;
        _limits.max_framebuffer_width = _device_properties.limits.maxFramebufferWidth;
        _limits.max_framebuffer_height = _device_properties.limits.maxFramebufferHeight;
        _limits.max_framebuffer_layers = _device_properties.limits.maxFramebufferLayers;
        _limits.max_color_attachments = _device_properties.limits.maxColorAttachments;
        _limits.max_viewports = _device_properties.limits.maxViewports;
    }

    PhysicalDevice::PhysicalDevice()
    :   _physical_device( VK_NULL_HANDLE )
    ,   _instance( nullptr )
    {}

//    const std::vector<const char*>& PhysicalDevice::getRequiredExtensions()const
//    {
//        return _required_device_extensions;
//    }

//    bool PhysicalDevice::isDeviceSuitable
//    (
//        const DeviceInitializationInfo& info,
//        Instance* instance,
//        VkSurfaceKHR surface
//    )
//    {
//        // Define weights for scoring preferences (adjust as needed)
//        const int SCORE_WEIGHT_DISCRETE_GPU = 4000;
//        const int SCORE_WEIGHT_INTEGRATED_GPU = 500; // Base score if not discrete
//        const int SCORE_WEIGHT_PER_GB_VRAM = 25;    // Score per GB of dedicated VRAM
//        const int SCORE_WEIGHT_SHADER_FLOAT64 = 1000;
//        const int SCORE_WEIGHT_GEOMETRY_SHADER = 30;
//        const int SCORE_WEIGHT_TESSELLATION_SHADER = 60;
//        const int SCORE_WEIGHT_COMPUTE_INVOCATIONS = 1; // Score per 128 max invocations
//        const int SCORE_WEIGHT_API_VERSION_MAJOR = 200;
//        const int SCORE_WEIGHT_API_VERSION_MINOR = 20;
//
//        Log& log = kegeLog[ "vk-instance" ][ "info" ] << "  - Evaluating: " << _device_properties.deviceName;
//        KEGE_LOG_INFO << " (API: " << VK_API_VERSION_MAJOR( _device_properties.apiVersion ) << "."
//                 << VK_API_VERSION_MINOR( _device_properties.apiVersion ) << "."
//                 << VK_API_VERSION_PATCH( _device_properties.apiVersion )
//                 << ", Type: " << _device_properties.deviceType << ")";
//
//        // --- 1. Minimum Requirements & Feature Checks ---
//
//        // Check Minimum Feature Requirements Here!
//        // Example: Must support dynamic rendering
//        if ( !_features.dynamic_rendering )
//        {
//            KEGE_LOG_INFO << "    - Reason: Does not support required Dynamic Rendering feature.";
//            return false;
//        }
//
//        if ( !_features.sampler_anisotropy )
//        {
//            KEGE_LOG_INFO << "    - Reason: Does not support required Sampler Anisotropy feature.";
//            return false;
//        }
//
//        // Queue Families
//        if ( !_queue_family_indices.isComplete() )
//        {
//            KEGE_LOG_INFO << "    - Skipping: Missing required queue families.";
//             return false;
//        }
//
//        // Required Core Features Check
//        if ( info.require_shader_float64 && !_device_features.shaderFloat64 )
//        {
//            KEGE_LOG_INFO << "    - Skipping: Missing required feature: shaderFloat64";
//            return false;
//        }
//
//        if ( info.require_geometry_shader && !_device_features.geometryShader )
//        {
//            KEGE_LOG_INFO << "    - Skipping: Missing required feature: geometryShader";
//            return false;
//        }
//
//        if (info.require_tessellation_shader && !_device_features.tessellationShader)
//        {
//            KEGE_LOG_INFO << "    - Skipping: Missing required feature: tessellationShader";
//             return false;
//        }
//
//        // Add more required core feature checks here...
//        // Ray Tracing Check (if required)
//        if ( info.enable_raytracing )
//        {
//            // Note: Requires VK_KHR_get_physical_device_properties2 INSTANCE extension enabled!
//            const std::vector<const char*> rtExtensions =
//            {
//                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
//                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
//                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME
//            };
//
//
//            if ( !checkDeviceExtensionSupport( _physical_device, instance->getRequiredDeviceExtensions() ) )
//            {
//                KEGE_LOG_INFO << "    - Skipping: Missing required ray tracing extensions.";
//                return false;
//            }
//
//            if ( !checkRayTracingSupport( _physical_device, instance->getHandle() ) )
//            { // Checks actual features
//                KEGE_LOG_INFO << "    - Skipping: Missing required ray tracing features.";
//                return false;
//            }
//            KEGE_LOG_INFO << "    - Ray Tracing Supported (Required).\n";
//        }
//
//        // Minimum Dedicated VRAM Check
//        _dedicated_video_memory_mb = 0;
//        for (uint32_t i = 0; i < _memory_properties.memoryHeapCount; ++i)
//        {
//            if (( _memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ))
//            {
//                 // Simplification: sum all device-local heaps. A more complex check
//                 // could verify the types using this heap are primarily device-local.
//                _dedicated_video_memory_mb += _memory_properties.memoryHeaps[i].size;
//            }
//        }
//
//        uint32_t dedicated_vram_gb = static_cast<uint32_t>( _dedicated_video_memory_mb / (1000 * 1000 * 1000) );
//        if ( info.min_dedicated_video_memory > 0 && dedicated_vram_gb < info.min_dedicated_video_memory )
//        {
//            KEGE_LOG_INFO << "Skipping: Insufficient dedicated VRAM ("
//                     << dedicated_vram_gb << "MB available, "
//                     << info.min_dedicated_video_memory << "-GB required).";
//            return false;
//        }
//        KEGE_LOG_INFO << "Dedicated VRAM: " << dedicated_vram_gb << "-GB";
//
//        // --- 2. Scoring Based on Preferences ---
//        _preference_score = 0;
//
//        // Device Type Preference
//        if ( info.prefer_discrete_gpu )
//        {
//            if ( _device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
//            {
//                _preference_score += SCORE_WEIGHT_DISCRETE_GPU;
//                KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_DISCRETE_GPU << " (Discrete GPU preferred and found)";
//            }
//            else if ( _device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
//            {
//                 // Give integrated GPU a lower base score if discrete is preferred but not found
//                 _preference_score += SCORE_WEIGHT_INTEGRATED_GPU / 2;
//                KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_INTEGRATED_GPU / 2 << " (Integrated GPU, Discrete preferred)";
//            }
//        }
//        else
//        {
//             // If discrete is not specifically preferred, still score it higher than integrated
//             if ( _device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
//             {
//                 _preference_score += SCORE_WEIGHT_DISCRETE_GPU / 2; // Lower score if not explicitly preferred
//                 KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_DISCRETE_GPU / 2 << " (Discrete GPU)";
//             } else if ( _device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
//             {
//                 _preference_score += SCORE_WEIGHT_INTEGRATED_GPU;
//                 KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_INTEGRATED_GPU << " (Integrated GPU)";
//             }
//        }
//
//        // Other device types (Virtual, CPU) get low/no score based on this criterion
//        // VRAM Score (Maximize)
//        if ( info.prefer_max_vram )
//        {
//             int vram_score = dedicated_vram_gb * SCORE_WEIGHT_PER_GB_VRAM;
//             _preference_score += vram_score;
//            KEGE_LOG_INFO << "Score +" << vram_score << " (VRAM Preference: " << dedicated_vram_gb << " MB)";
//        }
//
//        // Preferred Features Score
//        if ( info.prefer_shader_float64 && _device_features.shaderFloat64 )
//        {
//            _preference_score += SCORE_WEIGHT_SHADER_FLOAT64;
//            KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_SHADER_FLOAT64 << " (Preferred feature: shaderFloat64)";
//        }
//
//        if ( info.prefer_geometry_shader && _device_features.geometryShader )
//        {
//            _preference_score += SCORE_WEIGHT_GEOMETRY_SHADER;
//            KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_GEOMETRY_SHADER << " (Preferred feature: geometryShader)";
//        }
//
//        if ( info.prefer_tessellation_shader && _device_features.tessellationShader )
//        {
//            _preference_score += SCORE_WEIGHT_TESSELLATION_SHADER;
//            KEGE_LOG_INFO << "Score +" << SCORE_WEIGHT_TESSELLATION_SHADER << " (Preferred feature: tessellationShader)";
//        }
//        
//        // Add scoring for other preferred features...
//        // "Processing Power" Proxies Score
//        if ( info.prefer_high_compute_limits )
//        {
//            // Using maxComputeWorkGroupInvocations as a proxy
//            int compute_score = ( _device_properties.limits.maxComputeWorkGroupInvocations / 128 ) * SCORE_WEIGHT_COMPUTE_INVOCATIONS;
//            _preference_score += compute_score;
//            KEGE_LOG_INFO << "Score +" << compute_score << " (Compute Limit Preference: "
//                     << _device_properties.limits.maxComputeWorkGroupInvocations << " max invocations)";
//            // Could also add scoring based on maxComputeWorkGroupSize[0] etc.
//        }
//
//        // API Version Score
//        if ( info.prefer_higher_api_version )
//        {
//            int api_score = VK_API_VERSION_MAJOR( _device_properties.apiVersion ) * SCORE_WEIGHT_API_VERSION_MAJOR +
//                            VK_API_VERSION_MINOR( _device_properties.apiVersion ) * SCORE_WEIGHT_API_VERSION_MINOR;
//             _preference_score += api_score;
//
//            KEGE_LOG_INFO << "Score +" << api_score << " (API Version Preference: "
//                     << VK_API_VERSION_MAJOR( _device_properties.apiVersion ) << "."
//                     << VK_API_VERSION_MINOR( _device_properties.apiVersion ) << ")";
//        }
//        KEGE_LOG_INFO << "total score: " << _preference_score;
//        return true;
//    }

    std::vector< VkSurfaceFormatKHR > PhysicalDevice::getPhysicalDeviceSurfaceFormats( VkSurfaceKHR surface )
    {
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR( _physical_device, surface, &format_count, nullptr);
        if (format_count != 0)
        {
            std::vector< VkSurfaceFormatKHR > formats(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR( _physical_device, surface, &format_count, formats.data() );
            return formats;
        }
        else
        {
            return {};
        }
    }

    VkSurfaceCapabilitiesKHR PhysicalDevice::getPhysicalDeviceSurfaceCapabilities( VkSurfaceKHR surface )
    {
        VkSurfaceCapabilitiesKHR capabilities;
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _physical_device, surface, &capabilities );
        if ( result == VK_SUCCESS )
        {
            return capabilities;
        }
        return {};
    }


    std::vector< VkPresentModeKHR > PhysicalDevice::getPhysicalDeviceSurfacePresentModes( VkSurfaceKHR surface )
    {
        uint32_t count;
        vkGetPhysicalDeviceSurfacePresentModesKHR( _physical_device, surface, &count, nullptr );
        if (count != 0)
        {
            std::vector< VkPresentModeKHR > present_modes(count);
            vkGetPhysicalDeviceSurfacePresentModesKHR( _physical_device, surface, &count, present_modes.data() );
            return present_modes;
        }
        else
        {
            return {};
        }
    }

    const VkPhysicalDeviceMemoryProperties& PhysicalDevice::getMemoryProperties()const
    {
        return _memory_properties;
    }
    
    const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const
    {
        return _device_properties;
    }

    const VkPhysicalDeviceFeatures& PhysicalDevice::getFeatures() const
    {
        return _device_features;
    }

    Instance* PhysicalDevice::getInstance()
    {
        return _instance;
    }
    
    const VkPhysicalDevice PhysicalDevice::getHandle()const
    {
        return _physical_device;
    }

    VkPhysicalDevice PhysicalDevice::getHandle()
    {
        return _physical_device;
    }

    void getRequiredFeatures( VkPhysicalDevice physical_device, VulkanDeviceFeatures2& feature )
    {
        // Chain all necessary feature structs to _features2.pNext during query
        feature.features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        feature.features11 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        feature.features12 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        feature.features13 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        feature.dynamic_rendering_features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
        feature.dynamic_rendering_features.dynamicRendering = VK_TRUE;

        feature.features2.pNext = &feature.dynamic_rendering_features;
        //feature.dynamic_rendering_features.pNext = nullptr;
        feature.dynamic_rendering_features.pNext = &feature.features11;
        feature.dynamic_rendering_features.dynamicRendering = VK_TRUE;

        feature.features11.pNext = &feature.features12;
        feature.features12.pNext = &feature.features13;
        feature.features13.pNext = nullptr;
        vkGetPhysicalDeviceFeatures2( physical_device, &feature.features2 );
    }


}
