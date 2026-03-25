//
//  shader-layout.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-layout.hpp"

namespace kege{

    const kege::LayoutBindingDesc* ShaderLayout::getLayoutBindingDesc( const std::string& name )const
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
        {
            kege::Log::warning << "A SetLayout -> " << _name;
            kege::Log::warning << " does not contains a PushBlockDesc with name -> [ "<<name <<" ]." << kege::Log::nl;
            return nullptr;
        }
        return &_set_bindings[ i->second.set_binding_index ];
    }

    const kege::PushBlockDesc* ShaderLayout::getPushBlock( const std::string& name )const
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
        {
            kege::Log::warning << "A SetLayout -> " << _name;
            kege::Log::warning << " does not contains a PushBlockDesc with name -> [ "<<name <<" ]." << kege::Log::nl;
            return nullptr;
        }
        return &_push_block_layout[ i->second.set_binding_index ];
    }
    
    const kege::PushBlockDesc* ShaderLayout::getPushBlock( int index )const
    {
        return &_push_block_layout[ index ];
    }

//    const ref::SetLayout ShaderLayout::getSetLayout( const std::string& name )const
//    {
//        auto i = _quick_set_layout_lookup.find( name );
//        if ( i == _quick_set_layout_lookup.end() )
//        {
//            kege::Log::warning << "A SetLayout -> " << _name;
//            kege::Log::warning << " does not contains a SetLayout with name -> [ "<<name <<" ]." << kege::Log::nl;
//            return nullptr;
//        }
//        return _indexed_set_layouts[ i->second ].set;
//    }
//
//    ref::SetLayout ShaderLayout::getSetLayout( const std::string& name )
//    {
//        auto i = _quick_set_layout_lookup.find( name );
//        if ( i == _quick_set_layout_lookup.end() )
//        {
//            kege::Log::warning << "A SetLayout -> " << _name;
//            kege::Log::warning << " does not contains a SetLayout with name -> [ "<<name <<" ]." << kege::Log::nl;
//            return nullptr;
//        }
//        return _indexed_set_layouts[ i->second ].set;
//    }

    const kege::IndexedSetLayout& ShaderLayout::getSetLayout( int set_index )const
    {
        return _indexed_set_layouts[ set_index ];
    }

    kege::IndexedSetLayout ShaderLayout::getSetLayout( const std::string& binding_name )
    {
        auto i = _quick_lookup.find( binding_name );
        if ( i == _quick_lookup.end() )
        {
            kege::Log::warning << "ShaderLayout -> " << _name;
            kege::Log::warning << " does not contains a SetLayout with binding name -> [ "<<binding_name <<" ]." << kege::Log::nl;
            return {};
        }
        return _indexed_set_layouts[ i->second.set_layout_index ];
    }

    kege::IndexedSetLayout ShaderLayout::getSetLayout( int set_index )
    {
        return _indexed_set_layouts[ set_index ];
    }

    kege::BindSet ShaderLayout::allocateSet( const std::string& name )
    {
        auto i = _quick_lookup.find( name );
        if ( i == _quick_lookup.end() )
        {
            kege::Log::warning << "A SetLayout -> " << _name;
            kege::Log::warning << " does not contains a Set with name -> [ "<<name <<" ]." << kege::Log::nl;
            return {};
        }
        return allocateSet( i->second.set_layout_index );
    }

    kege::BindSet ShaderLayout::allocateSet( int set_index )
    {
        return kege::BindSet
        {
            .set = _indexed_set_layouts[ set_index ].set->allocateSet(),
            .index = _indexed_set_layouts[ set_index ].index
        };
    }

    int32_t ShaderLayout::getSetIndex( const kege::SetLayout* layout )const
    {
        auto i = _set_layout_index_lookup.find( layout );
        if ( i == _set_layout_index_lookup.end() ) return -1;
        return _indexed_set_layouts[ i->second ].set;
    }

    int ShaderLayout::getSetLayoutBindSignature()const
    {
        return _set_layout_bind_signature;
    }

    ShaderLayout::ShaderLayout
    (
        const std::string& name,
        const kege::IndexedSetLayouts& indexed_set_layouts,
        const kege::PushBlockDescs& push_blocks
    )
    :   _indexed_set_layouts( indexed_set_layouts )
    ,   _push_block_layout( push_blocks )
    ,   _name( name )
    ,   _set_layout_bind_signature( 0 )
    {
        for (uint32_t i=0; i<_indexed_set_layouts.size(); ++i)
        {
            for (int j=0; j<_indexed_set_layouts[i].set->_bindings.size(); ++j)
            {
                const BindPointDesc& binding = _indexed_set_layouts[i].set->_bindings[j];
                Entry& entry = _quick_lookup[ binding.name ];

                entry.set_binding_index = static_cast< int >( _set_bindings.size() );
                entry.set_layout_index = _indexed_set_layouts[i].index;
                entry.push_block_index = -1;

                _set_bindings.push_back
                ({
                    .index = _indexed_set_layouts[i].index,
                    .binding = binding
                });
            }

            _set_layout_bind_signature |= (1ULL << _indexed_set_layouts[i].index);
        }

        for (uint32_t i=0; i<push_blocks.size(); ++i)
        {
            const kege::PushBlockDesc& block = push_blocks[i];
            Entry& entry = _quick_lookup[ block.name ];

            entry.set_binding_index = -1;
            entry.set_layout_index = -1;
            entry.push_block_index = i;
        }
    }

    ShaderLayout:: ~ShaderLayout()
    {}

}
