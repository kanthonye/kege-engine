//
//  communication.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/13/25.
//

#ifndef communication_hpp
#define communication_hpp

#include <vector>
#include <unordered_map>
#include <functional>
//#include "type-id.hpp"
#include "ref.hpp"

namespace kege{

    class Communication
    {
    public:

        template< typename MessageT, typename HandlerT > static void add( HandlerT* handler )
        {
            // typically, the handler type is derived while the message type would be explicit
            // e.g. Channel<MyEvent>::add(this);
            Channel< MessageT >::instance().add( handler ); //forward to the appropriate queue
        }

        template< typename MessageT, typename HandlerT > static void remove( HandlerT* handler )
        {
            Channel< MessageT >::instance().remove( handler );
        }

        template< typename MessageT > static void broadcast( MessageT& message )
        {
            // usually no need to be explicit, the message type can be derived at compiletime
            Channel< MessageT >::instance().broadcast( message );
        }

    private:

        template <typename MessageT> class Channel
        {
        public:
            typedef std::function< void( MessageT& ) > Handler;

            static Channel& instance()
            {
                static Channel result;
                return result;
            }

            template< typename HandlerT > void add( HandlerT* handler )
            {
                std::lock_guard<std::mutex> lock( _mutex );
                _handlers.push_back([ handler ]( MessageT msg) { ( *handler )( msg ); });
                _original_ptrs.push_back(handler);
            }

            template < typename HandlerT > void remove( HandlerT* handler )
            {
                std::lock_guard<std::mutex> lock( _mutex );

                auto it = std::find(_original_ptrs.begin(), _original_ptrs.end(), handler);
                if (it == _original_ptrs.end()) return;
                    //throw std::runtime_error("Tried to remove a handler that was not in the handler list");

                auto idx = it - _original_ptrs.begin();

                _handlers.erase(_handlers.begin() + idx);
                _original_ptrs.erase(it);
            }

            void broadcast( MessageT& msg )
            {
                std::vector<Handler> localQueue( _handlers.size() );

                {
                    std::lock_guard< std::mutex > lock( _mutex );
                    localQueue = _handlers;
                }

                for (auto& handler : localQueue)
                    handler( msg );
            }

        private:

            std::mutex _mutex;
            std::vector< Handler > _handlers;
            std::vector< void* > _original_ptrs;
        };
    };

}
#endif /* communication_hpp */
