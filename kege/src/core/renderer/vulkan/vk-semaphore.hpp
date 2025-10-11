//
//  vk-semaphore.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#ifndef vk_semaphore_hpp
#define vk_semaphore_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan semaphore synchronization primitives
     *
     * Encapsulates a VkSemaphore for GPU-GPU synchronization.
     */
    struct Semaphore : public kege::Semaphore
    {
        Semaphore( VkSemaphore s, vk::Device* d ): handle( s ), device( d ), prev( nullptr ), next( nullptr ) {}
        const vk::Semaphore* vk()const{ return this; };
        vk::Semaphore* vk(){ return this; };
        ~Semaphore();
        
        VkSemaphore handle;
        vk::Device* device;
        vk::Semaphore* prev;
        vk::Semaphore* next;
    };
}
#endif /* vk_semaphore_hpp */
