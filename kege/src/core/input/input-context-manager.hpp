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
#include "inputs.hpp"
#include "input-manager.hpp"
#include "input-context.hpp"

namespace kege{

    class InputContextManager : public RefCounter
    {
    public:

        bool addContext( const kege::string& sid, kege::Ref< kege::InputContext > context );

        MappedInputs& getMappedInputs();

        bool initialize();
        void shutdown();

        void update();

        InputContextManager(kege::InputManager* input_manager);

    private:

        void update( uint16_t keycode, double value );

    private:

        std::unordered_map< std::string, kege::Ref< kege::InputContext > > _contexts;
        std::vector< kege::InputContext* > _active_contexts;
        MappedInputs _mapped_inputs;

        kege::InputManager* _input_manager;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::InputContextManager > InputContextManager;
}
#endif /* input_context_manager_hpp */
