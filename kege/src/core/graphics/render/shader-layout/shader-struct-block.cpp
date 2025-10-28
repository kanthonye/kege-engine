//
//  shader-buffer-block.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-struct-block.hpp"

namespace kege {

    const ShaderStructField* ShaderStructBlock::get( const std::string& name )const
    {
        auto itr = quick_lookup_map.find( name );
        if ( itr == quick_lookup_map.end() )
        {
            return nullptr;
        }
        return &fields[ itr->second ];
    }

    ShaderStructField* ShaderStructBlock::get( const std::string& name )
    {
        auto itr = quick_lookup_map.find( name );
        if ( itr == quick_lookup_map.end() )
        {
            return nullptr;
        }
        return &fields[ itr->second ];
    }

    const ShaderStructField* ShaderStructBlock::operator[]( int field_index )const
    {
        return &fields[ field_index ];
    }

    ShaderStructField* ShaderStructBlock::operator[]( int field_index )
    {
        return &fields[ field_index ];
    }

    ShaderStructBlock::ShaderStructBlock( const std::vector< ShaderStructField >& p )
    :   fields( p )
    {
        for ( int i = 0 ; i < p.size() ; ++i )
        {
            quick_lookup_map[ p[i].name ] = i;
        }
    }
}
