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
#include "vk-pipeline-layout-manager.hpp"

namespace kege::vk{

    struct RenderTasks
    {
        std::vector< vk::SubmitInfo > submission;
        std::vector< VkFence > fences;
        int fence_count;
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
            const std::vector< kege::SubmitInfo >& submit_infos,
            const vk::Semaphore* image_available,
            const vk::Semaphore* render_complete,
            uint32_t frame_index
        );

        bool submit( const kege::SubmitInfo& submit_info );
        bool present( vk::Swapchain* swapchain, const vk::Semaphore* wait_semaphore );

        bool beginSubmit( uint32_t frame_index );
        void endSubmit();

        bool initialize( vk::Device* device, QueueFamilyIndices queue_family_indices );
        void shutdown();

        ~QueueManager();
        QueueManager();
        
    private:

        std::vector< vk::RenderTasks > _render_tasks;

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

        bool _accepting_submissions;

        vk::Device* _device;
        friend vk::Device;
    };
}
#endif /* vk_queue_manager_hpp */
