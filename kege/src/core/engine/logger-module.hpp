//
//  logger-module.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/17/25.
//

#ifndef kege_logger_module_hpp
#define kege_logger_module_hpp

#include "../utils/log.hpp"
#include "module.hpp"

namespace kege{

    class LoggerModule : public Module
    {
    public:

        operator bool()const{ return true; }
        bool initialize()override;
        void shutdown()override;
        void add()override;

        LoggerModule( kege::Engine* engine );
    };

}
#endif /* kege_logger_module_hpp */
