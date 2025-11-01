//
//  shader-buffer-block.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-struct-block.hpp"

namespace kege {

//    ShaderStructBlock& ShaderStructBlock::operator =( const kege::ShaderStructBlock& other )
//    {
//        fields = other.fields;
//        name = other.name;
//        type = other.type;
//        quick_lookup_map = other.quick_lookup_map;
//        return *this;
//    }
//    ShaderStructBlock& ShaderStructBlock::operator =( kege::ShaderStructBlock&& other )
//    {
//        other.quick_lookup_map.swap( quick_lookup_map );
//        other.fields.swap( fields );
//        other.name.swap( name );
//        type = other.type;
//        return *this;
//    }

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

    kege::ShaderStructType ShaderStructBlock::getType()
    {
        return type;
    }

    ShaderStructBlock::ShaderStructBlock
    (
        ShaderStructType t,
        const std::string& n,
        const std::vector< ShaderStructField >& f
    )
    :   fields( f )
    ,   name( n )
    ,   type( t )
    {
        for ( int i = 0 ; i < f.size() ; ++i )
        {
            quick_lookup_map[ f[i].name ] = i;
        }
    }

//    ShaderStructBlock::ShaderStructBlock( const kege::ShaderStructBlock& other )
//    :   fields( other.fields )
//    ,   name( other.name )
//    ,   type( other.type )
//    ,   quick_lookup_map( other.quick_lookup_map )
//    {}
//
//    ShaderStructBlock::ShaderStructBlock( kege::ShaderStructBlock&& other )
//    :   type( other.type )
//    {
//        other.quick_lookup_map.swap( quick_lookup_map );
//        other.fields.swap( fields );
//        other.name.swap( name );
//    }

}
