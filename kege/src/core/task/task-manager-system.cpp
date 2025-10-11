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
        QueueManagerSystem::addTask( task, status, type );
    }




    void QueueManagerSystem::addTask( const std::function< void() >& task, Task::Status* status, Task::Type type )
    {
        taskManager( type )->addTask( task, status );
    }

    void QueueManagerSystem::addQueueManager( Task::Type type )
    {
        if( getQueueManager( type ) == nullptr )
        {
            createQueueManager( type );
        }
    }

    bool QueueManagerSystem::initialize()
    {
        if ( _task_managers.empty() )
        {
            addQueueManager( Task::Type::General );
        }
        return true;
    }

    void QueueManagerSystem::shutdown()
    {
        for ( int i=0; i<_task_managers.size(); ++i )
        {
            _task_managers[ i ]->shutdown();
            _task_managers[ i ].clear();
        }
        _task_managers.clear();
    }

    QueueManager* QueueManagerSystem::createQueueManager( Task::Type type )
    {
        QueueManager* manager = getQueueManager( type );
        if ( manager == nullptr )
        {
            manager = new QueueManager( type );
            _task_managers.push_back( manager );
        }
        //manager->addNewTaskExecutor();
        return manager;
    }

    QueueManager* QueueManagerSystem::getQueueManager( Task::Type type )
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

    QueueManager* QueueManagerSystem::taskManager( Task::Type type )
    {
        QueueManager* manager = getQueueManager( type );
        if ( manager == nullptr )
        {
            manager = createQueueManager( type );
        }
        return manager;
    }

    QueueManagerSystem::QueueManagerSystem()
    {
    }

    QueueManagerSystem::~QueueManagerSystem()
    {
        shutdown();
    }

    std::vector< kege::Ref< QueueManager > > QueueManagerSystem::_task_managers;
}
