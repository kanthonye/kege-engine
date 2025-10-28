//
//  vulkan-physical-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//
#include <map>
#include "vk-instance.hpp"
#include "vk-physical-device.hpp"

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

    const VkSurfaceKHR& PhysicalDevice::getSurface()const
    {
        return _surface;
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
