//
//  fence.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef fence_hpp
#define fence_hpp

#include "../common/graphics-common.h"

namespace kege
{
    class Fence;
}

namespace kege::vk
{
    class Fence;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Fence > Fence;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Fence > Fence;
}

namespace kege{

    enum struct FenceStatus
    {
        Success,
        NotReady,
        DeviceLost,
    };

    class Fence : public kege::RefCounter
    {
    public:

        /**
         * @brief Wait for a fence to become signaled
         *
         * Blocks the calling thread until the fence is signaled or the timeout expires.
         *
         * @param wait_all Boolean value to indicate if the device should wait for all
         * @param timeout_nanoseconds Maximum time to wait in nanoseconds (UINT64_MAX for indefinite)
         * @return true if the fence was signaled, false if the timeout expired
         */
        virtual bool waitForFence( uint32_t wait_all, uint64_t timeout_nanoseconds )const = 0;

        /**
         * @brief Reset a fence to the unsignaled state
         */
        virtual void resetFence() = 0;

        /**
         * @brief Get the current status of a fence
         * @return true if the fence is signaled, false otherwise
         */
        virtual bool isReady()const = 0;


        const vk::Fence* vk()const{ return nullptr; };
        vk::Fence* vk(){ return nullptr; };
        virtual ~Fence() = default;

    protected:

        Fence() = default;
    };

}

#endif /* fence_hpp */
