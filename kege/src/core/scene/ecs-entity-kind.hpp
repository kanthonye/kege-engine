//
//  entity-kind.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/27/25.
//

#ifndef entity_kind_hpp
#define entity_kind_hpp

#include "ecs-signature.hpp"
#include "ecs-component.hpp"

#ifndef INVALID_INDEX_U32
#define INVALID_INDEX_U32 0xFFFFFFFFu
#endif

namespace kege::ecs{

    class EntityManager;

    class EntityKind : public kege::RefCounter {
    public:

        struct Edge
        {
            EntityKind* add;
            EntityKind* remove;
        };

        EntityKind* dupWith(uint32_t type, Component::Info* info)
        {
            int comp_index;
            Component::Layout layout;
            layout.attributes.resize( _layout.attributes.size() + 1 );
            layout.stride = 0;

            for (comp_index = 0; comp_index < _layout.attributes.size(); ++comp_index)
            {
                layout.attributes[ comp_index ] = _layout.attributes[ comp_index ];
                layout.attributes[ comp_index ].offset = layout.stride;
                layout.stride += layout.attributes[ comp_index ].info->size;
            }
            layout.attributes[ comp_index ].info = info;
            layout.attributes[ comp_index ].offset = layout.stride;
            layout.stride += layout.attributes[ comp_index ].info->size;

            EntityKind* archetype= new EntityKind(type, layout, 0);
            archetype->_edges[ layout.attributes[ comp_index ].info->type ].remove = this;
            return archetype;
        }

        EntityKind* dupWithout(uint32_t type, uint32_t comp_type)
        {
            int comp_index, k;

            Component::Layout layout;
            layout.attributes.resize( _layout.attributes.size() - 1 );
            layout.stride = 0;

            for (comp_index = 0, k = 0; comp_index < _layout.attributes.size(); ++comp_index)
            {
                if ( _layout.attributes[ comp_index ].info->type == comp_type) continue;
                
                layout.attributes[ k ] = _layout.attributes[ comp_index ];
                layout.attributes[ k ].offset = layout.stride;
                layout.stride += layout.attributes[ k ].info->size;
                k += 1;
            }
            EntityKind* archetype= new EntityKind(type, layout, 0);
            archetype->_edges[ comp_type ].add = this;
            return archetype;
        }

        inline const Component::Layout& getLayout()const
        {
            return _layout;
        }

        inline const Component::Attribute& getAttribute(uint32_t comp_type)const
        {
            return _layout.attributes[ _type_to_local[ comp_type ] ];
        }

        inline uint32_t getAttributeCount()const
        {
            return (uint32_t) _layout.attributes.size();
        }

        uint32_t move(EntityKind* entitykind, uint32_t old_entity)
        {
            char*src = reinterpret_cast<char*>( entitykind->get( old_entity ) );
            uint32_t new_entity = create();
            if (src)
            {
                size_t offset = 0;
                //new_index = create();
                char* dst = getPtr( new_entity );

                uint32_t min_count = (getAttributeCount() < entitykind->getAttributeCount())
                ? getAttributeCount() : entitykind->getAttributeCount();

                for (int i=0; i<min_count; ++i)
                {
                    const Component::Attribute& src_attrib = entitykind->getAttribute( _layout.attributes[i].info->type );
                    _layout.attributes[i].info->moveConstruct(dst + _layout.attributes[i].offset, src + src_attrib.offset);
                    entitykind->getAttribute( _layout.attributes[i].info->type ).info->destructor(src + src_attrib.offset);
                    offset += _layout.attributes[i].info->size;
                }
            }
            return new_entity;
        }

        inline const uint8_t& getLocalType( int comp_type )const
        {
            return _type_to_local[ comp_type ];
        }

        inline uint32_t getCompOffset( int comp_type )const
        {
            return _layout.attributes[ _type_to_local[ comp_type ] ].offset;
        }

        inline uint32_t getCompSize( int comp_type )const
        {
            return _layout.attributes[ _type_to_local[ comp_type ] ].info->size;
        }

        inline uint32_t getCompType( int comp_type )const
        {
            return _layout.attributes[ _type_to_local[ comp_type ] ].info->type;
        }

        inline const void* getComponent(uint32_t entity_index, uint32_t type)const
        {
            if (_type_to_local[ type ] == 0xFF) return nullptr;
            if (entity_index >= _element_count) return nullptr;
            return getPtr( entity_index ) + getCompOffset( type );
        }

        inline void* getComponent(uint32_t entity_index, uint32_t type)
        {
            if (_type_to_local[ type ] == 0xFF) return nullptr;
            if (entity_index >= _element_count) return nullptr;
            return getPtr( entity_index ) + getCompOffset( type );
        }

        inline const void* get(uint32_t entity_index)const
        {
            return getPtr( entity_index );
        }

        inline void* get(uint32_t entity_index)
        {
            return getPtr( entity_index );
        }

