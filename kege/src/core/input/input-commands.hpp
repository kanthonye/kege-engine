//
//  input-commands.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/18/25.
//

#ifndef input_commands_hpp
#define input_commands_hpp

#include <unordered_map>
#include "../memory/ref.hpp"
#include "../math/algebra/vectors.hpp"
#include "../utils/kege-string.hpp"
#include "../input/keycodes.hpp"
#include "../input/input-actions.hpp"

namespace kege{

    struct InputCommand
    {
        enum{ ACTION, STATE, RANGE, TOTAL_TYPES };
        uint32_t command;
        double amount;
        int type;
    };

    class InputContextManager;

    class MappedInputs
    {
    public:

        typedef uint32_t Command;

        double operator[]( Command command )const;
        operator bool()const;

        void insert( const InputCommand* info );
        double coordX()const;
        double coordY()const;
        void clear();
        
        MappedInputs();

    private:

        typedef std::unordered_map< Command, const InputCommand* > MappedInputCommands;
        MappedInputCommands _input_commands;
        kege::dvec2 _pointer;

        friend InputContextManager;
    };

}

#endif /* input_commands_hpp */
