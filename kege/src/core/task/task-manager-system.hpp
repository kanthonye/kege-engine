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

    class TaskManagerSystem
    {
    public:

        static void addTask( const std::function< void() >& task, Task::Status* status = nullptr, Task::Type type = Task::Type::General );
        static void addTaskManager( Task::Type type );
        static bool initialize();
        static void shutdown();

        TaskManagerSystem();
        ~TaskManagerSystem();

    private:

        static TaskManager* createTaskManager( Task::Type type );
        static TaskManager* getTaskManager( Task::Type type );
        static TaskManager* taskManager( Task::Type type );

    private:

        static std::vector< kege::Ref< TaskManager > > _task_managers;
    };


    void executeTask( const std::function< void() >& task, Task::Status* status = nullptr, Task::Type type = Task::Type::General );
}

#endif /* task_manager_system_hpp */