        void resize(size_t new_element_capacity)
        {
            size_t old_capacity = _element_capacity;
            size_t new_byte_size = new_element_capacity * _layout.stride;

            if( new_byte_size == 0 )
            {
                return;
            }

            kege::array<char> new_buffer(new_byte_size);
            // Move existing constructed objects [0 to _element_count)
            for (uint32_t element = 0; element < _element_count; ++element)
            {
                char* old_ptr = getPtr(element);
                char* new_ptr = new_buffer.data() + (element * _layout.stride);

                size_t offset = 0;
                for (int i=0; i<_layout.attributes.size(); ++i)
                {
                    _layout.attributes[i].info->moveConstruct(new_ptr + offset, old_ptr + offset);
                    _layout.attributes[i].info->destructor(old_ptr + offset);
                    offset += _layout.attributes[i].info->size;
                }
            }

            // Construct NEW slots [old_capacity to new_element_capacity)
            for (size_t element = old_capacity; element < new_element_capacity; ++element)
            {
                char* ptr = new_buffer.data() + (element * _layout.stride);
                size_t offset = 0;
                for (int i=0; i<_layout.attributes.size(); ++i)
                {
                    _layout.attributes[i].info->defaultConstruct(ptr + offset);
                    offset += _layout.attributes[i].info->size;
                }
            }

            _element_capacity = new_element_capacity;
            _buffer = std::move(new_buffer);
        }

        void erase(uint32_t entity_index)
        {
            if ( entity_index < _element_count )
            {
                if (_freed_head == 0 && _freed_head == _freed_tail)
                {
                    _freed.resize( (_freed_tail == 0)? _initial_size : 2 * _freed_tail );
                    _freed[ _freed_tail ] = entity_index;
                    _freed_tail += 1;
                    return;
                }
                else if (_freed_tail < _freed_head)
                {
                    _freed[ _freed_tail ] = entity_index;
                    _freed_tail += 1;

                    if (_freed_tail == _freed_head)
                    {
                        _freed_head = 0;
                        _freed_tail = static_cast<uint32_t>(_freed.size());
                        _freed.resize( 2 * _freed_tail );
                    }
                }
                else
                {
                    _freed[ _freed_tail ] = entity_index;
                    _freed_tail = (_freed_tail + 1) % _freed.size();
                }
            }
        }

        uint32_t create()
        {
            uint32_t entity_index;
            if (_freed_head != _freed_tail )
            {
                entity_index = _freed[ _freed_head ];
                _freed_head = (_freed_head + 1) % _freed.size();
            }
            else
            {
                if (_element_count >= _element_capacity)
                {
                    resize( (_element_count == 0)? 16 : 2 * _element_count );
                }
                entity_index = _element_count;
            }
            _element_count += 1;
            return entity_index;
        }

        void purge()
        {
            _type_to_local.clear();
        }

        uint32_t type()const
        {
            return _type;
        }

        const Signature& signature()const
        {
            return _signature;
        }

        void reset()
        {
            _element_count = 0;
            _freed_head = 0;
            _freed_tail = 0;
        }

        size_t getBufferSize()const
        {
            return _buffer.size();
        }

        explicit EntityKind(uint32_t type, const Component::Layout& layout, uint32_t component_count)
        :   _type(type)
        ,   _layout(layout)
        ,   _element_capacity(0)
        ,   _element_count(0)
        ,   _initial_size(16)
        ,   _freed_head(0)
        ,   _freed_tail(0)
        {
            _type_to_local.resize( Component::total() );
            memset( _type_to_local.data(), 0xFF, _type_to_local.size() );

            for (int i=0; i<layout.attributes.size(); ++i)
            {
                _signature.set( layout.attributes[i].info->type );
                _type_to_local[ layout.attributes[i].info->type ] = i;
            }
        }

        ~EntityKind()
        {
            // IMPORTANT: Call destructors for all components!
            for (uint32_t element = 0; element < _element_count; ++element)
            {
                char* ptr = getPtr(element);
                for (size_t i = 0; i < _layout.attributes.size(); ++i)
                {
                    Component::Attribute& attribute = _layout.attributes[i];
                    attribute.info->destructor(ptr + attribute.offset);
                }
            }
            _freed.clear();
            _buffer.clear();
            _type_to_local.clear();
            _layout.attributes.clear();
            _element_count = 0;
        }

    private:

        inline const char* getPtr(uint32_t entity_index)const
        {
            size_t ptr_offset = (entity_index * _layout.stride);
            if (_buffer.size() <= ptr_offset) return nullptr;
            return _buffer.data() + ptr_offset;
        }

        inline char* getPtr(uint32_t entity_index)
        {
            size_t ptr_offset = (entity_index * _layout.stride);
            if (_buffer.size() <= ptr_offset) return nullptr;
            return _buffer.data() + ptr_offset;
        }

        template<typename E>
        void increment(int& count){ count += 1; }

    private:

        Signature _signature;
        kege::array<uint8_t> _type_to_local;

        std::unordered_map<uint8_t, EntityKind::Edge> _edges;
        uint32_t _type;

        Component::Layout _layout;
        //Component::Attributes _attributes;
        //kege::array<uint32_t> _offsets;
        kege::array<uint32_t> _freed;
        kege::array<char> _buffer;
        //uint32_t _size_of_element;
        size_t _element_capacity;
        uint32_t _element_count;
        uint32_t _initial_size;
        uint32_t _freed_head;
        uint32_t _freed_tail;

        friend EntityManager;
    };
}


#endif /* entity_kind_hpp */
