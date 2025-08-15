//
//  shader-resource-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "../../graphics/core/graphics.hpp"
#include "../../graphics/core/shader-resource-manager.hpp"

namespace kege{

    ShaderResrc ShaderResourceManager::allocate( const ShaderResrcAllocInfo& info )
    {
        uint32_t index = generate();
        splice( &_freed_head, &_freed_tail, index, &_alloc_head, &_alloc_tail );

        _shader_resources[ index ].shader_resource_sets.resize( info.quantity );
        for ( int i = 0; i < info.quantity; ++i )
        {
            ShaderResrcSetBindings& binding = _shader_resources[ index ];

            binding.shader_resource_sets[i].set = _graphics->allocateDescriptorSet( info.bindings );
            if ( !binding.shader_resource_sets[i].set )
            {
                return {};
            }

            //binding.shader_resource_sets[i].bindings.resize(<#size_type sz#>) = info.bindings[i].descriptor_type;
            binding.shader_resource_sets[i].bindings.resize( info.bindings.size() );
            for ( int k = 0; k < info.bindings.size(); ++k )
            {
                binding.shader_resource_sets[i].bindings[k] = {};
                binding.shader_resource_sets[i].bindings[k].binding = info.bindings[i].binding;
                binding.shader_resource_sets[i].bindings[k].descriptor_type = info.bindings[i].descriptor_type;
            }
        }
        return ShaderResrc( this, index );
    }

    void ShaderResourceManager::splice( int32_t* lhead, int32_t* ltail, int32_t index, int32_t* head, int32_t* tail )
    {
        if ( index == *lhead )
        {
            *lhead = _shader_resources[ *lhead ].next;
            if ( *lhead < 0 )
            {
                *ltail = -1;
            }
        }
        else if ( index == _alloc_tail )
        {
            *ltail = _shader_resources[ *ltail ].prev;
            if ( *ltail < 0 )
            {
                *lhead = -1;
            }
        }
        else
        {
            uint32_t prev = _shader_resources[ index ].prev;
            uint32_t next = _shader_resources[ index ].next;
            _shader_resources[ prev ].next = next;
            _shader_resources[ next ].prev = prev;
        }

        if ( *head < 0 )
        {
            *tail = *head = index;
        }
        else
        {
            _shader_resources[ *tail ].next = index;
            _shader_resources[ index ].prev = *tail;
            *tail = index;
        }
    }
    
    bool ShaderResourceManager::retain( const std::string& name, ShaderResrc& descriptor )
    {
        auto i = _name_index_map.find( name );
        if ( i != _name_index_map.end() )
        {
            KEGE_LOG_ERROR << "invalid operation -> redeclearation of resource -> " << name <<Log::nl;
            return false;
        }
        _name_index_map[ name ] = descriptor._index;
        splice( &_alloc_head, &_alloc_tail, descriptor._index, &_retained_head, &_retained_tail );
        return true;
    }

    bool ShaderResourceManager::undeclear( const std::string& name )
    {
        auto i = _name_index_map.find( name );
        if ( i == _name_index_map.end() )
        {
            return false;
        }

        splice( &_retained_head, &_retained_tail, i->second, &_freed_head, &_freed_tail );
        _name_index_map.erase( i );
        return true;
    }

    bool ShaderResourceManager::update( int frame_index, ShaderResrcSet* resource, const std::vector< kege::ShaderResrcUpdateInfo >& update_infos )
    {
        for ( const ShaderResrcUpdateInfo& info : update_infos )
        {
            resource->bindings[ info.index ] = info.infos;
        }

        std::vector< kege::WriteDescriptorSet > writes;
        for ( const ShaderResrcUpdateInfo& info : update_infos )
        {
            kege::ShaderResrcBinding& bindings = resource->bindings[ info.index ];

            kege::WriteDescriptorSet write = {};
            write.descriptor_type = resource->bindings[ info.index ].descriptor_type;
            write.binding = bindings.binding;
            write.array_element = 0;
            write.set = resource->set;

            switch ( write.descriptor_type )
            {
                case kege::DescriptorType::UniformBuffer:
                case kege::DescriptorType::StorageBuffer:
                case kege::DescriptorType::StorageBufferDynamic:
                case kege::DescriptorType::UniformBufferDynamic:
                {
                    if ( bindings.type != ShaderResrcBinding::BUFFER )
                    {
                        KEGE_LOG_ERROR
                        << "Invalid Operation attempting to map none buffer resource to a buffer DescriptorType"
                        << " in ShaderResourceManager::update" <<Log::nl;
                        return false;
                    }
                    write.buffer_info = bindings.buffer_info;
                    break;
                }

                case kege::DescriptorType::Sampler:
                case kege::DescriptorType::SampledImage:
                case kege::DescriptorType::CombinedImageSampler:
                {
                    if ( bindings.type != ShaderResrcBinding::IMAGE )
                    {
                        KEGE_LOG_ERROR
                        << "Invalid Operation attempting to map none image resource to a image DescriptorType"
                        << " in ShaderResourceManager::update" <<Log::nl;
                        return false;
                    }
                    write.image_info = bindings.image_info;
                    break;
                }

                case kege::DescriptorType::StorageTexelBuffer:
                case kege::DescriptorType::UniformTexelBuffer:
                {
                    if ( bindings.type != ShaderResrcBinding::TEXEL_BUFFER )
                    {
                        KEGE_LOG_ERROR
                        << "Invalid Operation attempting to map none buffer resource to a buffer DescriptorType"
                        << " in ShaderResourceManager::update" <<Log::nl;
                        return false;
                    }
                    write.texel_buffer_info = bindings.texel_buffer_info;
                    break;
                }

                default:
                {
                    KEGE_LOG_ERROR << "Invalid ShaderResrcUpdateInfo DescriptorType with image object" <<Log::nl;
                    return false;
                }
            }
            writes.push_back( write );
        }
        return _graphics->updateDescriptorSets( writes );
    }

