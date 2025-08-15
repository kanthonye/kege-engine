//
//  graphics-physical-device.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/28/25.
//

#include "../../graphics/core/graphics-physical-device.hpp"
namespace kege{

    const DeviceFeatures& PhysicalDevice::getDeviceFeatures()const
    {
        return _features;
    }

    const DeviceLimits& PhysicalDevice::getDeviceLimits()const
    {
        return _limits;
    }

    size_t PhysicalDevice::getDedicatedVideoMemorySize()const
    {
        return _dedicated_video_memory_mb;
    }

    PhysicalDeviceType PhysicalDevice::getPhysicalDeviceType()const
    {
        return _device_type;
    }

    uint32_t PhysicalDevice::getDriverVersion()const
    {
        return _driver_version;
    }

    uint32_t PhysicalDevice::getApiVersion()const
    {
        return _api_version;
    }

    uint32_t PhysicalDevice::getVendorID()const
    {
        return _vendor_id;
    }

    uint32_t PhysicalDevice::getDeviceID()const
    {
        return _device_id;
    }

    const std::string& PhysicalDevice::getName()const
    {
        return _name;
    }

    PhysicalDevice::PhysicalDevice()
    :   _features({})
    ,   _limits({})
    ,   _dedicated_video_memory_mb(0)
    ,   _device_id(0)
    ,   _name()
    {}

}
