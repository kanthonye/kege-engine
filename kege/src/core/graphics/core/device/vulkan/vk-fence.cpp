//
//  vk-fence.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#include "vk-fence.hpp"
#include "vk-device.hpp"

namespace kege::vk{
    
    bool Fence::waitForFence( uint32_t wait_all, uint64_t timeout_nanoseconds)const
    {
        VkResult result = vkWaitForFences
        (
            device->handle(),
            1,
            &handle,
            wait_all,
            timeout_nanoseconds
        );
        return result == VK_SUCCESS; // TODO: return SUCCESS or TIMEOUT or ERROR_DEVICE_LOST
    }

    void Fence::resetFence()
    {
        vkResetFences( device->handle(), 1, &handle );
    }

    bool Fence::isReady()const
    {
        switch ( vkGetFenceStatus( device->handle(), handle ) )
        {
            case VK_SUCCESS: return true;
            case VK_NOT_READY: return false;
            default: return false;
        }
    }

    Fence::~Fence()
    {
        if ( device )
        {
            device->destroyFence( this );
            device = nullptr;
        }
    }
}
