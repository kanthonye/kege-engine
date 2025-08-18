//
//  input-context-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/18/25.
//

#ifndef input_context_manager_hpp
#define input_context_manager_hpp

#include <string>
#include <unordered_map>
#include "../../core/input/inputs.hpp"
#include "../../core/input/input-context.hpp"
#include "../../core/input/input-context-loader.hpp"
#include "../../core/input/user-input-receiver.hpp"
#include "../graphics/core/graphics-window.hpp"

namespace kege{

    class InputContextManager : public RefCounter
    {
    public:

        bool addContext( const kege::string& sid, kege::Ref< kege::InputContext > context );

        std::vector< kege::Input >& getCurrentInputs();
        MappedInputs& getMappedInputs();

        bool initialize( GraphicsWindow* window );
        void updateCurrentInputs();
        void shutdown();

        InputContextManager();

    private:

        void process( const std::vector< Input >& inputs );
        void update( uint16_t keycode, double value );

    private:

        std::unordered_map< std::string, kege::Ref< kege::InputContext > > _contexts;
        std::vector< kege::InputContext* > _active_contexts;
        MappedInputs _mapped_inputs;

        kege::UserInputReceiver _user_input_receiver;
        std::vector< kege::Input > _current_inputs;
    };

}

#endif /* input_context_manager_hpp */
