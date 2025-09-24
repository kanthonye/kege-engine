//
//  shader-resource-binding.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/9/25.
//

#include "graphics.hpp"
#include "shader-resource.hpp"
#include "shader-resource-manager.hpp"

namespace kege{

    bool ShaderResource::update( const std::vector<int32_t> &set_indices )
    {
        return _mngr->update( set_indices, *this );
    }

    const std::vector< int >* ShaderResource::operator ->()const
    {
        return &_mngr->_shader_resources[ _index ].uniform_set_handles;
    }

    const kege::UniformSet& ShaderResource::operator[]( int i ) const
    {
        return _mngr->_shader_resources[ _index ].resource_sets[i];
    }

    kege::UniformSet& ShaderResource::operator[]( int i )
    {
        return _mngr->_shader_resources[ _index ].resource_sets[i];
    }

    ShaderResource::operator bool()const
    {
        return _index < _mngr->_shader_resources.size();
    }

    ShaderResource::ShaderResource( ShaderResourceManager* mngr, uint32_t index )
    :   _mngr( mngr )
    ,   _index( index )
    {}

    ShaderResource& ShaderResource::operator =( ShaderResource&& other)noexcept
    {
        _mngr = other._mngr;
        _index = other._index;
        other._mngr = nullptr;
        other._index = -1;
        return *this;
    }

    ShaderResource& ShaderResource::operator =( const ShaderResource& other )
    {
        _mngr = other._mngr;
        _index = other._index;
        _mngr->incrementReference( this );
        return *this;
    }

    ShaderResource::ShaderResource( const ShaderResource& other )
    :   _mngr( other._mngr )
    ,   _index( other._index )
    {
        _mngr->incrementReference( this );
    }

    ShaderResource::ShaderResource( ShaderResource&& other )noexcept
    :   _mngr( other._mngr )
    ,   _index( other._index )
    {
        other._mngr = nullptr;
        other._index = -1;
    }

    ShaderResource::ShaderResource()
    :   _mngr( nullptr )
    ,   _index( -1 )
    {}

    ShaderResource::~ShaderResource()
    {
        if ( _mngr )
        {
            _mngr->free( *this );
            _mngr = nullptr;
            _index = -1;
        }
    }

}
