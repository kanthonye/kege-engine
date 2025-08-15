//
//  log.cpp
//  vm
//
//  Created by Kenneth Esdaile on 6/13/25.
//

#include "log.hpp"

namespace kege{

    Log Log::warning("[ WARNING ] ", "\033[31m" );
    Log Log::error("[ ERROR ] ", "\033[31m" );
    Log Log::debug("[ INFO ] ", "\033[36m" );
    Log Log::info("[ INFO ] ", "\033[37m" );

    Log::NL Log::nl;

    void Log::flush( FILE* file )
    {
        if ( file != nullptr )
        {
            for ( const Entry* entry : logs )
            {
                for ( const std::string& str : *entry )
                {
                    fwrite( str.data(), sizeof(char), str.length(), file );
                }
            }
        }
        else
        {
            if ( name.empty() )
            {
                for ( const Entry* entry : logs )
                {
                    for ( const std::string& str : *entry )
                    {
                        std::cout << str;
                    }
                    std::cout <<"\n";
                }
            }
            else
            {
                for ( const Entry* entry : logs )
                {
                    if( entry->empty() ) continue;
                    std::cout << name <<": ";
                    for ( const std::string& str : *entry )
                    {
                        std::cout << str;
                    }
                    std::cout <<"\n";
                }
            }
        }
        for ( Entry* entry : logs )
        {
            delete entry;
        }
        logs.clear();
    }

    Log::Log( const std::string& name, const std::string& color )
    :   color( color )
    ,   name( name )
    ,   entry_index( 0 )
    {
    }

    Log::~Log()
    {
        for ( Entry* entry : logs )
        {
            delete entry;
        }
        logs.clear();
    }

    Log::Log()
    :   name()
    ,   entry_index( 0 )
    {
    }

}
