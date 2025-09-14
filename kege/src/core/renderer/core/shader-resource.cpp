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

//    const ShaderResourceSet* ShaderResource::operator ->()const
//    {
////        if ( _index >= _mngr->_shader_resources.size() ) return nullptr;
////        frame_index = _mngr->_graphics->getCurrFrameIndex() % _mngr->_shader_resources[ _index ].resources.size();
////        return &_mngr->_shader_resources[ _index ].resources[ frame_index ];
//    }
//
//    ShaderResourceSet* ShaderResource::operator ->()
//    {
////        if ( _index >= _mngr->_shader_resources.size() ) return nullptr;
////        frame_index = _mngr->_graphics->getCurrFrameIndex() % _mngr->_shader_resources[ _index ].resources.size();
////        return &_mngr->_shader_resources[ _index ].resources[ frame_index ];
//    }

    const kege::Uniform& ShaderResource::operator[]( int i ) const
    {
        if ( _index >= _mngr->_shader_resources.size() )
        {}
        return _mngr->_shader_resources[ _index ].resources[ i ];
    }

    kege::Uniform& ShaderResource::operator[]( int i )
    {
        if ( _index >= _mngr->_shader_resources.size() )
        {}
        return _mngr->_shader_resources[ _index ].resources[ i ];
    }
    const UniformBindingElements& ShaderResource::resources()const
    {
        if ( _index >= _mngr->_shader_resources.size() )
        {}
        return _mngr->_shader_resources[ _index ].resources;
    }

    int32_t ShaderResource::operator*() const
    {
        if ( _index >= _mngr->_shader_resources.size() )
        {
            return -1;
        }
        return _mngr->_shader_resources[ _index ].handle;
    }

//    kege::ShaderResource ShaderResource::handle()const
//    {
//        if ( _index >= _mngr->_shader_resources.size() ) return {};
//        int frame_index = _mngr->_graphics->getCurrFrameIndex() % _mngr->_shader_resources[ _index ].resources.size();
//        return &_mngr->_shader_resources[ _index ].resources[ frame_index ].handle;
//    }

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
            _mngr->free( 1, this );
            _mngr = nullptr;
            _index = -1;
        }
    }

}
