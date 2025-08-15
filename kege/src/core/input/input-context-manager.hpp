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

namespace kege{

    class InputContextManager
    {
    public:

        bool addContext( const kege::string& sid, kege::Ref< kege::InputContext > context );
        void process( const std::vector< Input >& inputs );
        void update( uint16_t keycode, double value );

        void setMappedInputs( MappedInputs* inputs );
        void shutdown();

        InputContextManager();
        
    private:

        std::unordered_map< std::string, kege::Ref< kege::InputContext > > _contexts;
        std::vector< kege::InputContext* > _active_contexts;
        MappedInputs* _mapped_inputs;
    };

}

#endif /* input_context_manager_hpp */
