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




namespace kege{

    const UniformResourceSet& ShaderResrc::operator[]( int set_index )const
    {
        return layout.resources[ set_index ];
    }

    UniformResourceSet& ShaderResrc::operator[]( int set_index )
    {
        return layout.resources[ set_index ];
    }

    const ShaderBindings& ShaderResrc::getShaderBindings()const
    {
        return bindings;
    }

    void ShaderResrc::update( const std::vector< int >& sets )
    {
        if ( !sets.empty() )
        {
            for ( int i=0; i<sets.size(); ++i )
            {
                updateSet( sets[i] );
            }
        }
        else
        {
            for ( int set_index=0; set_index<layout.resources.size(); ++set_index )
            {
                updateSet( set_index );
            }
        }
    }

    void ShaderResrc::updateSet( int set_index )
    {
        for ( int i = 0; i < layout.resources[ set_index ].size(); ++i )
        {
            layout.graphics->updateSet
            (
                bindings[ set_index ].resource,
                layout.resources[ set_index ]
            );
        }
    }

    void ShaderResrc::release()
    {
        for (int set_index=0; set_index<layout.resources.size(); ++set_index)
        {
            for (int binding_index=0; binding_index<layout.resources[set_index].size(); ++binding_index)
            {
                switch ( layout.resources[set_index][binding_index].uniform.type )
                {
                    case Uniform::BUFFER:
                    {
                        for (int u=0; u<layout.resources[set_index][binding_index].uniform.buffers.size(); ++u)
                        {
                            layout.graphics->destroyBuffer( layout.resources[set_index][binding_index].uniform.buffers[u].buffer );
                        }
                        break;
                    }

                    case Uniform::IMAGE:
                    {
                        for (int u=0; u<layout.resources[set_index][binding_index].uniform.images.size(); ++u)
                        {
                            layout.graphics->destroyImage( layout.resources[set_index][binding_index].uniform.images[u].image );
                        }
                        break;
                    }

                    case Uniform::BUFFER_VIEW:
                    {
                        for (int u=0; u<layout.resources[set_index][binding_index].uniform.buffer_views.size(); ++u)
                        {
                            layout.graphics->destroyBufferView( layout.resources[set_index][binding_index].uniform.buffer_views[u] );
                        }
                        break;
                    }

                    default: break;
                }
            }
            layout.resources[set_index].clear();
            layout.graphics->freeSet( bindings[ set_index ].resource );
        }
        layout.resources.clear();
    }

    ShaderResrc::ShaderResrc( Graphics* graphics, const UniformDescriptorSets& descriptors )
    {
        layout.descriptors = descriptors;
        layout.resources.resize( descriptors.size() );
        bindings.resize( descriptors.size() );

        for ( int set_index = 0; set_index < layout.resources.size(); ++set_index )
        {
            bindings[ set_index ].set_index = layout.descriptors[ set_index ].set;
            
            bindings[ set_index ].resource = layout.graphics->makeSet
            ( layout.descriptors[ set_index ], layout.resources[ set_index ] );
        }
    }

    ShaderResrc::ShaderResrc( const UniformResourceLayout& layout )
    :   layout( layout )
    {
        bindings.resize( layout.descriptors.size() );
        for ( int set_index = 0; set_index < layout.resources.size(); ++set_index )
        {
            bindings[ set_index ].set_index = layout.descriptors[ set_index ].set;
            bindings[ set_index ].resource = layout.graphics->makeSet
            ( layout.descriptors[ set_index ], layout.resources[ set_index ] );
        }
    }

    ShaderResrc::ShaderResrc()
    {}

    ShaderResrc::~ShaderResrc()
    {
        release();
    }
}
