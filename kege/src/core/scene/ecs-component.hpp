//
//  component.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/25/25.
//

#ifndef component_hpp
#define component_hpp

#include <stdlib.h>
#include <functional>
#include <memory>
#include <typeindex>
#include <cstring>
#include "../utils/array.hpp"

namespace kege::ecs{

    using ComponentType = uint16_t;
    struct Component
    {
        template<typename T>
        static void increment(int* count){ *count += 1; }

        template<typename... T>
        static int count()
        {
            int value = 0;
            ((increment<T>( &value )),...);
            return value;
        }

        struct Info
        {
            // Function pointers for lifetime management
            std::function<void(void*, const void*)> copyConstruct;
            std::function<void(void*, void*)> moveConstruct;
            std::function<void(void*)> defaultConstruct;
            std::function<void(void*)> destructor;

            uint32_t size;
            uint32_t type;

        protected:

            // Use std::atomic for thread safety
            static std::atomic<uint32_t> type_count;
            friend Component;
        };

        template<typename T> struct InfoT : public Info
        {
            static uint32_t _type;
            InfoT(int* attrib_offset = nullptr)
            {
                type = _type;
                size = sizeof(T);

                // Default constructor - always safe
                defaultConstruct = [](void* ptr) {
                    new (ptr) T();  // placement new
                    printf( "Construct attribute at %p\n", ptr);
                };

                // Only set non-trivial operations if type is non-trivial
                if constexpr (!std::is_trivially_destructible_v<T>)
                {
                    destructor = [](void* ptr)
                    {
                        static_cast<T*>(ptr)->~T();
                    };
                }
                else
                {
                    // Trivial types: no need for custom destruct/copy/move
                    destructor = [](void*){};
                }

                // Only set non-trivial operations if type is non-trivial
                if constexpr (!std::is_trivially_copyable_v<T> || !std::is_trivially_default_constructible_v<T>)
                {
                    copyConstruct = [](void* dst, const void* src) {
                        new (dst) T(*static_cast<const T*>(src));
                    };

                    moveConstruct = [](void* dst, void* src) {
                        new (dst) T(std::move(*static_cast<T*>(src)));
                    };
                }
                else
                {
                    // Trivial types: no need for custom copy/move
                    copyConstruct = nullptr;
                    moveConstruct = nullptr;
                    // Use memcpy instead in your entity copy logic!
                }
            }
        };

        template<typename T> static Info* getInfo()
        {
            static InfoT<T> info;
            return &info;
        }

        struct Attribute
        {
            Info* info;
            uint32_t offset;
        };

        struct Layout
        {
            kege::array< Component::Attribute > attributes;
            uint32_t stride;
        };

        template<typename... T>
        static const Component::Layout& getLayout()
        {
            static Component::Layout layout;
            if( layout.attributes.empty() )
            {
                int i = 0;
                layout.stride = 0;
                layout.attributes.resize( count<T...>() );
                ((layout.attributes[i++].info = getInfo<T>()),...);

                for(int i=0; i<layout.attributes.size(); ++i)
                {
                    layout.attributes[i].offset = layout.stride;
                    layout.stride += layout.attributes[i].info->size;
                }
            }
            return layout;
        }

        template<typename T> static uint32_t type()
        {
            return InfoT<T>::_type;
        }

        static uint32_t total()
        {
            return Component::Info::type_count;
        }
    };

    template<typename T> uint32_t Component::InfoT<T>::_type = Component::Info::type_count++;
}
#endif /* component_hpp */
