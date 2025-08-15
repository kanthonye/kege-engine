//
//  task-manager.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#ifndef task_manager_hpp
#define task_manager_hpp

#include "task-executor.hpp"

namespace kege{

    class TaskManager : public kege::RefCounter
    {
    public:

        void addTask( const std::function< void() >& task, Task::Status* status );
        void shutdown();

        Task::Type type()const;

        TaskManager( Task::Type type );
        ~TaskManager();

    private:

        void addTaskExecutor();
        void run();

    private:

        std::vector< kege::Ref< TaskExecutor > > _executors;
        Task::Type _type;

        std::queue< Task > _task_queue;
        
        std::mutex  _queue_mutex;
        std::thread _thread;

        uint32_t _curr_index;
        bool _running;
    };

}

#endif /* task_manager_hpp */
