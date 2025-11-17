//
//  kege-string.cpp
//  CacheSystem
//
//  Created by Kenneth Esdaile on 3/12/23.
//
#include <ctype.h>
#include "kege-string.hpp"
namespace kege{

    std::ostream& operator <<(std::ostream& os, const kege::string& str)
    {
        return (os << (( !str.empty() ) ? str.c_str() : ""));
    }

    bool ischar(char c, const char* s)
    {
        while ( *s != c && *s != 0 ) s++;
        return *s != 0;
    }
    
    const char* strskp( const char* str, const char* skips )
    {
        while ( *str != 0 )
        {
            if ( strchr( skips, *str ) != nullptr )
            {
                str++;
                continue;
            }
            break;
        }
        return str;
    }

    const char* strcpy( char* buf, int size, const char* brks, const char* str )
    {
        uint32_t i = 0;
        while ( strchr( brks, *str ) == nullptr && i < size - 1 && *str != 0 )
        {
            buf[i++] = *str++;
        }
        buf[i] = 0;
        return str;
    }

    kege::string dtostr(double n, int space)
    {
        char str[32];
        long w = snprintf(str, 32, "%.*f", space,n);
        str[w] = 0;
        return string(str, w);
    }

    kege::string ftostr(float n, int space)
    {
        char str[32] = {0,0,0,0,0,0,0,0};
        long w = snprintf(str, 32, "%.*f", space,n);
        str[w] = 0;
        return string(str, w);
    }

    kege::string itostr(int32_t n)
    {
        char str[32] = {0,0,0,0,0,0,0,0};
        long w = snprintf(str, 32, "%i", n);
        str[w] = 0;
        return string(str, w);
    }

    kege::string itostr(int64_t n)
    {
        char str[32] = {0,0,0,0,0,0,0,0};
        long w = snprintf(str, 32, "%lld", n);
        str[w] = 0;
        return string(str, w);
    }

    string operator +(const char* a, const string& b)
    {
        int64_t a_length = strlen( a );
        int64_t length = b.length() + a_length;
        char* str = new char[ length + 1 ];
        memcpy( &str[ 0 ], a, a_length );
        memcpy( &str[ a_length ], b.c_str(), b.length() );
        str[ length ] = 0;
        return string( str, length );
    }

    string operator +(int8_t a, const string& b)
    {
        int64_t length = b._length + 1;
        char* str = new char[ length + 1 ];
        str[ 0 ] = a;
        memcpy( &str[ 1 ], b._str, b._length );
        str[ length ] = 0;
        return string( str, length );
    }

    bool operator ==(const string& a, const string& b)
    {
        if ( a.empty() && b.empty() ) return true;
        if (( a.empty() || b.empty() )) return false;
        return strcmp( a._str, b._str ) == 0;
    }

    bool operator <(const string& a, const string& b)
    {
        return StringHash()(a) < StringHash()(b);
    }

    const char& string::operator [](uint64_t i)const
    {
        return _str[ i ];
    }

    char& string::operator [](uint64_t i)
    {
        return _str[ i ];
    }

    string& string::operator +=( const string& str )
    {
        string s = string::operator +( str );

        clear();

        _length = s._length;
        _str = s._str;

        s._str = nullptr;
        return *this;
    }
    string& string::operator +=( const char* str )
    {
        string s = string::operator +( str );

        clear();

        _length = s._length;
        _str = s._str;

        s._str = nullptr;
        return *this;
    }
    string& string::operator +=( int chr )
    {
        string s = string::operator +( chr );

        clear();
        
        _length = s._length;
        _str = s._str;

        s._str = nullptr;
        return *this;
    }

    string string::operator +( const string& str )const
    {
        int64_t length = _length + str._length;
        char* s = new char[ length + 1 ];
        if( !empty() ) memcpy( &s[0], _str, _length );
        memcpy( &s[ _length ], str._str, str._length );
        s[ length ] = 0;

        return string( s, length );
    }
    string string::operator +( const char* str )const
    {
        int64_t str_length = strlen( str );
        int64_t length = _length + str_length;

        char* s = new char[ length + 1 ];
        if( !empty() ) memcpy( &s[0], _str, _length );
        memcpy( &s[ _length ], str, str_length );
        s[ length ] = 0;

        return string( s, length );
    }
    string string::operator +( int chr )const
    {
        int64_t length = _length + 1;

        char* s = new char[ length + 1 ];
        if( !empty() ) memcpy( &s[0], _str, _length );
        s[ _length ] = chr;
        s[ length ] = 0;

        return string( s, length );
    }