    bool ShaderResourceManager::update( ShaderResrc& resource, const std::vector< ShaderResrcUpdateInfos >& infos )
    {
        for (int frame_index = 0; frame_index < _shader_resources[ resource._index ].shader_resource_sets.size(); ++frame_index )
        {
            if ( !update( frame_index, &_shader_resources[ resource._index ].shader_resource_sets[ frame_index ], infos[ frame_index ] ) )
            {
                KEGE_LOG_ERROR << "ShaderResrc update attempt failed in ShaderResourceManager::update" <<Log::nl;
                return false;
            }
        }
        return true;
    }

    ShaderResrc ShaderResourceManager::get( const std::string& name )
    {
        auto itr = _name_index_map.find( name );
        return ( itr != _name_index_map.end() )
        ? ShaderResrc( this, itr->second )
        : ShaderResrc();
    }

    void ShaderResourceManager::reset()
    {
        if ( _freed_head < 0 )
        {
            _freed_head = _alloc_head;
            _freed_tail = _alloc_tail;
        }
        else
        {
            _shader_resources[ _freed_tail ].next = _alloc_head;
            _shader_resources[ _alloc_head ].prev = _freed_tail;
            _freed_tail = _alloc_head;
        }
        _alloc_head = -1;
        _alloc_tail = -1;
    }

    uint32_t ShaderResourceManager::generate()
    {
        uint32_t index;
        if ( 0 <= _freed_head )
        {
            index = _freed_head;
            _freed_head = _shader_resources[ _freed_head ].next;
            if ( _freed_head < 0 )
            {
                _freed_tail = -1;
            }
            return index;
        }

        index = static_cast< uint32_t >( _shader_resources.size() );
        _shader_resources.push_back({});
        //_shader_resources[ index ].shader_resource_sets = resources;
        //_shader_resources[ index ].shader_resource_sets = set;
        _shader_resources[ index ].next = -1;
        _shader_resources[ index ].prev = -1;

        if ( _freed_head < 0 )
        {
            _freed_tail = _freed_head = index;
        }
        else
        {
            _shader_resources[ _freed_tail ].next = index;
            _shader_resources[ index ].prev = _freed_tail;
            _freed_tail = index;
        }
        return index;
    }

    void ShaderResourceManager::initalize( kege::Graphics* graphics )
    {
        _graphics = graphics;
        _retained_head = -1;
        _retained_tail = -1;
        _alloc_head = -1;
        _alloc_tail = -1;
        _freed_head = -1;
        _freed_tail = -1;
    }

    void ShaderResourceManager::shutdown()
    {
        if ( _graphics )
        {
            for ( ShaderResrcSetBindings& bindings : _shader_resources )
            {
                for (int i = 0; i < bindings.shader_resource_sets.size(); ++i )
                {
                    _graphics->freeDescriptorSet( bindings.shader_resource_sets[i].set );
                }
            }
            _shader_resources.clear();
            _name_index_map.clear();
            _graphics = nullptr;
            _retained_head = -1;
            _retained_tail = -1;
            _alloc_head = -1;
            _alloc_tail = -1;
            _freed_head = -1;
            _freed_tail = -1;
        }
    }

    ShaderResourceManager::~ShaderResourceManager()
    {
        shutdown();
    }
    
    ShaderResourceManager::ShaderResourceManager()
    :   _graphics( nullptr )
    ,   _retained_head( -1 )
    ,   _retained_tail( -1 )
    ,   _alloc_head( -1 )
    ,   _alloc_tail( -1 )
    ,   _freed_head( -1 )
    ,   _freed_tail( -1 )
    {}

}


namespace kege{



    const ShaderResrcSet* ShaderResrc::operator ->()const
    {
        int index = _mngr->_graphics->getCurrFrameIndex() % _mngr->_shader_resources[ _index ].shader_resource_sets.size();
        return &_mngr->_shader_resources[ _index ].shader_resource_sets[ index ];
    }

    ShaderResrc::operator bool()const
    {
        return _mngr != nullptr;
    }

    ShaderResrc::ShaderResrc( ShaderResourceManager* mngr, uint32_t index )
    :   _mngr( mngr )
    ,   _index( index )
    {}

    ShaderResrc::ShaderResrc()
    :   _mngr( nullptr )
    ,   _index( 0 )
    {}

}
