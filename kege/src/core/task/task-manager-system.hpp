//
//  task-manager-system.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#ifndef task_manager_system_hpp
#define task_manager_system_hpp

#include "task-manager.hpp"

namespace kege{

    class QueueManagerSystem
    {
    public:

        static void addTask( const std::function< void() >& task, Task::Status* status = nullptr, Task::Type type = Task::Type::General );
        static void addQueueManager( Task::Type type );
        static bool initialize();
        static void shutdown();

        QueueManagerSystem();
        ~QueueManagerSystem();

    private:

        static QueueManager* createQueueManager( Task::Type type );
        static QueueManager* getQueueManager( Task::Type type );
        static QueueManager* taskManager( Task::Type type );

    private:

        static std::vector< kege::Ref< QueueManager > > _task_managers;
    };


    void executeTask( const std::function< void() >& task, Task::Status* status = nullptr, Task::Type type = Task::Type::General );
}

#endif /* task_manager_system_hpp */
