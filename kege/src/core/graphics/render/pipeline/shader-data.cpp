//
//  shader-data.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-data.hpp"
#include "shader-layout.hpp"
#include "shader-pipeline.hpp"

namespace kege{

    void ShaderData::setPushBlock( size_t offset, size_t size, const void* data )
    {
        memcpy( _push_block_data.data() + offset, data, size );
    }

    bool ShaderData::setBuffers( int set_index, int binding_index, const BufferBindings& bindings, int frame )
    {
        auto i = _sets.find( set_index );
        if ( i == _sets.end() ) return false;
        return i->second.set->bind( frame, binding_index, bindings );
    }

    bool ShaderData::setBuffers( const std::string& block_name, const BufferBindings& bindings, int frame )
    {
        const kege::LayoutBindingDesc* set = _pipeline->getShaderLayout()->getLayoutBindingDesc( block_name );
        if ( !set ) return false;
        return setBuffers( set->index, set->binding.index, bindings, frame );
    }

    bool ShaderData::setImages( int set_index, int binding_index, const ImageBindings& bindings, int frame )
    {
        auto i = _sets.find( set_index );
        if ( i == _sets.end() ) return false;
        return i->second.set->bind( frame, binding_index, bindings );
    }

    bool ShaderData::setImages( const std::string& block_name, const ImageBindings& bindings, int frame )
    {
        const kege::LayoutBindingDesc* set = _pipeline->getShaderLayout()->getLayoutBindingDesc( block_name );
        if ( !set ) return false;
        return setImages( set->index, set->binding.index, bindings, frame );
    }

    const std::map< int, kege::IndexedSet >& ShaderData::getShaderSets()const
    {
        return _sets;
    }
    
    const ref::ShaderPipeline& ShaderData::getShaderPipeline()const
    {
        return _pipeline;
    }

    void ShaderData::update()
    {
        for ( auto& [index, set] : _sets )
        {
            set.set->update();
        }
    }

    ShaderData::ShaderData( const ref::ShaderPipeline& p, const kege::SetNames& s )
    :   _pipeline( p )
    {
        for (const std::string& set_name : s )
        {
            IndexedSet set = _pipeline->getShaderLayout()->allocateSet( set_name );
            _sets[ set.index ] = { .index = set.index, .set = set.set };
        }
    }

    ShaderData::ShaderData( const ref::ShaderPipeline& p, const kege::SetIndices& s )
    :   _pipeline( p )
    {
        for (int set_index : s )
        {
            _sets[ set_index ] = _pipeline->getShaderLayout()->allocateSet( set_index );
        }
    }

    ShaderData::~ShaderData()
    {
        _sets.clear();
        _pipeline.clear();
    }

}
