//
//  vk-fence.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/10/25.
//

#ifndef vk_fence_hpp
#define vk_fence_hpp

#include "vk-utils.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan fence synchronization primitives
     *
     * Encapsulates a VkFence for CPU-GPU synchronization.
     */
    struct Fence : public kege::Fence
    {
        /**
         * @brief Wait for a fence to become signaled
         *
         * Blocks the calling thread until the fence is signaled or the timeout expires.
         *
         * @param wait_all Boolean value to indicate if the device should wait for all
         * @param timeout_nanoseconds Maximum time to wait in nanoseconds (UINT64_MAX for indefinite)
         * @return true if the fence was signaled, false if the timeout expired
         */
        bool waitForFence( uint32_t wait_all, uint64_t timeout_nanoseconds )const;

        /**
         * @brief Reset a fence to the unsignaled state
         */
        void resetFence();

        /**
         * @brief Get the current status of a fence
         * @return true if the fence is signaled, false otherwise
         */
        bool isReady()const;

        Fence( VkFence f, vk::Device* d ): handle( f ), device( d ), prev( nullptr ), next( nullptr ) {}
        ~Fence();
        
        /** @brief Native Vulkan fence handle */
        VkFence handle;
        vk::Device* device;
        vk::Fence* prev;
        vk::Fence* next;
    };
}
#endif /* vk_fence_hpp */
