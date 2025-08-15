//
//  task.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#ifndef task_hpp
#define task_hpp

#include <queue>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include "ref.hpp"

namespace kege{

    class TaskExecutor;

    struct Task
    {
        enum class Type { General, Physics, Rendering, Custom };

        class Status
        {
        public:

            enum State{ Idle, Pending, Executing, Completed };
            bool operator==( const Task::Status::State& state );
            bool operator==( const Task::Status& status );
            void endTask();
            Status(): state( State::Idle ) {}

        private:

            friend TaskExecutor;
            std::mutex mutex;
            State state;
        };

        // Optional: Constructor to initialize the task.
        Task& operator=( const Task&& task )
        {
            execute = std::move( task.execute );
            status = task.status;
            return *this;
        }

        // Optional: Constructor to initialize the task.
        Task& operator&&( Task&& task )
        {
            execute = std::move( task.execute );
            status = task.status;
            task.execute = nullptr;
            task.status = nullptr;
            return *this;
        }

        // Optional: Constructor to initialize the task.
        Task( std::function<void()> func, Status* status )
        :   execute( std::move( func ) )
        ,   status( status )
        {}

        // Optional: Constructor to initialize the task.
        Task( const Task& task )
        :   execute( std::move( task.execute ) )
        ,   status( task.status )
        {
        }

        // Optional: Constructor to initialize the task.
        Task( Task&& task )
        :   execute( std::move( task.execute ) )
        ,   status( task.status )
        {
            task.execute = nullptr;
            task.status = nullptr;
        }

        Task()
        :   execute( nullptr )
        ,   status( nullptr )
        {}

        // The actual work to be executed by the task.
        std::function<void()> execute;

        // flag to track task status (e.g., pending, running, completed).
        Status* status;
    };

}

#endif /* task_hpp */
