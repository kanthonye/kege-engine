//
//  "instance.hpp"
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef kege_graphics_instance_hpp
#define kege_graphics_instance_hpp

#include "device.hpp"

namespace kege
{
    class GraphicsInstance;
}

namespace kege::vk
{
    class GraphicsInstance;
}

namespace kege::ref
{
    typedef kege::Ref< kege::GraphicsInstance > GraphicsInstance;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::GraphicsInstance > GraphicsInstance;
}

namespace kege{

    class GraphicsInstance : public RefCounter
    {
    public:

        virtual GraphicsDevice* createDevice( kege::PhysicalDevice* physical_device ) = 0;
        virtual void destroyDevice( GraphicsDevice* device ) = 0;

        virtual bool initalize( const DeviceInitializationInfo& info ) = 0;
        virtual void shutdown() = 0;

        virtual PhysicalDevice* getPhysicalDevice( uint32_t physical_device_index ) = 0;

        virtual PhysicalDevice* getBestSuitablePhysicalDevice
        ( const DeviceInitializationInfo& info ) = 0;

        virtual uint32_t getPhysicalDeviceCount() = 0;
        virtual void listPhysicalDevicesInfo() = 0;

        virtual GraphicsAPI getGraphicsAPI() = 0;

        virtual ~GraphicsInstance(){}
    };
    
}
namespace kege::ref{
    typedef kege::Ref< kege::GraphicsInstance > GraphicsInstance;
}
#endif /* kege_graphics_instance_hpp */
