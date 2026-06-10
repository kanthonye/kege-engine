//
//  physical-device.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//

#ifndef graphics_physical_device_hpp
#define graphics_physical_device_hpp

#include "common/graphics-common.h"

namespace kege
{
    class PhysicalDevice;
}

namespace kege::vk
{
    class PhysicalDevice;
}

namespace kege::ref
{
    typedef kege::Ref< kege::PhysicalDevice > PhysicalDevice;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::PhysicalDevice > PhysicalDevice;
}

namespace kege{

    enum struct PhysicalDeviceType
    {
        Other,
        IntegratedGpu,
        DiscreteGpu,
        VirtualGpu,
        DeviceCpu
    };

    /**
     * @brief Optional hardware features that can be requested during device initialization
     *
     * Not all hardware supports all features. Applications should check availability
     * before enabling and provide fallbacks when critical features are unavailable.
     */
    struct DeviceFeatures
    {
        /** @brief Enable per-attachment blend states instead of global blend */
        bool independent_blend = false;

        /** @brief Support for geometry shader stage in the pipeline */
        bool geometry_shader = false;

        /** @brief Support for tessellation shader stages in the pipeline */
        bool tessellation_shader = false;

        /** @brief Multiple simultaneous viewports and scissors for single-pass rendering */
        bool multi_viewport = false;

        /** @brief Support for DirectX Block Compressed texture formats (BC1-BC7) */
        bool texture_compression_bc = false;

        /** @brief Support for ETC2/EAC compressed texture formats (common on mobile) */
        bool texture_compression_etc2 = false;

        /** @brief Support for ASTC LDR compressed texture formats (common on mobile) */
        bool texture_compression_astc_ldr = false;

        /** @brief Anisotropic texture filtering for improved quality at oblique angles */
        bool sampler_anisotropy = false;

        /** @brief Allows rendering geometry beyond the near/far planes without clipping */
        bool depth_clamp = false;

        /** @brief Controls bias clamping for shadow maps and other depth techniques */
        bool depth_bias_clamp = false;

        /** @brief Enables wireframe and point rendering modes */
        bool fill_mode_non_solid = false;

        /** @brief Support for line widths greater than 1.0 pixel */
        bool wide_lines = false;

        /** @brief Support for point sizes greater than 1.0 pixel */
        bool large_points = false;

        /**
         * @brief Support for 64-bit floating point operations in shaders
         *
         * Enables double precision which is useful for scientific computing
         * but typically much slower than 32-bit operations.
         */
        bool shader_float64 = false;

        /**
         * @brief Support for 64-bit integer operations in shaders
         *
         * Enables long integer types which can be useful for certain algorithms.
         */
        bool shader_int64 = false;

        /**
         * @brief Support for concurrent queries from multiple command buffers
         *
         * Allows query objects to be used across multiple command buffers
         * without explicit synchronization.
         */
        bool pipeline_statistics_query = false;

        /** @brief Support for dynamic rendering */
        bool dynamic_rendering = false;

        /**
         * @brief Creates a feature request set for a basic 3D application
         *
         * @return DeviceFeatures configured for common 3D rendering needs
         */
        static DeviceFeatures createBasic3D() {
            DeviceFeatures features;
            features.independent_blend = true;
            features.fill_mode_non_solid = true;  // For debug wireframe
            features.sampler_anisotropy = true;   // For better texture quality
            features.depth_clamp = true;          // For shadow mapping
            return features;
        }
    };

    /**
     * @brief Hardware limits and capabilities of the selected device
     *
     * These values are queried from the device during initialization and
     * applications should respect them to ensure compatibility across
     * hardware configurations.
     */
    struct DeviceLimits {
        /** @brief Maximum width for 1D textures */
        uint32_t max_image_dimension_1d = 0;

        /** @brief Maximum width/height for 2D textures */
        uint32_t max_image_dimension_2d = 0;

        /** @brief Maximum width/height/depth for 3D textures */
        uint32_t max_image_dimension_3d = 0;

        /** @brief Maximum width/height for cubemap textures */
        uint32_t max_image_dimension_cube = 0;

        /** @brief Maximum array layers for array textures */
        uint32_t max_image_array_layers = 0;

        /** @brief Maximum elements in a texel buffer */
        uint32_t max_texel_buffer_elements = 0;

        /** @brief Maximum size in bytes for a uniform buffer range */
        uint32_t max_uniform_buffer_range = 0;

        /** @brief Maximum size in bytes for a storage buffer range */
        uint32_t max_storage_buffer_range = 0;

        /** @brief Maximum size in bytes for push constants */
        uint32_t max_push_constants_size = 0;

        /** @brief Maximum number of memory allocations */
        uint32_t max_memory_allocation_count = 0;

        /** @brief Maximum number of sampler objects */
        uint32_t max_sampler_allocation_count = 0;

        /** @brief Maximum anisotropy level for texture filtering */
        float max_sampler_anisotropy = 0.0f;

        /** @brief Maximum number of viewports */
        uint32_t max_viewports = 0;

        /** @brief Maximum width of a framebuffer */
        uint32_t max_framebuffer_width = 0;

        /** @brief Maximum height of a framebuffer */
        uint32_t max_framebuffer_height = 0;

        /** @brief Maximum layers of a framebuffer */
        uint32_t max_framebuffer_layers = 0;

        /** @brief Maximum color attachments in a framebuffer (MRTs) */
        uint32_t max_color_attachments = 0;

        /** @brief Supported sample counts for color attachments */
        SampleCount framebuffer_color_sample_counts = SampleCount::Count1;

        /** @brief Supported sample counts for depth attachments */
        SampleCount framebuffer_depth_sample_counts = SampleCount::Count1;

        /** @brief Required alignment for uniform buffer offsets */
        uint64_t min_uniform_buffer_offset_alignment = 0;

        /** @brief Required alignment for storage buffer offsets */
        uint64_t min_storage_buffer_offset_alignment = 0;
    };
    
    class PhysicalDevice : public kege::RefCounter
    {
    public:

        virtual const vk::PhysicalDevice* vk() const { return nullptr; }
        virtual vk::PhysicalDevice* vk() { return nullptr; }

        /** @brief Enabled device features */
        const DeviceFeatures& getDeviceFeatures()const;

        /** @brief Physical device limits */
        const DeviceLimits& getDeviceLimits()const;

        size_t   getDedicatedVideoMemorySize()const;
        uint32_t getDeviceID()const;

        PhysicalDeviceType getPhysicalDeviceType()const;
        uint32_t getDriverVersion()const;
        uint32_t getApiVersion()const;
        uint32_t getVendorID()const;

        const std::string& getName()const;

        virtual ~PhysicalDevice(){}
        PhysicalDevice();
        
    protected:

        /** @brief Enabled device features */
        DeviceFeatures _features = {};

        /** @brief Physical device limits */
        DeviceLimits _limits = {};

        size_t _dedicated_video_memory_mb;

        std::string _name;

        PhysicalDeviceType _device_type;
        uint32_t _driver_version;
        uint32_t _api_version;
        uint32_t _device_id;
        uint32_t _vendor_id;
    };
}
#endif /* graphics_physical_device_hpp */
