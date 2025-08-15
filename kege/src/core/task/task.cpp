//
//  task.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/18/24.
//

#include "task.hpp"

namespace kege{

    bool Task::Status::operator==( const Task::Status::State& state )
    {
        std::unique_lock< std::mutex > lock( this->mutex );
        return this->state = state;
    }
    
    bool Task::Status::operator==( const Task::Status& status )
    {
        std::unique_lock< std::mutex > lock( this->mutex );
        return this->state = status.state;
    }
    
    void Task::Status::endTask()
    {
        std::unique_lock< std::mutex > lock( this->mutex );
        this->state = Completed;
    }

}
