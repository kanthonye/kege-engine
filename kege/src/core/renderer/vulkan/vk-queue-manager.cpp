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
            KEGE_LOG_ERROR << "submission to compute queue failed in submitCommands()"<<Log::nl;
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

    bool QueueManager::submit( const std::vector< kege::SubmitInfo >& submit_infos, kege::Swapchain* swapchain, uint32_t frame_index )
    {
        if ( !_accepting_submissions )
        {
            kege::Log::error << "Must called QueueManager::beingSubmit() before calling QueueManager::submit()." <<Log::nl;
            return false;
        }

        int image_index = swapchain->vk()->acquireNextImage();
        if( image_index < 0 )
        {
            kege::Log::error << "Could not acquireNextImage() from swapchain -> '" <<swapchain->name() <<"'"<<Log::nl;
            return false;
        }

        std::vector< VkSubmitInfo > graphics_submits;
        std::vector< VkSubmitInfo > compute_submits;
        std::vector< VkSubmitInfo > transfer_submits;

        std::vector< std::vector< VkSemaphore > > wait_semaphores( submit_infos.size() );
        std::vector< std::vector< VkPipelineStageFlags > > wait_stages( submit_infos.size() );

        for ( int submit_index = 0; submit_index <submit_infos.size(); ++ submit_index )
        {
            const kege::SubmitInfo& info = submit_infos[ submit_index ];

            for (int sem_index=0; sem_index<info.wait_semaphores.size(); ++sem_index)
            {
                wait_stages[ submit_index ].push_back( convertPipelineStage( info.wait_stages[sem_index] ) );
                wait_semaphores[ submit_index ].push_back( info.wait_semaphores[sem_index]->vk()->handle );
            }

            VkSubmitInfo submit_info         = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submit_info.commandBufferCount   = 1;
            submit_info.pCommandBuffers      = &info.command_buffer->vk()->handle();
            submit_info.pWaitDstStageMask    = wait_stages[ submit_index ].data();
            submit_info.waitSemaphoreCount   = static_cast<uint32_t>( wait_semaphores[ submit_index ].size() );
            submit_info.pWaitSemaphores      = ( wait_semaphores[ submit_index ].size() ) ? wait_semaphores[ submit_index ].data() : VK_NULL_HANDLE;

            if( info.render_complete_semaphore )
            {
                submit_info.signalSemaphoreCount = 1;
                submit_info.pSignalSemaphores    = &info.render_complete_semaphore->vk()->handle;
            }


            switch ( info.command_buffer->getQueueType() )
            {
                case QueueType::Graphics: graphics_submits.push_back( submit_info ); break;
                case QueueType::Compute:  compute_submits.push_back( submit_info );  break;
                case QueueType::Transfer: transfer_submits.push_back( submit_info ); break;

                default:
                {
                    kege::Log::error << "INVALID_DEVICE_QUEUE"<<Log::nl;
                    return false;
                }
            }
        }

        vk::FrameSync& sync = swapchain->vk()->getFrameSync( image_index );
        vk::RenderTasks& tasks = _render_tasks[ frame_index ];
        tasks.fence_count = 0;

        if ( !graphics_submits.empty() )
        {
            /**
             * For graphics submits, Before execution can begin, it need to wait for the swapchain image to be available.
             * Thus we need to modify the first and last submit info, the first submit should wait on image_available_semaphore.
             * The system needs to know when the execution finish also, thus the render_complete_semaphore is used.
             */
            std::vector< VkSemaphore > first_wait_sems;
            std::vector< VkPipelineStageFlags > first_wait_stages;

            // Add swapchain semaphore, so the execution process wait on the swapchain image to be available before rendering
            first_wait_sems.push_back( sync.image_available_semaphore );
            first_wait_stages.push_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );

            // Add the existing semaphores
            for (int i=0; i<graphics_submits[0].waitSemaphoreCount; ++i)
            {
                first_wait_sems.push_back( graphics_submits[0].pWaitSemaphores[i] );
                first_wait_stages.push_back( graphics_submits[0].pWaitDstStageMask[i] );
            }
            graphics_submits[0].waitSemaphoreCount = static_cast<uint32_t>( first_wait_sems.size() );
            graphics_submits[0].pWaitSemaphores = first_wait_sems.data();
            graphics_submits[0].pWaitDstStageMask = first_wait_stages.data();

            // Don't forget to add render_complete_semaphore to the LAST submit!
            size_t last_index = graphics_submits.size() - 1;
            std::vector< VkSemaphore > last_signal_sems;
            last_signal_sems.push_back( sync.render_complete_semaphore );
            for (int i=0; i<graphics_submits[ last_index ].signalSemaphoreCount; ++i)
            {
                last_signal_sems.push_back( graphics_submits[ last_index ].pSignalSemaphores[i] );
            }
            graphics_submits[ last_index ].signalSemaphoreCount = static_cast<uint32_t>( last_signal_sems.size() );
            graphics_submits[ last_index ].pSignalSemaphores = last_signal_sems.data();

            // finally we submit all the submit-info for execution.
            if( vkQueueSubmit( _graphics_queue, (int)graphics_submits.size(), graphics_submits.data(), tasks.fences[ tasks.fence_count++ ] ) != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
        }
        if ( !compute_submits.empty() )
        {
            if( vkQueueSubmit( _compute_queue, (int)compute_submits.size(), compute_submits.data(), tasks.fences[ tasks.fence_count++ ] ) != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
        }
        if ( !transfer_submits.empty() )
        {
            if( vkQueueSubmit( _transfer_queue, (int)transfer_submits.size(), transfer_submits.data(), tasks.fences[ tasks.fence_count++ ] ) != VK_SUCCESS )
            {
                kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                return false;
            }
        }

        return true;
    }

    bool QueueManager::submit( const kege::SubmitInfo& info )
    {
        VkSubmitInfo submit_info         = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &info.command_buffer->vk()->handle();
        switch ( info.command_buffer->getQueueType() )
        {
            case QueueType::Graphics:
                if( vkQueueSubmit( _graphics_queue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS )
                {
                    kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;

            case QueueType::Compute:
                if( vkQueueSubmit( _compute_queue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS )
                {
                    kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;
                
            case QueueType::Transfer:
                if( vkQueueSubmit( _transfer_queue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS )
                {
                    kege::Log::error << "submission to graphics queue failed in submitCommands()"<<Log::nl;
                    return false;
                }
                break;

            default:
            {
                kege::Log::error << "INVALID_DEVICE_QUEUE"<<Log::nl;
                return false;
            }
        }
        return true;
    }

    bool QueueManager::beginSubmit( uint32_t frame_index )
    {
        int task_index = frame_index % _render_tasks.size();
        
        // get the current frame
        vk::RenderTasks& tasks = _render_tasks[ task_index ];

        // clear the submission data from previous frame
        tasks.submission.clear();

        if ( tasks.fence_count != 0 )
        {
            // wait for rendering task for the current frame to complete, if all its fences was not yet signaled.
            vkWaitForFences( _device->_device, tasks.fence_count, tasks.fences.data(), VK_TRUE, UINT64_MAX );

            // reset the fences for the current frame, so that they can be reused
            vkResetFences( _device->_device, tasks.fence_count, tasks.fences.data() );

            tasks.fence_count = 0;
        }

        _accepting_submissions = true;
        return true;
    }

    void QueueManager::endSubmit()
    {
        _accepting_submissions = false;
    }

    bool QueueManager::present( vk::Swapchain* swapchain )
    {
        if ( _accepting_submissions )
        {
            kege::Log::error << "Can not present while accepting submissions. Must endSubmit() before presenting." <<Log::nl;
            return false;
        }

        vk::FrameSync& sync = swapchain->getFrameSync( swapchain->getImageIndex() );

        VkSemaphore wait_semaphores[] = { sync.render_complete_semaphore };
        VkSwapchainKHR swapchains[] = { swapchain->getHandle() };
        uint32_t image_indices[] = { swapchain->getImageIndex() };

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = wait_semaphores;
        present_info.swapchainCount = 1;
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
            KEGE_LOG_ERROR << "Failed to queue present! Error: " << result <<Log::nl;
            return false;
        }

        /** On success, no specific action needed here, but render loop needs to check needsRecreation flag. */
        return true;
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
            KEGE_LOG_ERROR << "Present queue family index not found after device creation (should not happen)."<<Log::nl;
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
            KEGE_LOG_ERROR << "unable to create graphics CommandBufferPool. reason ->" << result<<Log::nl;
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
            KEGE_LOG_ERROR << "unable to create compute CommandBufferPool. reason ->" << result<<Log::nl;
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
            KEGE_LOG_ERROR << "unable to create transfer CommandBufferPool. reason ->" << result<<Log::nl;
            return false;
        }
        _render_tasks.resize( MAX_FRAMES_IN_FLIGHT );
        for (int i=0; i<_render_tasks.size(); ++i)
        {
            _render_tasks[i].fence_count = 0;
            _render_tasks[i].fences.resize(3); // 3 fence one for each queue. graphics, compute, and transfer
            for (int k=0; k<_render_tasks[i].fences.size(); ++k)
            {
                _render_tasks[i].fences[k] = _device->_manager.createFence( 0 );
            }
        }
        return true;
    }
    
    void QueueManager::shutdown()
    {
        if( _device )
        {
            for (int i=0; i<_render_tasks.size(); ++i)
            {
                _render_tasks[i].submission.clear();
                for (int k=0; k<_render_tasks[i].fences.size(); ++k)
                {
                    _device->_manager.destroyFence( _render_tasks[i].fences[k] );
                }
                _render_tasks[i].fences.clear();
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
    ,   _accepting_submissions( false )
    {}

    QueueManager::~ QueueManager()
    {
        shutdown();
    }

    
}
