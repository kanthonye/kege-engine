//
//  time.cpp
//  kege
//
//  Created by Kenneth Esdaile on 8/23/24.
//

#include "time.hpp"
namespace kege{
    
    std::chrono::microseconds Time::now()const
    {
        return std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::system_clock::now().time_since_epoch() );
    }

    double Time::getTime() const
    { 
        return (now() - _prev_time).count() / 1000000.0;
    }

    Time::Time()
    {  
        _prev_time = now();
    }

}
