//
//  time.hpp
//  kege
//
//  Created by Kenneth Esdaile on 8/23/24.
//

#ifndef kege_time_hpp
#define kege_time_hpp

#include <chrono>

namespace kege {
    
    class Time
    {
    private:
        
        std::chrono::microseconds now()const;
        std::chrono::microseconds _prev_time;

    public:

        double getTime() const;
        Time();
    };

}
#endif /* kege_time_hpp */
