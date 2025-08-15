//
//  input-commands.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/18/25.
//

#include "input-commands.hpp"

namespace kege{

    double MappedInputs::operator[]( Command command )const
    {
        MappedInputCommands::const_iterator i = _input_commands.find( command );
        if ( i == _input_commands.end() ) return 0.0;
        return i->second->amount;
    }

    void MappedInputs::insert( const InputCommand* command )
    {
        if ( command->amount == 0.0 && command->type == InputCommand::STATE )
        {
            MappedInputCommands::iterator i = _input_commands.find( command->command );
            if ( i != _input_commands.end() )
            {
                _input_commands.erase( i );
                return;
            }
        }
        _input_commands[ command->command ] = command;
    }

    MappedInputs::operator bool()const
    {
        return !_input_commands.empty();
    }

    void MappedInputs::clear()
    {
        std::vector< MappedInputCommands::iterator > removables;
        for( MappedInputCommands::iterator i = _input_commands.begin(); i != _input_commands.end(); i++ )
        {
            if ( i->second->type != InputCommand::STATE )
            {
                removables.push_back(i);
            }
        }

        for( MappedInputCommands::iterator& i : removables )
        {
            _input_commands.erase( i );
        }
    }

    double MappedInputs::coordX()const
    {
        return _pointer.x;
    }
    double MappedInputs::coordY()const
    {
        return _pointer.y;
    }
    MappedInputs::MappedInputs()
    :   _pointer(0.0, 0.0)
    {}
}
