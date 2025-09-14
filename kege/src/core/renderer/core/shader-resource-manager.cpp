//
//  shader-resource-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "graphics.hpp"
#include "shader-resource-manager.hpp"

namespace kege{

//    bool ShaderResourceManager::allocate( const std::vector< kege::UniformDesc >& info, int quantity, ShaderResource* resource )
//    {
//        UniformSetLayout usl = createDescriptorSetLayout( info );
//        return allocate( usl, quantity, resource );
//    }
//
//    bool ShaderResourceManager::allocate( const UniformSetLayout& layout, int quantity, ShaderResource* resource )
//    {
//        std::vector< ShaderResource > handles( quantity );
//        if ( _device->allocateDescriptors( layout, quantity, handles.data() ) )
//        {
//            generate( quantity, resource, handles.data() );
//            return true;
//        };
//        return false;
//    }
//
//    ShaderResource ShaderResourceManager::allocate( const std::vector< kege::UniformDesc >& info )
//    {
//        UniformSetLayout usl = createDescriptorSetLayout( info );
//        return allocate( usl );
//    }
//
//    ShaderResource ShaderResourceManager::allocate( const UniformSetLayout& layout )
//    {
//        ShaderResource resource;
//        ShaderResource handle;
//        if ( _device->allocateDescriptors( layout, 1, &handle ) )
//        {
//            generate( 1, &resource, &handle );
//            return resource;
//        };
//        return {};
//    }
//
//    UniformSetLayout ShaderResourceManager::createDescriptorSetLayout( const std::vector< UniformDesc >& info )
//    {
//        size_t key = kege::hash( info );
//        auto m = _uniform_set_layouts.find( key );
//        if ( m == _uniform_set_layouts.end() )
//        {
//            UniformSetLayout usl = _device->createDescriptorSetLayout( info );
//            _uniform_set_layouts[ info ] = usl;
//            return usl;
//        }
//        return m->second;
//    }
//
//    void ShaderResourceManager::destroyUniformSetLayout( UniformSetLayout& handle )
//    {
//        size_t key = kege::hash( info );
//        auto m = _uniform_set_layouts.find( key );
//        if ( m != _uniform_set_layouts.end() )
//        {
//            UniformSetLayout usl = _device->createDescriptorSetLayout( info );
//            _uniform_set_layouts[ info ] = usl;
//            return usl;
//        }
//        _device->destroyUniformSetLayout( handle );
//    }
//


//    bool ShaderResourceManager::create( const std::vector< kege::UniformResourceSet >& uniforms, ShaderResource* resources )
//    {
//        for ( int i = 0; i < uniforms.size(); ++i )
//        {
//            
//        }
//    }
//
//    ShaderResource ShaderResourceManager::create( const kege::UniformResourceSet& uniform )
//    {
//        ShaderResource resource = allocate( uniform.frames_in_flight, uniform.descriptors );
//
//        for ( int i = 0; i < uniform.frames_in_flight; ++i )
//        {
//            for ( int k = 0; k < uniform.descriptors.size(); ++k )
//            {
//                resource[i]->bindings[k].resource = uniform.resources[k];
//            }
//            _device->updateDescriptorSet( resource[i]->handle, resource[i]->bindings );
//        }
//
//        return resource;
//    }

//    ShaderResource ShaderResourceManager::allocate( int frames_in_flight, const std::vector< kege::UniformDesc >& bindings )
//    {
//        ShaderResourceLayoutObject* layout;
//
//        auto m = _uniform_set_layouts.find( bindings );
//        if ( m == _uniform_set_layouts.end() )
//        {
//            int index = static_cast< int >( _shader_set_layouts.size() );
//            _uniform_set_layouts[ bindings ] = index;
//            _shader_set_layouts.push_back({});
//
//            layout = &_shader_set_layouts[ _shader_set_layouts.size() - 1 ];
//            layout->handle = _device->createDescriptorSetLayout( bindings );
//            layout->freed.head = -1;
//            layout->freed.tail = -1;
//            layout->id = index;
//        }
//        else
//        {
//            layout = &_shader_set_layouts[ m->second ];
//        }
//
//        uint32_t index;
//        ShaderResourceObject* sro;
//        if ( 0 <= layout->freed.head )
//        {
//            index = layout->freed.head;
//            sro = &_shader_resources[ index ];
//            layout->freed.head = _shader_resources[ layout->freed.head ].next;
//        }
//        else
//        {
//            index = generate();
//            sro = &_shader_resources[ index ];
//        }
//
////        sro->shader_set_layout_id = layout->id;
////        ShaderResource descriptor[ frames_in_flight ];
////        _device->allocateDescriptors( layout->handle, frames_in_flight, descriptor );
////
////        if ( sro->resources.size() != frames_in_flight )
////        {
////            sro->resources.resize( frames_in_flight );
////        }
////
////        for ( int i = 0; i < frames_in_flight; ++i )
////        {
////            sro->resources[i].handle = descriptor[i];
////            sro->resources[i].bindings.resize( bindings.size() );
////            for ( int k = 0; k < bindings.size(); ++k )
////            {
////                sro->resources[i].bindings[k] = {};
////                sro->resources[i].bindings[k].binding = bindings[i].binding;
////                sro->resources[i].bindings[k].descriptor_type = bindings[i].descriptor_type;
////            }
////        }
//
//        return ShaderResource( this, index );
//    }

//    bool ShaderResourceManager::update( int frame_index, ShaderResource& handler )
//    {
//        if ( handler._index >= _shader_resources.size() )
//        {
//            return false;
//        }
//
//        ShaderResourceObject& node = _shader_resources[ handler._index ];
//        return _device->updateDescriptorSet
//        (
//            node.resources[ frame_index ].handle,
//            node.resources[ frame_index ].bindings
//        );
//    }
//
//    bool ShaderResourceManager::update( ShaderResource& resource )
//    {
//        if ( resource._index >= _shader_resources.size() )
//        {
//            return false;
//        }
//
//        ShaderResourceObject& node = _shader_resources[ resource._index ];
//        for (int frame_index = 0; frame_index < node.resources.size(); ++frame_index )
//        {
//            bool state = _device->updateDescriptorSet
//            (
//                node.resources[ frame_index ].handle,
//                node.resources[ frame_index ].bindings
//            );
//            if ( !state )
//            {
//                return false;
//            }
//        }
//        return true;
//    }

