//
//  settings.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "settings.hpp"

namespace kege{

    void Setting::insert( const std::string& name, kege::Ref< Setting > setting )
    {
        _settings[ name ] = setting;
    }

    Setting* Setting::operator[]( const std::string& name )
    {
        auto m = _settings.find( name );
        return ( m != _settings.end() ) ? m->second.ref() : nullptr;
    }

    void Setting::remove(const std::string& name )
    {
        auto m = _settings.find( name );
        if ( m != _settings.end() )
        {
            _settings.erase( m );
        }
    }

    Setting::Setting( void* data ): _data( data )
    {}

}
