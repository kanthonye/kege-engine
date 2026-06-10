//
//  render-graph-resource-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/10/26.
//

#ifndef render_graph_resource_manager_hpp
#define render_graph_resource_manager_hpp

#include "graph/render-graph-common.hpp"

namespace kege{

    class RenderGraphResourceManager
    {
    private:

        template<typename Desc, typename Resource> struct Element
        {
            Desc desc;
            Resource data;

            /** @brief Handle to the resource in the render graph */
            RgHandle handle = {};

            mutable uint32_t frame_id = 0;
            int64_t prev = -1;
            int64_t next = -1;
            bool imported;
            bool created;
        };

        template<typename Desc, typename Resource> struct Manager
        {
            using ResourceEntry = Element< Desc, Resource >;
            using ResourceEntryTable = std::unordered_map< std::string, RgHandle >;

            typedef void (*DestroyFunct)( kege::GraphicsDevice* device, const Desc& desc, Resource& resrc );
            typedef void (*CreateFunct)( kege::GraphicsDevice* device, const Desc& desc, Resource& resrc );

            Resource* getOrCreate( const std::string& name )
            {
                auto i = _string_to_index_map.find( name );
                if( i == _string_to_index_map.end() )
                    return nullptr;
                return getOrCreate( i->second );
            }

            Resource* getOrCreate( const RgHandle& handle )
            {
                if( _resources.size() <= handle.id )
                    return nullptr;

                ResourceEntry& entry = _resources[ handle.id ];
                if( !entry.created )
                {
                    if( _createResource != nullptr)
                    {
                        _createResource( _device, entry.desc, entry.data );
                        entry.created = true;
                        entry.frame_id = _frame_index;
                    }
                }
                return &entry.data;
            }

            RgHandle importResource( const Desc& desc, const Resource& resource )
            {
                RgHandle handle = defnDesc( desc );
                ResourceEntry* entry = &_resources[ handle.id ];
                entry->resource = resource;
                entry->imported = true;
                entry->created = true;
                return handle;
            }

            RgHandle defnDesc( const Desc& desc )
            {
                ResourceEntry* entry = 0;

                if(_head >= 0)
                {
                    entry = &_resources[ _head ];
                    _head = entry->next;
                    if(_head < 0) _tail = -1;
                }
                else
                {
                    RgHandle handle;
                    handle.id = _resources.size();
                    _resources.emplace_back();

                    entry = &_resources[ handle.id ];
                    entry->handle = handle;
                }

                entry->prev = -1;
                entry->next = -1;
                entry->desc = desc;
                entry->created = false;
                entry->imported = false;
                entry->frame_id = _frame_index;
                return entry->handle;
            }

            RgHandle getRgHandle( const std::string& name ) const
            {
                auto i = _string_to_index_map.find( name );
                if( i == _string_to_index_map.end() )
                    return {};
                return _resources[ i->second.id ].handle;
            }

            const Resource* getResource( const std::string& name ) const
            {
                auto i = _string_to_index_map.find( name );
                if( i == _string_to_index_map.end() )
                    return nullptr;
                return getResource( i->second );
            }

            const Resource* getResource( const RgHandle& handle ) const
            {
                const ResourceEntry& entry = _resources[ handle.id ];
                entry.frame_id = _frame_index;
                return &entry.data;
            }

            const Desc* getDesc( const std::string& name ) const
            {
                auto i = _string_to_index_map.find( name );
                if( i == _string_to_index_map.end() )
                    return nullptr;
                return &_resources[ i->second.id ].desc;
            }

            const Desc* getDesc( const RgHandle& handle ) const
            {
                return &_resources[ handle.id ].desc;
            }

            void remove( const std::string& name )
            {
                auto i = _string_to_index_map.find( name );
                if( i == _string_to_index_map.end() )
                    return;
                destroy( i->second.id );
            }

            void remove( const RgHandle& handle )
            {
                destroy( handle.id );
            }

            void destroy( uint32_t i )
            {
                if(_resources.size() <= i ) return;
                if(_resources[i].prev < 0 && 0 > _resources[i].next)
                {
                    if(_head < 0)
                    {
                        _tail = _head = _resources[i].handle.id;
                    }
                    else
                    {
                        _resources[ _resources[i].handle.id ].prev = _tail;
                        _resources[ _tail ].next = _resources[i].handle.id;
                        _tail = _resources[i].handle.id;
                    }
                }
                if( _destroyResource != nullptr && _resources[i].created)
                {
                    _destroyResource( _device, _resources[i].desc, _resources[i].data );
                    _resources[i].created = false;
                }
            }

            Element< Desc, std::vector< Resource > >& at(uint32_t i)
            {
                return _resources[i];
            }

            size_t count()const
            {
                return _resources.size();
            }

            void beginFrame( uint32_t frame_index )
            {
                _frame_index = frame_index;
            }

            void endFrame()
            {
                std::vector< ResourceEntryTable::iterator > idle_resources;
                for(ResourceEntryTable::iterator i = _string_to_index_map.begin(); i != _string_to_index_map.end(); i++)
                {
                    ResourceEntry& entry = _resources[ i->second ];

                    bool state = ( _frame_index < entry.frame_id )
                    ? (( _frame_index - entry.frame_id >= 0 ) ? true : false)
                    : _frame_index - entry.frame_id > 60;

                    if( state )
                    {
                        idle_resources.push_back( i );
                    }
                }
                for(auto& map_itr : idle_resources)
                {
                    _resources.erase( _resources.begin() + map_itr->second.id );
                    _string_to_index_map.erase( map_itr );
                }
            }

            void initialize( kege::GraphicsDevice* device, CreateFunct cfn, DestroyFunct dfn )
            {
                _device = device;
                _destroyResource = dfn;
                _createResource = cfn;
            }

            void purge()
            {
                _resources.clear();
                _string_to_index_map.clear();
            }

            Manager()
            :   _device( nullptr )
            ,   _destroyResource( nullptr )
            ,   _createResource( nullptr )
            ,   _frame_index( 0 )
            {}

            std::unordered_map< std::string, RgHandle > _string_to_index_map;
            std::vector< ResourceEntry > _resources;

            kege::GraphicsDevice* _device;

            DestroyFunct _destroyResource;
            CreateFunct _createResource;

            uint32_t _frame_index;
            int64_t _head = -1;
            int64_t _tail = -1;
        };

