//
//  shader-resource-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "graphics.hpp"
#include "shader-resource-manager.hpp"

namespace kege{

//    bool ShaderResourceManager::update( const std::vector< int32_t >& set_indices, const ShaderResource& resource )
//    {
//        if ( !set_indices.empty() )
//        {
//            for ( int32_t set_index : set_indices )
//            {
//                bool success = _device->updateUniformSet
//                (
//                    _shader_resources[ resource._index ].uniform_set_handles[set_index],
//                    _shader_resources[ resource._index ].resource_sets[set_index]
//                );
//                if( !success )
//                {
//                    return false;
//                }
//            }
//            return true;
//        }
//        else // update every set
//        {
//            return _device->updateUniformSets
//            (
//                _shader_resources[ resource._index ].uniform_set_handles,
//                _shader_resources[ resource._index ].resource_sets
//            );
//        }
//    }
//
//    ShaderResource ShaderResourceManager::create( const UniformSetsDesc& desc )
//    {
//        int index = generate();
//        _shader_resources[index].uniform_set_handles = _device->allocateUniformSets( desc );
//        _shader_resources[index].resource_sets.sets.resize( desc.size() );
//
//        for (int i=0; i<desc.size(); ++i)
//        {
//            UniformSet& set = _shader_resources[index].resource_sets.sets[i];
//            set.elements.resize( desc[i].size() );
//
//            for (int k=0; k<desc[i].size(); ++k)
//            {
//                set.elements[k].binding = desc[i][k].binding;
//            }
//        }
//
//        return ShaderResource( this, index );
//    }
//
//    ShaderResource ShaderResourceManager::create( const UniformSetDesc& desc )
//    {
//        int index = generate();
//        _shader_resources[index].uniform_set_handles = { _device->allocateUniformSet( desc ) };
//        _shader_resources[index].resource_sets.sets.resize( 1 );
//
//        UniformSet& set = _shader_resources[index].resource_sets.sets[0];
//        set.elements.resize( desc.size() );
//
//        for (int k=0; k<desc.size(); ++k)
//        {
//            set.elements[k].binding = desc[k].binding;
//        }
//
//        return ShaderResource( this, index );
//    }
//
//    void ShaderResourceManager::free( const ShaderResource& resource )
//    {
//        const int& index = resource._index;
//        if ( index >= _shader_resources.size() )
//        {
//            return;
//        }
//
//        _shader_resources[ index ].duplicates -= 1;
//        if ( _shader_resources[ index ].duplicates <= 0 )
//        {
//            for (int k=0; k<_shader_resources[ index ].uniform_set_handles.size(); ++k)
//            {
//                _device->freeSet( _shader_resources[ index ].uniform_set_handles[k] );
//            }
//            _shader_resources[ index ].uniform_set_handles = {};
//            _shader_resources[ index ].next = -1;
//            _shader_resources[ index ].prev = -1;
//
//
//            if ( _head < 0 )
//            {
//                _tail = _head = index;
//            }
//            else
//            {
//                _shader_resources[ _head ].prev = index;
//                _shader_resources[ index ].next = _head;
//                _head = index;
//            }
//        }
//    }
//
//    void ShaderResourceManager::incrementReference( ShaderResource* handler )
//    {
//        if ( handler->_index >= _shader_resources.size() ) return;
//        _shader_resources[ handler->_index ].duplicates += 1;
//    }
//
//    int ShaderResourceManager::generate()
//    {
//        int index = _shader_resource_counter;
//        if ( 0 <= _head)
//        {
//            index = _head;
//            _head = _shader_resources[ _head ].next;
//            if ( _head < 0 )
//            {
//                _tail = -1;
//            }
//            else
//            {
//                _shader_resources[ _head ].prev = -1;
//            }
//            _shader_resources[ index ].duplicates = 1;
//            _shader_resources[ index ].next = -1;
//            _shader_resources[ index ].prev = -1;
//            return index;
//        }
//
//        if ( _shader_resource_counter >= _shader_resources.size() )
//        {
//            index = _shader_resource_counter;
//            _shader_resources.push_back({});
//        }
//
//        _shader_resources[ index ].duplicates = 1;
//        _shader_resources[ index ].next = -1;
//        _shader_resources[ index ].prev = -1;
//        _shader_resource_counter += 1;
//        return index;
//    }
//
//    void ShaderResourceManager::initalize( kege::GraphicsDevice* device )
//    {
//        _device = device;
//        _shader_resource_counter = 0;
//        _shader_resources.resize( 500 );
//        for ( int i=1; i<_shader_resources.size(); ++i )
//        {
//            _shader_resources[i-1].duplicates = 0;
//            _shader_resources[i-1].next = i;
//            _shader_resources[i-1].prev = i - 1;
//        }
//        _head = 0;
//        _tail = 499;
//    }
//
//    void ShaderResourceManager::shutdown()
//    {
//        if ( _device )
//        {
//            for ( int i=0; i<_shader_resources.size(); ++i )
//            {
//                for ( int j=0; j<_shader_resources[i].uniform_set_handles.size(); ++j )
//                {
//                    _device->freeSet( _shader_resources[i].uniform_set_handles[j] );
//                }
//            }
//        }
//        _shader_resources.clear();
//    }
//
//    ShaderResourceManager::~ShaderResourceManager()
//    {
//        shutdown();
//    }
//    
//    ShaderResourceManager::ShaderResourceManager()
//    :   _device( nullptr )
//    ,   _shader_resource_counter(0)
//    {}

}
