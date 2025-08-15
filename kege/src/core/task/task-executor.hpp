//
//  task-executor.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#ifndef task_executor_hpp
#define task_executor_hpp

#include "task.hpp"

namespace kege{

    class TaskExecutor : public kege::RefCounter
    {
    public:

        void addTask( const std::function< void() >& task, Task::Status* status );
        void addTask( Task& task );
        bool isIdle();
        void stop();

        ~TaskExecutor();
        TaskExecutor();

    private:

        void run();

    private:

        std::queue< Task > _task_queue;
        std::atomic< bool > _is_running;
        std::mutex _queue_mutex;
        std::thread _thread;
    };

}

#endif /* task_executor_hpp */