    void ShaderResourceManager::free( int quantity, const ShaderResource* resources )
    {
        for (int i = 0; i < quantity; ++i )
        {
            if ( resources[i]._index >= _shader_resources.size() )
            {
                continue;
            }
            _shader_resources[ resources[i]._index ].duplicates -= 1;
            if ( _shader_resources[ resources[i]._index ].duplicates <= 0 )
            {
                _device->freeDescriptor( _shader_resources[ resources[i]._index ].handle );
                _shader_resources[ resources[i]._index ].handle = -1;
                _shader_resources[ resources[i]._index ].next = -1;
                _shader_resources[ resources[i]._index ].prev = -1;

                if ( _freed.head < 0 )
                {
                    _freed.tail = _freed.head = resources[i]._index;
                }
                else
                {
                    _shader_resources[ _freed.tail ].next = resources[i]._index;
                    _shader_resources[ resources[i]._index ].prev = _freed.tail;
                    _freed.tail = resources[i]._index;
                }
            }
        }
    }

    void ShaderResourceManager::incrementReference( ShaderResource* handler )
    {
        if ( handler->_index >= _shader_resources.size() ) return;
        _shader_resources[ handler->_index ].duplicates += 1;
    }

    void ShaderResourceManager::generate( int quantity, int32_t* descriptor_ids, ShaderResource* resources )
    {
        for ( int i=0; i<quantity; ++i )
        {
            uint32_t index = _shader_resource_counter;
            _shader_resource_counter += 1;

            if ( index >= _shader_resources.size() )
            {
                _shader_resources.resize( (index * 2) + 1 );
            }

            _shader_resources[ index ].handle = descriptor_ids[i];
            _shader_resources[ index ].duplicates = 1;
            _shader_resources[ index ].next = -1;
            _shader_resources[ index ].prev = -1;

            resources[i]._index = index;
            resources[i]._mngr = this;
        }
    }

    void ShaderResourceManager::initalize( kege::GraphicsDevice* device )
    {
        _device = device;
        _shader_resource_counter = 0;
    }

    void ShaderResourceManager::shutdown()
    {
//        if ( _device )
//        {
//            for ( ShaderResourceObject& bindings : _shader_resources )
//            {
//                for (int i = 0; i < bindings.resources.size(); ++i )
//                {
//                    _device->freeDescriptorSet( bindings.resources[i].handle );
//                }
//            }
//            _shader_resources.clear();
//            _device = nullptr;
//        }
    }

    ShaderResourceManager::~ShaderResourceManager()
    {
        shutdown();
    }
    
    ShaderResourceManager::ShaderResourceManager()
    :   _device( nullptr )
    ,   _shader_resource_counter(0)
    {}

}
