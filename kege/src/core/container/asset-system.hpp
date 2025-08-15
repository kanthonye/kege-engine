//
//  asset-system.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#ifndef asset_system_hpp
#define asset_system_hpp

#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace kege{

    class ResrcMngr;

    struct Resource
    {
        Resource( int32_t type, int32_t index ): type( type ), index( index ) {}
        Resource(): type( -1 ), index( -1 ) {}
    private:

        uint32_t type;
        uint32_t index;
        friend class ResrcMngr;
    };


    class ResrcMngr
    {
    public:

        virtual Resource getHandle( const std::string& name ) = 0;
        virtual void remove( const std::string& name ) = 0;
        virtual void clear() = 0;

        virtual ~ResrcMngr(){}
        static uint32_t getMaxResMgrCount(){ return _enumerator; }

    protected:

        inline uint32_t resourceType( const kege::Resource& resource )const
        {
            return resource.type;
        }
        inline uint32_t index( const kege::Resource& resource )const
        {
            return resource.type;
        }
        inline kege::Resource createResource( int32_t type, int32_t index )const
        {
            return kege::Resource{ type, index };
        }

    protected:

        static uint32_t _enumerator;
    };

}


namespace kege{

    template< typename T > class ResrcMngrT : public ResrcMngr
    {
    public:

        struct Item
        {
            std::string name;
            T resource;
        };

        typedef std::vector< T >::reverse_iterator ReverseIterator;
        typedef std::vector< T >::iterator Iterator;

        Resource add( const std::string& name, T res );
        Resource getHandle( const std::string& name );

        const T* getAsset( const std::string& name )const;
        T* getAsset( const std::string& name );

        const T& get( Resource handle )const;
        T& get( Resource handle );

        void remove( const std::string& name );
        void remove( const Resource& handle );
        void clear();

        static uint32_t type();

        ReverseIterator rbegin();
        ReverseIterator rend();

        Iterator begin();
        Iterator end();

    private:

        std::unordered_map< std::string, uint32_t > _resrc_index_map;
        std::vector< Item > _resources;
        static uint32_t _type;
    };

}


namespace kege{

    template< typename T >
    uint32_t ResrcMngrT< T >::_type = ResrcMngr::_enumerator++;


    template< typename T > Resource ResrcMngrT< T >::add( const std::string& name, T res )
    {
        auto m = _resrc_index_map.find( name );
        if ( m ==  _resrc_index_map.end() )
        {
            int32_t index = static_cast< uint32_t >( _resources.size() );
            Resource handle = createResource( _type, index );

            _resources.push_back({ name, res });
            _resrc_index_map[ name ] = index;

            return handle;
        }
        return createResource( _type, m->second );
    }

    template< typename T > Resource ResrcMngrT< T >::getHandle( const std::string& name )
    {
        auto m = _resrc_index_map.find( name );
        if ( m ==  _resrc_index_map.end() )
        {
            return {};
        }
        return createResource( _type, m->second );
    }

    template< typename T > const T* ResrcMngrT< T >::getAsset( const std::string& name )const
    {
        Resource handle = getHandle( name );
        if ( index( handle ) >= _resources.size() ) return nullptr;
        return &get( handle );
    }

    template< typename T > T* ResrcMngrT< T >::getAsset( const std::string& name )
    {
        Resource handle = getHandle( name );
        if ( index( handle ) >= _resources.size() ) return nullptr;
        return &get( handle );
    }

    template< typename T > const T& ResrcMngrT< T >::get( Resource handle )const
    {
        return _resources[ index( handle ) ].resource;
    }

    template< typename T > T& ResrcMngrT< T >::get( Resource handle )
    {
        return _resources[ index( handle ) ].resource;
    }

    template< typename T > void ResrcMngrT< T >::remove( const std::string& name )
    {
        auto m = _resrc_index_map.find( name );
        if ( m ==  _resrc_index_map.end() )
        {
            return;
        }
        _resources.erase( _resources.begin() + m->second );
        _resrc_index_map.erase( m );
    }

    template< typename T > void ResrcMngrT< T >::remove( const Resource& handle )
    {
        if ( resourceType( handle ) == _type || index( handle ) < _resources.size() ) return;

        auto m = _resrc_index_map.find( _resources[ index( handle ) ].name );
        _resources.erase( _resources.begin() + m->second );
        _resrc_index_map.erase( m );
    }

    template< typename T > void ResrcMngrT< T >::clear()
    {
        _resources.clear();
        _resrc_index_map.clear();
    }

    template< typename T >  uint32_t ResrcMngrT< T >::type()
    {
        return _type;
    }

    template< typename T > ResrcMngrT< T >::ReverseIterator ResrcMngrT< T >::rbegin()
    {
        return _resources.rbegin();
    }

    template< typename T > ResrcMngrT< T >::ReverseIterator ResrcMngrT< T >::rend()
    {
        return _resources.rend();
    }

    template< typename T > ResrcMngrT< T >::Iterator ResrcMngrT< T >::begin()
    {
        return _resources.begin();
    }

    template< typename T > ResrcMngrT< T >::Iterator ResrcMngrT< T >::end()
    {
        return _resources.end();
    }

}


namespace kege{

    class AssetSystem
    {
    public:

        template< typename T > Resource add( const std::string& name, T res )
        {
            return getManager< T >()->add( name, res );
        }

        template< typename T > T* getAsset( const std::string& name )
        {
            return getManager< T >()->getAsset( name );
        }

        template< typename T > Resource getHandle( const std::string& name )
        {
            return getManager< T >()->getHandle( name );
        }

        template< typename T > T& get( Resource handle )
        {
            return getManager< T >()->get( handle );
        }

        template< typename T > void remove( const std::string& name )
        {
            return getManager< T >()->remove( name );
        }

        template< typename T > void remove( const  Resource& handle )
        {
            return getManager< T >()->remove( handle );
        }

        template< typename T > void clear()
        {
            return getManager< T >()->clear();
        }

        template< typename T > ResrcMngrT< T >::ReverseIterator rbegin()
        {
            return getManager< T >()->rbegin();
        }

        template< typename T > ResrcMngrT< T >::ReverseIterator rend()
        {
            return getManager< T >()->rend();
        }

        template< typename T > ResrcMngrT< T >::Iterator begin()
        {
            return getManager< T >()->begin();
        }

        template< typename T > ResrcMngrT< T >::Iterator end()
        {
            return getManager< T >()->end();
        }

        template<typename T> ResrcMngrT<T>* getManager()
        {
            if ( _managers[ ResrcMngrT< T >::type() ] == nullptr )
            {
                _managers[ ResrcMngrT< T >::type() ] = new ResrcMngrT< T >;
            }
            return (ResrcMngrT<T>*)_managers[ ResrcMngrT< T >::type() ];
        }

        void shutdown();
        ~AssetSystem();
        AssetSystem();

    private:

        std::vector< ResrcMngr* > _managers;
    };

}

#endif /* asset_system_hpp */
