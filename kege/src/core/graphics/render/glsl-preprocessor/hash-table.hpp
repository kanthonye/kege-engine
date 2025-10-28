#ifndef hash_table_hpp
#define hash_table_hpp

#include <map>
#include <string>
#include <vector>

template< typename T > class HashTable
{
private:

    typedef std::pair< std::string, T > Element;
    typedef std::vector< Element > Bucket;
    typedef std::map< std::string, Bucket > Container;

public:
    
    void insert( const std::string& id, T& t )
    {
        _elements[ id ].push_back( Element( id, t ) );
    }

    T& operator []( const std::string& id )
    {
        typename Container::iterator m = _elements.find( id );
        if( m != _elements.end() )
        {
            typename Bucket::iterator i = m->second.begin();
            for(; i != m->second.end(); i++ )
            {
                if( id == i->first )
                {
                    return i->second;
                }
            }
        }
        Bucket& bucket = _elements[ id ];
        bucket.push_back( Element( id, T() ) );
        return bucket[ bucket.size() - 1 ].second;
    }

    void erase( const std::string& id )
    {
        typename Container::iterator m = _elements.find( id );
        if( m != _elements.end() )
        {
            typename Bucket::iterator i = m->second.begin();
            for(; i != m->second.end(); i++ )
            {
                if( id == i->first )
                {
                    m->second.erase( i );
                    break;
                }
            }
        }
    }

    T* find( const std::string& id )
    {
        typename Container::iterator m = _elements.find( id );
        if( m != _elements.end() )
        {
            typename Bucket::iterator i = m->second.begin();
            for(; i != m->second.end(); i++ )
            {
                if( id == i->first )
                {
                    return &i->second;
                }
            }
        }
        return nullptr;
    }

    void clear()
    {
        typename Container::iterator m = _elements.begin();
        for( ; m != _elements.end(); m++ )
        {
            m->second.clear();
        }
        _elements.clear();
    }

    bool empty()const
    {
        return _elements.empty();
    }

    void foreach( std::function< void( const T& t ) > func )const
    {
        for (typename Container::const_iterator m = _elements.begin(); m != _elements.end(); ++m)
        {
            for (typename Bucket::const_iterator i = m->second.begin(); i != m->second.end(); ++i)
            {
                func( i->second );
            }
        }
    }
    
public:
    
    class Iterator
    {
    public:

        bool hasNext()const
        {
            return  m != _elements->end();
        }

        bool operator ++( int )
        {
            if ( i == m->second.end() )
            {
                m++;
                if ( m == _elements->end() )
                {
                    return false;
                }
                i = m->second.begin();
            }
            return true;
        }

        Iterator( Container* elements, typename Container::iterator m )
        :   _elements( elements )
        {
            if( elements )
            {
                m = elements->begin();
                i = m->second.begin();
            }
        }

    private:

        Container* _elements;
        typename Container::iterator m;
        typename Bucket::iterator i;
    };

    Iterator begin()
    {
        return Iterator( &_elements, _elements.begin() );
    }

    Iterator end()
    {
        return Iterator( &_elements, _elements.end() );
    }

private:

    Container _elements;
};


#endif
