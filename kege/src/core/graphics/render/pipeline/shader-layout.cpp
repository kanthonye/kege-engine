//
//  shader-layout.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-layout.hpp"

namespace kege{

//    const ShaderStructField* ShaderLayout::getField( const std::string& name, const std::string& field )const
//    {
//        auto i = _quick_lookup_map.find( name );
//        if ( i == _quick_lookup_map.end() )
//            return nullptr;
//        return _shader_struct_blocks[ i->second.index ]->get( field );
//    }
//
//    ShaderStructField* ShaderLayout::getField( const std::string& name, const std::string& field )
//    {
//        auto i = _quick_lookup_map.find( name );
//        if ( i == _quick_lookup_map.end() )
//            return nullptr;
//        return _shader_struct_blocks[ i->second.index ]->get( field );
//    }

    const kege::PushBlock* ShaderLayout::getPushBlock( const std::string& name )const
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
            return nullptr;
        if ( i->second.type == LookUpEntry::PUSH_CONSTANT )
        {
            return getPushBlock( i->second.index );
        }
        return nullptr;
    }
    
    const kege::PushBlock* ShaderLayout::getPushBlock( int index )const
    {
        return &_push_block_layout[ index ];
    }

    const ref::SetLayout ShaderLayout::getSetLayout( const std::string& name )const
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
            return nullptr;
        if ( i->second.type == LookUpEntry::LAYOUT )
        {
            return _indexed_set_layouts[ i->second.index ].set;
        }
        return nullptr;
    }

    ref::SetLayout ShaderLayout::getSetLayout( const std::string& name )
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
            return nullptr;
        if ( i->second.type == LookUpEntry::LAYOUT )
        {
            return _indexed_set_layouts[ i->second.index ].set;
        }
        return nullptr;
    }

    const ref::SetLayout& ShaderLayout::getSetLayout( int set_index )const
    {
        return _indexed_set_layouts[ set_index ].set;
    }

    ref::SetLayout ShaderLayout::getSetLayout( int set_index )
    {
        return _indexed_set_layouts[ set_index ].set;
    }

    int32_t ShaderLayout::getSetIndex( const kege::SetLayout* layout )const
    {
        auto i = _set_index_mapper.find( layout );
        if ( i == _set_index_mapper.end() ) return -1;
        return i->second;
    }
    
    ShaderLayout::ShaderLayout
    (
        const std::string& name,
        const kege::IndexedSetLayouts& indexed_set_layouts,
        const kege::PushBlockLayout& push_block_layout
    )
    :   _indexed_set_layouts( indexed_set_layouts )
    ,   _push_block_layout( push_block_layout )
    ,   _name( name )
    {
        for (uint32_t i=0; i<_indexed_set_layouts.size(); ++i)
        {
            for (int j=0; j<_indexed_set_layouts[i].set->_bindings.size(); ++j)
            {
                const std::string& name = _indexed_set_layouts[i].set->_bindings[j].name;
                _quick_lookup[ name ] = LookUpEntry
                {
                    .type = LookUpEntry::LAYOUT,
                    .index = i
                };
            }
            _set_index_mapper[ _indexed_set_layouts[i].set.ref() ] = _indexed_set_layouts[i].index;
        }
        
        for (uint32_t i=0; i<_push_block_layout.size(); ++i)
        {
            _quick_lookup[ _push_block_layout[ i ].name ] = LookUpEntry
            {
                .type = LookUpEntry::PUSH_CONSTANT,
                .index = i
            };
        }
    }


    ShaderLayout:: ~ShaderLayout()
    {}

}
