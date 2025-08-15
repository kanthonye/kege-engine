//
//  task-manager-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#include "task-manager-system.hpp"

namespace kege{

    void executeTask( const std::function< void() >& task, Task::Status* status, Task::Type type )
    {
        TaskManagerSystem::addTask( task, status, type );
    }




    void TaskManagerSystem::addTask( const std::function< void() >& task, Task::Status* status, Task::Type type )
    {
        taskManager( type )->addTask( task, status );
    }

    void TaskManagerSystem::addTaskManager( Task::Type type )
    {
        if( getTaskManager( type ) == nullptr )
        {
            createTaskManager( type );
        }
    }

    bool TaskManagerSystem::initialize()
    {
        if ( _task_managers.empty() )
        {
            addTaskManager( Task::Type::General );
        }
        return true;
    }

    void TaskManagerSystem::shutdown()
    {
        for ( int i=0; i<_task_managers.size(); ++i )
        {
            _task_managers[ i ]->shutdown();
            _task_managers[ i ].clear();
        }
        _task_managers.clear();
    }

    TaskManager* TaskManagerSystem::createTaskManager( Task::Type type )
    {
        TaskManager* manager = getTaskManager( type );
        if ( manager == nullptr )
        {
            manager = new TaskManager( type );
            _task_managers.push_back( manager );
        }
        //manager->addNewTaskExecutor();
        return manager;
    }

    TaskManager* TaskManagerSystem::getTaskManager( Task::Type type )
    {
        for ( int i=0; i<_task_managers.size(); ++i )
        {
            if ( _task_managers[ i ]->type() == type )
            {
                return _task_managers[ i ].ref();
            }
        }
        return nullptr;
    }

    TaskManager* TaskManagerSystem::taskManager( Task::Type type )
    {
        TaskManager* manager = getTaskManager( type );
        if ( manager == nullptr )
        {
            manager = createTaskManager( type );
        }
        return manager;
    }

    TaskManagerSystem::TaskManagerSystem()
    {
    }

    TaskManagerSystem::~TaskManagerSystem()
    {
        shutdown();
    }

    std::vector< kege::Ref< TaskManager > > TaskManagerSystem::_task_managers;
}
