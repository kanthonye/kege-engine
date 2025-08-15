//
//  input-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef input_system_hpp
#define input_system_hpp

#include "../../core/system/system.hpp"
#include "../../core/input/input-context.hpp"

namespace kege{

    class CoreInput : public CoreSystem< kege::UserInputReceiver >
    {
    public:

        CoreInput( kege::Engine* engine );

        std::vector< kege::Input >& getCurrentInputs();
        kege::InputContextManager& getContextManager();
        kege::MappedInputs& getMappedInputs();

        bool initialize();
        void shutdown();
        void update();

        kege::MappedInputs _mapped_inputs;
        kege::InputContextManager _context_manager;
        std::vector< kege::Input > _current_inputs;
    };
}


#endif /* input_system_hpp */
