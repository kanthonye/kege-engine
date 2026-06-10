//
//  vk-queue-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/5/25.
//

#ifndef vk_queue_manager_hpp
#define vk_queue_manager_hpp

#include "vk-utils.hpp"
#include "vk-swapchain.hpp"
#include "vk-command-buffer.hpp"

namespace kege::vk{

    struct SubmitSync
    {
        std::vector< VkPipelineStageFlags > wait_stages;
        std::vector< VkSemaphore > wait_semaphores;
        std::vector< VkSemaphore > signal_semaphores;
    };

    struct RenderTask
    {
        std::vector< SubmitSync > submit_syncs;
        std::vector< VkSubmitInfo > graphics_submits;
        std::vector< VkSubmitInfo > compute_submits;
        std::vector< VkSubmitInfo > transfer_submits;
        kege::array< VkFence > fences;
    };


    struct CpuFrameSync
    {
        void reset( const VkDevice& device )
        {
            if ( fence_count != 0 )
            {
                // wait for rendering task for the current frame to complete, if all its fences was not yet signaled.
                vkWaitForFences
                (
                    device,
                    fence_count,
                    fences, VK_TRUE, UINT64_MAX
                );
                // reset the fences for the current frame, so that they can be reused
                vkResetFences
                (
                    device,
                    fence_count,
                    fences
                );
                fence_count = 0;
            }
        }
        void push( VkFence fence )
        {
            fences[ fence_count ] = fence;
            fence_count += 1;
        }
        enum{GRAPHICS, COMPUTE, TRANSFER, MAX_FENCE_COUNT};
        VkFence fences[ MAX_FENCE_COUNT ];
        uint32_t fence_count;
    };

    /**
     * @file Device.h
     * @brief A Vulkan implementation of the GraphicsDevice interface
     *
     * This class encapsulates a Vulkan logical device and provides a complete
     * abstraction over all Vulkan resources and operations. It manages the creation,
     * usage, and destruction of all Vulkan resources through handle-based references.
     */
    class QueueManager
    {
    public:

        void endTransferQueueCommandBuffer( VkCommandBuffer command_buffer );
        VkCommandBuffer beginTransferQueueCommandBuffer();

        bool submit
        (
            vk::Swapchain* swapchain,
            const std::vector< kege::SubmitInfo >& submit_infos
        );

        bool submitEmpty
        (
            vk::Swapchain* swapchain
        );

        bool present( vk::Swapchain* swapchain );


        uint32_t getFrameIndex()const;
        void waitForFences();
        
        bool initialize( vk::Device* device, QueueFamilyIndices queue_family_indices );
        void shutdown();

        ~QueueManager();
        QueueManager();


        CpuFrameSync* getCpuFrameSync();
        RenderTask* getRenderTask();

    private:
        
        kege::array< kege::vk::RenderTask > _render_tasks;
        kege::array< CpuFrameSync > _cpu_frame_syncs;

        /** @brief Command pools for each queue type */
        std::unordered_map< QueueType, VkCommandPool > _command_pools;

        QueueFamilyIndices _queue_family_indices;

        /** @brief Graphics queue information */
        VkQueue _graphics_queue;

        /** @brief Compute queue information (might be same as graphics) */
        VkQueue _compute_queue;

        /** @brief Transfer queue information (might be same as graphics or compute) */
        VkQueue _transfer_queue;

        /** @brief Present queue information (often same as graphics) */
        VkQueue _present_queue;

        uint32_t _next_frame_index;
        uint32_t _frame_index;
        vk::Device* _device;
        friend vk::Device;
    };
}
#endif /* vk_queue_manager_hpp */
