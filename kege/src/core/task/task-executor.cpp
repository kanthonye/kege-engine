//
//  task-executor.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#include "task-executor.hpp"

namespace kege{

    void TaskExecutor::addTask( const std::function<void()>& task, Task::Status* status )
    {
        std::lock_guard<std::mutex> lock( _queue_mutex );
        _task_queue.push( Task( task, status ) );
        if ( status )
        {
            status->state = Task::Status::Pending;
        }
    }

    void TaskExecutor::addTask( Task& task )
    {
        std::lock_guard<std::mutex> lock( _queue_mutex );
        _task_queue.push( task );
        if ( task.status )
        {
            task.status->state = Task::Status::Pending;
        }
    }

    void TaskExecutor::stop()
    {
        _is_running = false;
        //_condition.notify_all();
        if ( _thread.joinable() )
        {
            _thread.join();
        }
    }

    bool TaskExecutor::isIdle()
    {
        std::lock_guard<std::mutex> lock( _queue_mutex );
        return _task_queue.empty();
    }

    void TaskExecutor::run()
    {
        while ( _is_running )
        {
            Task task;
            {
                std::unique_lock< std::mutex > lock( _queue_mutex );
                //_condition.wait( lock, [this] { return !_is_running || !_task_queue.empty(); } );
                if (!_is_running || _task_queue.empty())
                    continue;
                
                task = std::move( _task_queue.front() );
                _task_queue.pop();
            }

            if ( task.execute )
            {
                if ( task.status ) 
                {
                    if ( task.status->state == Task::Status::Pending )
                    {
                        {
                            std::unique_lock< std::mutex > lock( task.status->mutex );
                            task.status->state = Task::Status::Executing;
                        }
                        {
                            std::unique_lock< std::mutex > lock( task.status->mutex );
                            task.execute();
                        }
                        {
                            std::unique_lock< std::mutex > lock( task.status->mutex );
                            task.status->state = Task::Status::Completed;
                        }
                    }
                }
                else
                {
                    task.execute(); // Execute the task
                }
            }
        }
    }

    TaskExecutor::~TaskExecutor()
    {
        stop();
    }

    TaskExecutor::TaskExecutor()
    :   _is_running( true )
    {
        _thread = std::thread( &TaskExecutor::run, this );
    }

}
