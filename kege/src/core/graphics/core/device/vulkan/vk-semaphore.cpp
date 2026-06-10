//
//  vk-semaphore.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#include "vk-semaphore.hpp"
#include "vk-device.hpp"

namespace kege::vk{
    
    Semaphore::~Semaphore()
    {
        if ( device )
        {
            device->destroySemaphore( this );
            device = nullptr;
        }
    }
}