    string& string::operator =( const string& str )
    {
        if ( _length == str._length && str._length != 0 )
        {
            memcpy( _str, str._str, _length );
            _str[ _length ] = 0;
        }
        else if ( str._str != nullptr )
        {
            clear();
            _length = str._length;
            _str = new char[ _length + 1 ];
            memcpy( _str, str._str, _length );
            _str[ _length ] = 0;
        }
        else
        {
            clear();
        }
        return *this;
    }
    string& string::operator =( const char* str )
    {
        size_t size = strlen( str );
        if ( _length == size && size != 0 )
        {
            memcpy( _str, str, size );
            _str[ _length ] = 0;
        }
        else if ( str != nullptr )
        {
            clear();
            _length = strlen( str );
            _str = new char[ _length + 1 ];
            strncpy( _str, str, _length );
            _str[ _length ] = 0;
        }
        return *this;
    }

    const char& string::operator *()const
    {
        return *_str;
    }
    char& string::operator *()
    {
        return *_str;
    }

//    string::operator std::string()const
//    {
//        return std::string( _str );
//    }

    string::operator bool()const
    {
        return (_str == nullptr) ? false: *_str != 0;
    }
    
//    void string::copyFrom( uint64_t size, const char* str )
//    {
//        clear();
//        char* s = new char[ size + 1];
//        memcpy(s, str, size);
//        s[size] = 0;
//        //_allocated = true;
//        _length = size;
//        _str = s;
//    }


    string& string::copyFrom( uint64_t offset, uint64_t size, const char* str )
    {
        if ( empty() )
        {
            resize( offset + size );
        }
        memcpy(&_str[ offset ], str, size);
        return *this;
    }
    string& string::copyTo( uint64_t offset, uint64_t size, char* str )
    {
        if ( empty() )
        {
            resize( offset + size );
        }

        ASSERT(offset + size == _length, "string buffer overflow: offset + size exceeds buffer length!");
        memcpy(str, &_str[ offset ], size);
        return *this;
    }

    string& string::copyFrom( uint64_t offset, const string& str )
    {
        if ( empty() )
        {
            resize( offset + str._length );
        }
        memcpy(&_str[ offset ], str._str, str._length);
        return *this;
    }

    string& string::copyTo( uint64_t offset, string& str )
    {
        if ( empty() )
        {
            resize( offset + str._length );
        }

        ASSERT(offset + str._length == _length, "string buffer overflow: offset + size exceeds buffer length!");
        memcpy(str._str, &_str[ offset ], str._length);
        return *this;
    }

    bool string::match( uint64_t offset, const char* str )const
    {
        const char *s1 = str;
        const char *s2 = &_str[ offset ];
        if( *s1 != 0 && *s2 != 0 )
        {
            while ( *s1 == *s2 && *s1 != 0 && *s2 != 0 )
            {
                s1++;
                s2++;
            }
            return *s1 == 0 || *s2 == 0;
        }
        return *s1 == *s2;
    }
//    {
//        if (_str == nullptr || str == nullptr || *_str == '\0' || *str == '\0' || _length <= offset )
//            return false;
//
//        const char *r = str;
//        const char *l = &_str[ offset ];
//        while (*r == *l && *r != '\0' && *l != '\0')
//        {
//            r++;
//            l++;
//        }
//        if( *r != '\0' && *l != '\0' )
//        {
//            return true;
//        }
//        if( *r == '\0' && *l != '\0' )
//        {
//            l--;
//            return (*r == *l);
//        }
//        return false;
//    }
  
    void string::insert( size_t pos, size_t count, char ch )
    {
        if (count == 0) return;

        if (pos > _length)
        {
            pos = _length; // clamp to end
        }

        size_t new_capacity = _length + count;
        char* new_str = new char[new_capacity + 1];
        if (_str)
        {
            std::memcpy(new_str, _str, _length + 1);
            delete[] _str;
        }
        new_str[ new_capacity ] = 0;
        _length = new_capacity;
        _str = new_str;

        // Move existing tail to make room (including null terminator)
        std::memmove(_str + pos + count, _str + pos, _length - pos + 1);

        // Fill the gap with `ch`
        for (size_t i = 0; i < count; ++i)
        {
            _str[pos + i] = ch;
        }

        _length = new_capacity;
    }

    void string::insert(size_t pos, const char* str)
    {
        size_t insert_len = std::strlen(str);
        if (insert_len == 0) return;

        if (pos > _length)
        {
            pos = _length; // clamp to end
        }

        size_t new_capacity = _length + insert_len;
        char* new_data = new char[new_capacity + 1];
        if ( _str )
        {
            std::memcpy( new_data, _str, pos ); // copy old string (including null)
            std::memcpy( new_data + pos, str, insert_len );
            std::memcpy( new_data + pos + insert_len, str + pos, _length - insert_len );
            delete[] _str;
        }
        new_data[ new_capacity ] = 0;
        _str = new_data;
        _length = new_capacity;
    }

    void string::erase(size_t begin, size_t end)
    {
        if (begin >= _length) return;

        // Clamp end
        if (end > _length)
            end = _length;

        // Nothing to remove
        if (begin >= end)
            return;

        const size_t removeCount = end - begin;
        const size_t tailCount   = _length - end;

        // Shift the tail left
        if (tailCount > 0)
        {
            memmove(_str + begin, _str + end, tailCount);
        }

        _length -= removeCount;
        _str[_length] = '\0';
    }

