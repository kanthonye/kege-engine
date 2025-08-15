//
//  graphics-physical-device.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//

#ifndef graphics_physical_device_hpp
#define graphics_physical_device_hpp

#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    class PhysicalDevice
    {
    public:

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
