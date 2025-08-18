//
//  input-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef input_system_hpp
#define input_system_hpp

#include "module.hpp"
#include "../../core/input/input-context.hpp"
#include "../../core/input/user-input-receiver.hpp"

namespace kege{

    class InputModule : public Module
    {
    public:

        InputModule( kege::Engine* engine );

        const kege::Ref< kege::InputContextManager > operator ->() const { return _module; }
        kege::Ref< kege::InputContextManager > operator ->() { return _module; }
        operator bool()const{ return _module != nullptr; }

        bool initialize();
        void shutdown();
        void add();

    private:

        kege::Ref< kege::InputContextManager > _module;
    };
}


#endif /* input_system_hpp */
