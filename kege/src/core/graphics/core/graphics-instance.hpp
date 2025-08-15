//
//  graphics-instance.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef graphics_instance_hpp
#define graphics_instance_hpp

#include "../../graphics/core/graphics-device.hpp"
#include "../../graphics/core/graphics-physical-device.hpp"

namespace kege{

    class GraphicsInstance : public RefCounter
    {
    public:

        virtual GraphicsDevice* createDevice( kege::PhysicalDevice* physical_device, kege::GraphicsSurface surface ) = 0;
        virtual void destroyDevice( GraphicsDevice* device ) = 0;

        virtual bool initalize( const DeviceInitializationInfo& info ) = 0;
        virtual void shutdown() = 0;

        virtual PhysicalDevice* getPhysicalDevice( uint32_t physical_device_index ) = 0;
        virtual PhysicalDevice* getBestSuitablePhysicalDevice( const DeviceInitializationInfo& info, kege::GraphicsSurface surface ) = 0;
        virtual uint32_t getPhysicalDeviceCount() = 0;
        virtual void listPhysicalDevicesInfo() = 0;

        virtual GraphicsAPI getGraphicsAPI() = 0;

        virtual ~GraphicsInstance(){}
    };
    
}
#endif /* graphics_instance_hpp */