    public:

        using BufferManager = Manager< kege::RgBufferDesc, std::vector< kege::BufferBindInfo > >;
        using ImageManager = Manager< kege::RgImageDesc, std::vector< kege::ImageBindInfo > >;

        const kege::Binding getBinding( const RgHandle& handle )const
        {
            return  {};
        }

        BufferManager* getBufferManager()
        {
            return &_buffers;
        }

        ImageManager* getImageManager()
        {
            return &_images;
        }

        void beginFrame()
        {
            _frame_index = (_frame_index >= 60) ? _frame_index = 0 : _frame_index + 1;
            _images.beginFrame( _frame_index );
            _buffers.beginFrame( _frame_index );
        }

        void endFrame()
        {
            _buffers.endFrame();
            _images.endFrame();
        }

        void initialize( kege::GraphicsDevice* device )
        {
            _buffers.initialize( device, createBuffer, destroyBuffer );
            _images.initialize( device, createImage, destroyImage );
        }

        void shutdown()
        {
            _buffers.purge();
            _images.purge();
        }

        RenderGraphResourceManager()
        {}

    private:

        static void destroyBuffer
        (
            kege::GraphicsDevice* device,
            const kege::RgBufferDesc& desc,
            std::vector< kege::BufferBindInfo >& resrc
        )
        {
            for(int i=0; i<desc.frames_in_flight; ++i)
            {
                resrc[i].buffer.clear();
            }
        }

        static void createBuffer
        (
            kege::GraphicsDevice* device,
            const kege::RgBufferDesc& desc,
            std::vector< kege::BufferBindInfo >& resrc
        )
        {
            for(int i=0; i<desc.frames_in_flight; ++i)
            {
                resrc[i].buffer = device->createBuffer
                ({
                    .size = desc.size,
                    .data = nullptr,
                    .usage = desc.usage,
                    .memory_usage = desc.memory_usage,
                    .name = desc.name.c_str(),
                });
            }
        }

        static void destroyImage
        (
            kege::GraphicsDevice* device,
            const kege::RgImageDesc& desc,
            std::vector< kege::ImageBindInfo >& resrc
        )
        {
            for(int i=0; i<desc.frames_in_flight; ++i)
            {
                resrc[i].image.clear();
                resrc[i].sampler.clear();
            }
        }

        static void createImage
        (
            kege::GraphicsDevice* device,
            const kege::RgImageDesc& desc,
            std::vector< kege::ImageBindInfo >& resrc
        )
        {
            for(int i=0; i<desc.frames_in_flight; ++i)
            {
                resrc[i].image = device->createImage
                ({
                    .type = desc.type,
                    .format = desc.format,
                    .extent = {desc.width, desc.height, desc.depth},
                    .mip_levels = 1,
                    .array_layers = desc.array_layers,
                    .usage = desc.usages,
                    .memory_usage = MemoryUsage::GpuOnly,
                    .initial_layout = desc.layout,
                    .name = desc.name,
                });
                resrc[i].layout = kege::ImageLayout::ShaderRead;
            }
        }

    private:

        BufferManager _buffers;
        ImageManager _images;
        uint32_t _frame_index;
    };
}
#endif /* render_graph_resource_manager_hpp */
