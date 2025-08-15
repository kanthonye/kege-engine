//
//  communicator.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/1/24.
//

#ifndef communicator_hpp
#define communicator_hpp

#include <vector>
#include <unordered_map>
#include "type-id.hpp"
#include "ref.hpp"

namespace kege{

    struct Communicator : public kege::RefCounter
    {
    public:

        class Callback : public kege::RefCounter
        {
            public: virtual ~Callback(){}
        };

        template< typename P > class CallbackT : public Callback
        {
            public: virtual ~CallbackT(){}
            virtual void notify(P) = 0;
        };

        template<typename T, typename P > struct CallbacksT : public CallbackT< P >
        {
            struct Entry
            {
                void(T::*function)(P);
                T* object;
            };

            void notify(P p)override
            {
                for (Entry& entry : callbacks)
                {
                    (entry.object->*entry.function)( p );
                }
            }

            void add(T* object, void(T::*function)(P))
            {
                callbacks.push_back({ function, object });
            }

            void erase(T* ptr)
            {
                typename std::vector< Entry >::iterator i;
                for (i = callbacks.begin(); i != callbacks.end(); i++)
                {
                    if( i->object == ptr)
                    {
                        callbacks.erase( i );
                        break;
                    }
                }
            }

            std::vector< Entry > callbacks;
        };

    public:

        template< typename T, typename P > void subcribe(T* object, void(T::*function)(P))
        {
            kege::Ref< Callback >& callbacks = _callbacks[ getTypeId< P >() ];
            if( !callbacks )
            {
                callbacks = new CallbacksT< T,P >;
            }
            ((CallbacksT< T,P >*) callbacks.ref())->add( object, function );
        }

        template< typename T, typename P > void unsubcribe(T* object)
        {
            auto m = _callbacks.find( getTypeId< P >() );
            if( m != _callbacks.end() )
            {
                ((CallbacksT< T,P >*) m->second.ref())->erase( object );
            }
        }

        template< typename P > void notify( P p)
        {
            auto m = _callbacks.find( getTypeId< P >() );
            if( m != _callbacks.end() )
            {
                ((CallbackT< P >*) m->second.ref())->notify( p );
            }
        }

        template< typename P > uint32_t getTypeId()
        {
            return kege::TypeId< Communicator, P >::get();
        }

        void clear()
        {
            _callbacks.clear();
        }
        
    private:

        std::unordered_map< uint32_t, kege::Ref< Callback > > _callbacks;
    };



}
#endif /* communicator_hpp */