    bool string::find( char c )const
    {
        return ( _str != nullptr )? strchr( _str, c ) != nullptr : false;
    }

    string string::parseFileName()const
    {
        char* s = &_str[ _length - 1 ];
        while( !ischar(*s, "/\\") ) s--;
        s++;
        return kege::string( s, _length - (s - _str) );
    }

    string string::parseFilePath()const
    {
        char* s = &_str[ _length - 1 ];
        while( !ischar(*s, "/\\") ) s--;
        uint64_t size = s - _str;
        s = new char[ size + 1 ];
        memcpy( s, _str, size );
        s[size] = 0;
        return kege::string( s, size );
    }

    string string::parseFileExt()const
    {
        char* s = &_str[ _length - 1 ];
        while( !ischar(*s, ".") ) s--;
        s++;
        return kege::string( s, _length - (s - _str) );
    }

    string string::parseName()const
    {
        char* s = &_str[ _length - 1 ];
        while( !ischar(*s, "./\\") ) s--;
        uint64_t period_position = s - _str;

        while( !ischar(*s, "/\\") ) s--;
        s++;

        uint64_t slash_position = s - _str;
        uint64_t size = period_position - slash_position;
        s = new char[ size + 1 ];
        memcpy( s, &_str[ slash_position ], size );
        s[ size ]  = 0;

        return kege::string( s, size );
    }
    
    string string::lowercase()const
    {
        char* s = new char[ _length + 1 ];
        char* k = _str;
        while ( *_str != 0 )
        {
            *s = tolower(*k++);
        }
        s[ _length ] = 0;
        return string( s, _length );
    }
    
    string string::uppercase()const
    {
        char* s = new char[ _length + 1 ];
        char* k = _str;
        while ( *_str != 0 )
        {
            *s = toupper(*k++);
        }
        s[ _length ] = 0;
        return string( s, _length );
    }

    int64_t string::length()const
    {
        return _length;
    }

    void string::resize( uint64_t size )
    {
        if( size == 0 ) 
        {
            clear();
        }
        else
        {
            char* s = new char[ size + 1 ];;

            if ( _str != nullptr )
            {
                uint64_t n = (_length >= size ) ? size : _length;
                memcpy(s, _str, n);
                s[ size ] = 0;
                clear();
            }
            //_allocated = true;
            _length = size;
            _str = s;
        }
    }

    const char* string::c_str()const
    {
        return _str;
    }

    char* string::str()
    {
        return _str;
    }

    double string::toFloat()const
    {
        if ( empty() ) return 0.0f;
        string n = *this;
        return atof( _str );
    }

    int64_t string::toInt64()const
    {
        if ( empty() ) return 0;
        string n = *this;
        return atoll( _str );
    }

    int32_t string::toInt32()const
    {
        if ( empty() ) return 0;
        string n = *this;
        return atoi( _str );
    }

    bool string::empty()const
    {
        return (_str == nullptr) ? true: _length == 0;
    }
    
    void string::clear()
    {
        if ( _str != nullptr )
        {
            delete [] _str;
            _str = nullptr;
            _length = 0;
        }
    }

    string::string( char* str, uint64_t length )
    :   _length( length )
    ,   _str( str )
    {}
    
    string::string( const char* str )
    :   _length( 0 )
    ,   _str( nullptr )
    {
        if ( str != nullptr )
        {
            _length = strlen( str );
            _str = new char[ _length + 1 ];
            memcpy( _str, str, _length );
            _str[ _length ] = 0;
        }
    }

    string::string( const string& str )
    :   _length( 0 )
    ,   _str( nullptr )
    {
        if ( str._str != nullptr )
        {
            _length = str._length;
            _str = new char[ _length + 1 ];
            memcpy( _str, str._str, _length );
            _str[ _length ] = 0;
        }
    }

    string::string( string&& str )
    :   _length( str._length )
    ,   _str( str._str )
    {
        str._str = nullptr;
        str._length = 0;
    }

    string::string()
    :   _length( 0 )
    ,   _str( nullptr )
    {}

    string::~string()
    {
        clear();
    }



//    std::size_t string::hash(const char* str)
//    {
////        std::size_t n = std::hash<std::string>{}( str );
////        return n;
//        if (str == NULL) return 0;
//
//        enum
//        {
//            A = 54059, /* a prime */
//            B = 76963, /* another prime */
//            C = 86969, /* yet another prime */
//            FIRSTH  =37 /* also prime */
//        };
//
//        const char * s = str;
//
//        std::size_t h = FIRSTH;
//        while (*s)
//        {
//            h = (h * A) ^ (s[0] * B);
//            s++;
//        }
//        return h;
//    };

//    std::size_t string::hash()const
//    {
//        return string::hash( _str );
//    };

}
