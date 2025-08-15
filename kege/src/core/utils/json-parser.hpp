//
//  json-parser.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/7/24.
//

#ifndef json_parser_hpp
#define json_parser_hpp

#include <map>
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include "json-lexer.hpp"


#ifndef kege_arr_T_N_hpp
#define kege_arr_T_N_hpp
namespace kege {
    template< typename T, int N > struct arr
    {
        T data[ N ];
    };
}
#endif

namespace kege {

    class JsonValue;

    class Json
    {
    public:

        void foreach( const std::function<void(const std::string&, const Json&)>& )const;
        void foreach( const std::function<void(const Json&)>& )const;

        template< typename T, int N > arr< T,N > getArray( T(*func)(const char* s) )const
        {
            arr< T,N > a;
            for( int i = 0; i < count(); ++i )
            {
                a.data[i] = func( operator[]( i ).value() );
            }
            return a;
        };

        template< typename T > std::vector< T > getVector( T(*func)(const char* s) )const
        {
            std::vector< T > a;
            a.reserve( count() );
            for( int i = 0; i < count(); ++i )
            {
                a.push_back( func( operator[]( i ).value() ) );
            }
            return a;
        };

        const char* getString(const std::string& key, const char* default_value)const;
        double getFloat(const std::string& key, float default_value = 0.0f)const;
        bool getBool(const std::string& key, bool default_value = false)const;
        int32_t getInt(const std::string& key, int default_value = 0)const;

        const char* getString( const char* str = "" )const;
        double getDouble( double val = 0.0 )const;
        float getFloat( float val = 0.0 )const;
        bool getBool( bool val = false )const;
        int32_t getInt( int32_t val = 0 )const;

        const Json operator[]( const std::string& id )const;
        const Json operator[]( const char* id )const;
        const Json operator[]( uint32_t i )const;
        const Json operator[]( int32_t i )const;
        Json& operator=( const Json& json );
        Json& operator=( JsonValue* json );
        Json& operator=( Json&& json );

        const std::string& name(uint32_t i)const;
        
        bool insert( const std::string& id, Json json );
        bool insert( Json json );

        bool print( int spaces = 0 )const;
        bool valueIs( const char* str )const;
        const char* value()const;

        uint32_t count()const;
        operator bool()const;
        void clear();

        Json( const Json& json );
        Json( Json&& json );

        Json( JsonValue* json );
        ~Json();
        Json();

        void setref( JsonValue* json );

    protected:

        JsonValue* _json;
    };

}

namespace kege
{

    class JsonParser
    {
    private:

        static Json parseValue( std::stack< Json >& stack, kege::JsonLexer::Token& token, kege::JsonLexer& lexer );

    public:

        static Json parse( const char* buffer );
        static Json load( const char* filename );
    };

}
#endif /* json_parser_hpp */
