//
//  task-manager.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#include "task-manager.hpp"

namespace kege{

    void TaskManager::addTask( const std::function< void() >& task, Task::Status* status )
    {
        std::lock_guard<std::mutex> lock( _queue_mutex );
        _task_queue.push( Task( task, status ) );


        /*

         A task manager can have one  or more task executor, so to distribute the work
         evenly between all task executor we cycle through an increasing index counter
         by reseting it to zero when it reach the total count of executors
         
         */
//        _curr_index = _curr_index % _executors.size();
//        _executors[ _curr_index ]->addTask( task, status );
//        _curr_index++;
    }

    void TaskManager::shutdown()
    {
        _running = false;
        if ( _thread.joinable() )
        {
            _thread.join();
        }
        
        std::unique_lock< std::mutex > lock( _queue_mutex );
        for ( int i=0; i<_executors.size(); ++i )
        {
            _executors[ i ]->stop();
            _executors[ i ].clear();
        }
        _executors.clear();
    }

    Task::Type TaskManager::type()const
    {
        return _type;
    }

    void TaskManager::addTaskExecutor()
    {
        _executors.push_back( new TaskExecutor );
    }

    void TaskManager::run()
    {
        _running = true;
        uint32_t executors_index = 0;
        while ( _running )
        {
            std::queue< Task > tasks;
            {
                std::unique_lock< std::mutex > lock( _queue_mutex );
                tasks.swap( _task_queue );
            }

            executors_index = 0;
            while ( !tasks.empty() )
            {
                Task task = std::move( tasks.front() );
                tasks.pop();

                if ( task.execute )
                {
                    executors_index %= _executors.size();
                    _executors[ executors_index ]->addTask( task );
                    executors_index++;
                }
            }
//            Task task;
//            {
//                std::unique_lock< std::mutex > lock( _queue_mutex );
//                //_condition.wait( lock, [this] { return !_is_running || !_task_queue.empty(); } );
//                if (!_running || _task_queue.empty())
//                    continue;
//
//                task = std::move( _task_queue.front() );
//                _task_queue.pop();
//            }

//            if ( task.execute )
//            {
//                task_index = task_index % _executors.size();
//                _executors[ task_index ]->addTask( task );
//                task_index++;
//            }
        }
        
    }

    TaskManager::TaskManager( Task::Type type )
    :   _type( type )
    ,   _curr_index( 0 )
    {
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        addTaskExecutor();
        _thread = std::thread( &TaskManager::run, this );
    }

    TaskManager::~TaskManager()
    {
        shutdown();
    }
}
