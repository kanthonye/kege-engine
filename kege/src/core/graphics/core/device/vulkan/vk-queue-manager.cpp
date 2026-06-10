//
//  vk-queue-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/5/25.
//

#include "vk-device.hpp"
#include "vk-queue-manager.hpp"

namespace kege::vk{

    VkResult createCommandBufferPool( VkDevice device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags, VkCommandPool* command_pool )
    {
        VkCommandPoolCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.queueFamilyIndex = queue_family_index;
        create_info.flags = flags;
        return vkCreateCommandPool( device, &create_info, nullptr, command_pool );
    }

    void QueueManager::endTransferQueueCommandBuffer( VkCommandBuffer command_buffer )
    {
        vkEndCommandBuffer( command_buffer );

        // submit the command buffer to the graphics queue for execution
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        if( vkQueueSubmit( _transfer_queue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS )
        {
            kege::Log::error << "submission to compute queue failed in submitCommands()"<<Log::nl;
            return;
        }
        vkQueueWaitIdle( _transfer_queue );
        vkFreeCommandBuffers( _device->handle(), _command_pools[ QueueType::Transfer ], 1, &command_buffer );
    }

    VkCommandBuffer QueueManager::beginTransferQueueCommandBuffer()
    {
        VkCommandBuffer command_buffer;

        // allocate a single command buffer from the command pool
        VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        info.commandPool = _command_pools[ QueueType::Transfer ];
        vkAllocateCommandBuffers( _device->handle(), &info, &command_buffer );

        // begin recording commands into the command buffer
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer( command_buffer, &begin_info );
        return command_buffer;
    }

    bool QueueManager::submit
    (
        vk::Swapchain* swapchain,
        const std::vector< kege::SubmitInfo >& submit_infos
    )
    {
        GpuFrameSync* gpu_sync = swapchain->getGpuFrameSync();
        CpuFrameSync* cpu_sycn = getCpuFrameSync();
        RenderTask* task = getRenderTask();
        task->submit_syncs.clear();
        task->transfer_submits.clear();
        task->compute_submits.clear();
        task->graphics_submits.clear();
        task->submit_syncs.resize( submit_infos.size() );

        uint32_t graphics_submit_count = 0;
        uint32_t compute_submit_count = 0;
        uint32_t transfer_submit_count = 0;

        for ( int submit_index = 0; submit_index <submit_infos.size(); ++ submit_index )
        {
            const kege::SubmitInfo& submit = submit_infos[ submit_index ];
            kege::vk::SubmitSync& sync = task->submit_syncs[ submit_index ];
            
            for (int sem_index=0; sem_index < submit.wait_semaphores.size(); ++sem_index)
            {
                sync.wait_stages.push_back( convertPipelineStage( submit.wait_stages[sem_index] ) );
                sync.wait_semaphores.push_back( submit.wait_semaphores[sem_index]->vk()->handle );
            }
            if( submit.render_complete_semaphore )
            {
                sync.signal_semaphores.push_back( submit.render_complete_semaphore->vk()->handle );
            }

            switch ( submit.command_buffer->getQueueType() )
            {
                case QueueType::Graphics: graphics_submit_count += 1; break;
                case QueueType::Compute:  compute_submit_count  += 1; break;
                case QueueType::Transfer: transfer_submit_count += 1; break;

                default:
                {
                    kege::Log::error << "INVALID_DEVICE_QUEUE"<<Log::nl;
                    return false;
                }
            }
        }

        // Track which queue type will have the last submit
        bool graphics_has_work = (graphics_submit_count > 0);
        bool compute_has_work  = (compute_submit_count > 0);
        bool transfer_has_work = (transfer_submit_count > 0);

        // Signal render_complete on the last submit of the frame
        bool signal_on_graphics = graphics_has_work;
        bool signal_on_compute  = !graphics_has_work && compute_has_work;
        bool signal_on_transfer = !graphics_has_work && !compute_has_work && transfer_has_work;

        if( graphics_submit_count != 0 ) task->graphics_submits.resize( graphics_submit_count );
        if( compute_submit_count  != 0 ) task->compute_submits.resize( compute_submit_count );
        if( transfer_submit_count != 0 ) task->transfer_submits.resize( transfer_submit_count );

        graphics_submit_count = 0;
        compute_submit_count = 0;
        transfer_submit_count = 0;

        for ( int submit_index = 0; submit_index <submit_infos.size(); ++ submit_index )
        {
            const kege::SubmitInfo& info = submit_infos[ submit_index ];
            kege::vk::SubmitSync& sync = task->submit_syncs[ submit_index ];

            VkSubmitInfo* submit = nullptr;
            switch ( info.command_buffer->getQueueType() )
            {
                case QueueType::Graphics:
                {
                    // image_available_semaphore Only on First Graphics Submit
                    // only the first graphics submit waits for the swapchain image.
                    if (graphics_submit_count == 0)
                    {
                        sync.wait_semaphores.push_back( gpu_sync->image_available_semaphore->vk()->handle );
                        sync.wait_stages.push_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
                    }
                    submit = &task->graphics_submits[ graphics_submit_count++ ];
                    // render_complete_semaphore Only on Last Graphics Submit
                    // only the last graphics submit signals presentation completion.
                    if (signal_on_graphics && graphics_submit_count == task->graphics_submits.size())
                    {
                        sync.signal_semaphores.push_back( gpu_sync->render_complete_semaphore->vk()->handle );
                    }
                    break;
                }
                case QueueType::Compute:
                {
                    submit = &task->compute_submits[ compute_submit_count++ ];
                    // If you have a frame with only compute or transfer work (no graphics submits),
                    // The swapchain expects the render_complete_semaphore to be signaled before presentation.
                    if (signal_on_compute && compute_submit_count == task->compute_submits.size())
                    {
                        sync.signal_semaphores.push_back( gpu_sync->render_complete_semaphore->vk()->handle );
                    }
                    break;
                }
                case QueueType::Transfer:
                {
                    submit = &task->transfer_submits[ transfer_submit_count++ ];
                    // If you have a frame with only compute or transfer work (no graphics submits),
                    // The swapchain expects the render_complete_semaphore to be signaled before presentation.
                    if (signal_on_transfer && transfer_submit_count == task->transfer_submits.size())
                    {
                        sync.signal_semaphores.push_back( gpu_sync->render_complete_semaphore->vk()->handle );
                    }
                    break;
                }
                default:
                {
                    kege::Log::error << "INVALID_DEVICE_QUEUE"<<Log::nl;
                    return false;
                }
            }

            *submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submit->commandBufferCount   = 1;
            submit->pCommandBuffers      = &info.command_buffer->vk()->handle();

            submit->pWaitDstStageMask    = sync.wait_stages.data();

            submit->waitSemaphoreCount   = static_cast<uint32_t>( sync.wait_semaphores.size() );
            submit->pWaitSemaphores      = ( sync.wait_semaphores.size() ) ? sync.wait_semaphores.data() : VK_NULL_HANDLE;

            submit->signalSemaphoreCount = static_cast<uint32_t>( sync.signal_semaphores.size() );
            submit->pSignalSemaphores    = ( sync.signal_semaphores.size() ) ? sync.signal_semaphores.data() : VK_NULL_HANDLE;

//            for (size_t i = 0; i < sync.wait_semaphores.size(); i++)
//            {
//                std::cout <<"Frame:" <<_frame_index << " Submit " << submit_index << " wait[" << i << "] = " << (void*) sync.wait_semaphores[i]<<"\n";
//            }
//            for (size_t i = 0; i < sync.signal_semaphores.size(); i++)
//            {
//                std::cout <<"Frame:" <<_frame_index << " Submit " << submit_index << " signal[" << i << "] = " << (void*) sync.signal_semaphores[i]<<"\n";
//            }
        }
        //std::cout <<"\n";

        if ( !task->graphics_submits.empty() )
        {
            VkResult result = vkQueueSubmit
            (
                _graphics_queue,
                (int) task->graphics_submits.size(),
                task->graphics_submits.data(),
                task->fences[ vk::CpuFrameSync::GRAPHICS ]
            );
            if( result != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
            cpu_sycn->push( task->fences[ vk::CpuFrameSync::GRAPHICS ] );
        }

        if ( !task->compute_submits.empty() )
        {
            VkResult result = vkQueueSubmit
            (
                _compute_queue,
                (int)task->compute_submits.size(),
                task->compute_submits.data(),
                task->fences[ vk::CpuFrameSync::COMPUTE ]
            );
            if( result != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
            cpu_sycn->push( task->fences[ vk::CpuFrameSync::COMPUTE ] );
        }
        
        if ( !task->transfer_submits.empty() )
        {
            VkResult result = vkQueueSubmit
            (
                _transfer_queue,
                (int)task->transfer_submits.size(),
                task->transfer_submits.data(),
                task->fences[ vk::CpuFrameSync::TRANSFER ]
            );
            if( result != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
            cpu_sycn->push( task->fences[ vk::CpuFrameSync::TRANSFER ] );
        }

        return true;
    }

    bool QueueManager::submitEmpty
    (
        vk::Swapchain* swapchain
    )
    {
        GpuFrameSync* gpu_sync = swapchain->getGpuFrameSync();
        CpuFrameSync* cpu_sync = QueueManager::getCpuFrameSync();
        kege::vk::RenderTask* task = getRenderTask();

        task->submit_syncs.clear();
        task->transfer_submits.clear();
        task->compute_submits.clear();
        task->graphics_submits.clear();
        //_render_task.submit_syncs.clear();
        task->submit_syncs.resize( 1 );

        VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.commandBufferCount = 0;

        // Wait for the acquired image
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &gpu_sync->image_available_semaphore->vk()->handle;
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        submit_info.pWaitDstStageMask = &wait_stage;

        // Signal render_complete immediately
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &gpu_sync->render_complete_semaphore->vk()->handle;

        VkResult result = vkQueueSubmit(_graphics_queue, 1, &submit_info, task->fences[ vk::CpuFrameSync::GRAPHICS ]);
        cpu_sync->push( task->fences[ vk::CpuFrameSync::GRAPHICS ] );
        return result == VK_SUCCESS;
    }

    bool QueueManager::present( vk::Swapchain* swapchain )
    {
        const vk::Semaphore* render_complete = swapchain->getFrameFinishSemaphore()->vk();

        //VkSemaphore wait_semaphores[] = { wait_semaphore->handle };
        VkSwapchainKHR swapchains[] = { swapchain->getHandle() };
        uint32_t image_indices[] = { swapchain->getImageIndex() };

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = (render_complete != nullptr)? &render_complete->handle: nullptr;
        present_info.swapchainCount = (render_complete != nullptr) ? 1 : 0;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = image_indices;
        present_info.pResults = nullptr; // Optional

        /** --- Submit Present Request --- */
        VkResult result = vkQueuePresentKHR( _present_queue, &present_info );

        /** --- Handle Results --- */
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            KEGE_LOG_WARN << "Out of date image during present. Needs recreation." <<Log::nl;
            swapchain->setShouldRecreate( true );
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            KEGE_LOG_WARN << "Suboptimal image during present. Needs recreation soon." <<Log::nl;
            swapchain->setShouldRecreate( true );
        }
        else if (result != VK_SUCCESS)
        {
            /** This could be a more serious error (e.g., device lost) */
            kege::Log::error << "Failed to queue present! Error: " << result <<Log::nl;
            return false;
        }

        /** On success, no specific action needed here, but render loop needs to check needsRecreation flag. */
        return true;
    }

    CpuFrameSync* QueueManager::getCpuFrameSync()
    {
        return &_cpu_frame_syncs[ _frame_index ];
    }

    RenderTask* QueueManager::getRenderTask()
    {
        return &_render_tasks[ _frame_index ];
    }

    uint32_t QueueManager::getFrameIndex()const
    {
        return _frame_index;
    }

    void QueueManager::waitForFences()
    {
        _frame_index = _next_frame_index;
        _cpu_frame_syncs[ _frame_index ].reset( _device->handle() );
        _next_frame_index = (_next_frame_index + 1) % (uint32_t)_render_tasks.size();

        //sync->reset();
        /**
         * Fences are tracked per frame-in-flight, not per swapchain image.
         *
         * At the beginning of each frame, the CPU waits on the fence associated
         * with the current frame index:
         *
         *   frame_index = 0 -> wait for frame 0 fence
         *   frame_index = 1 -> wait for frame 1 fence
         *   frame_index = 2 -> wait for frame 2 fence
         *   frame_index = 0 -> wait for frame 0 fence (wrap around)
         *
         * The swapchain image index must not be used for fence lookup because
         * it is only known after vkAcquireNextImageKHR() returns. If fence
         * synchronization were tied to image_index, vkAcquireNextImageKHR()
         * would need to be called before vkWaitForFences(), which breaks the
         * intended frame-in-flight synchronization model and can lead to
         * synchronization issues.
         */
    }

    bool QueueManager::initialize(vk::Device *device, QueueFamilyIndices queue_family_indices )
    {
        _device = device;
        _queue_family_indices = queue_family_indices;
        
        // 5. Get Queue Handles
        // Use the stored _queue_family_indices
        //_graphics_queue.family_index = _queue_family_indices.graphics_family.value();

        if ( !_queue_family_indices.compute_family.has_value() )
        {
            // Compute might reuse graphics queue if no dedicated compute family found
            _queue_family_indices.compute_family = _queue_family_indices.graphics_family;
            KEGE_LOG_INFO <<"- " << "Using graphics queue for compute operations.\n";
        }

        if ( !_queue_family_indices.transfer_family.has_value() )
        {
            // Transfer might reuse graphics or compute queue
            _queue_family_indices.transfer_family = _queue_family_indices.graphics_family;
            KEGE_LOG_INFO <<"- " << "Using graphics queue for transfer operations.\n";
        }

        if ( !_queue_family_indices.present_family.has_value() )
        {
            // This is usually an error if you need devices, as findQueueFamilies should have failed.
            kege::Log::error << "Present queue family index not found after device creation (should not happen)."<<Log::nl;
            // Maybe default to graphics? But presentation might fail later.
            _queue_family_indices.present_family = _queue_family_indices.graphics_family;
            vkGetDeviceQueue( _device->handle(), _queue_family_indices.present_family.value(), 0, &_present_queue );
        }

        /** ---------- Get Devuce Queues ---------- */
        vkGetDeviceQueue( _device->handle(), _queue_family_indices.graphics_family.value(), 0, &_graphics_queue );
        vkGetDeviceQueue( _device->handle(), _queue_family_indices.compute_family.value(),  0, &_compute_queue );
        vkGetDeviceQueue( _device->handle(), _queue_family_indices.transfer_family.value(), 0, &_transfer_queue );
        vkGetDeviceQueue( _device->handle(), _queue_family_indices.present_family.value(),  0, &_present_queue );

        /** ---------- Create Command Buffer Pools ---------- */

        VkResult result = createCommandBufferPool
        (
            _device->handle(),
            _queue_family_indices.graphics_family.value(),
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Graphics ]
        );

        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "unable to create graphics CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }

        result = createCommandBufferPool
        (
            _device->handle(),
            _queue_family_indices.compute_family.value(),
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Compute ]
        );

        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "unable to create compute CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }

        result = createCommandBufferPool
        (
            _device->handle(),
            _queue_family_indices.transfer_family.value(),
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            &_command_pools[ QueueType::Transfer ]
        );

        if ( result != VK_SUCCESS )
        {
            kege::Log::error << "unable to create transfer CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }

        _cpu_frame_syncs.resize( MAX_FRAMES_IN_FLIGHT );
        _render_tasks.resize( MAX_FRAMES_IN_FLIGHT );
        for (int i=0; i<_render_tasks.size(); ++i)
        {
            _render_tasks[i].fences.resize( CpuFrameSync::MAX_FENCE_COUNT );
            for (int j=0; j<CpuFrameSync::MAX_FENCE_COUNT; ++j) {
                _render_tasks[i].fences[j] = _device->core().createFence( false );
                if( _render_tasks[i].fences[j] == VK_NULL_HANDLE )
                {
                    return false;
                }
            }
        }
        return true;
    }
    
    void QueueManager::shutdown()
    {
        _cpu_frame_syncs.clear();
        
        if( _device )
        {
            for (int i=0; i<_render_tasks.size(); ++i)
            {
                for (int j=0; j<CpuFrameSync::MAX_FENCE_COUNT; ++j)
                {
                    _device->core().destroyFence( _render_tasks[i].fences[j] );
                }
            }
            _render_tasks.clear();

            for(auto const& [key, val] : _command_pools)
            {
                if (val != VK_NULL_HANDLE)
                {
                    vkDestroyCommandPool( _device->handle(), val, nullptr );
                }
            }
            _command_pools.clear();
            _device = nullptr;
        }
    }

    QueueManager::QueueManager()
    :   _device( nullptr )
    ,   _graphics_queue( VK_NULL_HANDLE )
    ,   _compute_queue( VK_NULL_HANDLE )
    ,   _transfer_queue( VK_NULL_HANDLE )
    ,   _present_queue( VK_NULL_HANDLE )
    ,   _next_frame_index( 0 )
    ,   _frame_index( 0 )
    {}

    QueueManager::~ QueueManager()
    {
        shutdown();
    }

    
}
