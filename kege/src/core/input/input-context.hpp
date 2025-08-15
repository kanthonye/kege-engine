//
//  input-context.hpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/17/24.
//

#ifndef kege_input_context_hpp
#define kege_input_context_hpp

#include <unordered_map>
#include "../memory/ref.hpp"
#include "../container/kege-string.hpp"
#include "../input/keycodes.hpp"
#include "../input/input-commands.hpp"

namespace kege{

    class InputContext : public kege::RefCounter
    {
    public:

        void map( uint16_t keycode, const kege::InputCommand& in );
        const kege::InputCommand* get( uint16_t keycode )const;
        kege::InputCommand* get( uint16_t keycode );
        ~InputContext();

    private:

        typedef std::unordered_map< int, kege::InputCommand > InputMap;
        InputMap _inputs;
    };

    struct AddInputContext
    {
        kege::string sid;
        kege::Ref< kege::InputContext > context;
    };
}

#endif /* kege_input_context_hpp */
