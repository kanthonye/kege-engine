//
//  json-parser.cpp
//  kege
//
//  Created by Kenneth Esdaile on 11/7/24.
//

#include "json-parser.hpp"

namespace kege
{

    class JsonValue
    {
    public:

        virtual void foreach( const std::function<void(const std::string&, const Json&)>& )const;
        virtual void foreach( const std::function<void(const Json&)>& )const;

        virtual const Json operator[]( const std::string& )const;
        virtual const Json operator[]( uint32_t )const;

        virtual bool insert( const std::string& id, Json json );
        virtual bool insert( Json json );

        virtual const std::string& name(uint32_t i)const;

        virtual bool print( int spaces )const;
        virtual const std::string& value()const;
        virtual uint32_t count()const;

        ~JsonValue();
        JsonValue();

    protected:

        static std::string _nullstr;
        int _refcount;
        friend Json;
    };

    struct JsonString : public JsonValue
    {
        JsonString( const std::string& s );
        const std::string& value()const;
        bool print( int spaces )const;
        std::string str;
    };

    class JsonArray : public JsonValue
    {
    public:

        virtual void foreach( const std::function<void(const std::string&, const Json&)>& )const;
        virtual void foreach( const std::function<void(const Json&)>& )const;

        virtual bool insert( const std::string& id, Json json );
        virtual bool insert( Json json );

        virtual const Json operator[]( uint32_t i )const;

        virtual bool print( int spaces )const;
        virtual const std::string& value()const;
        uint32_t count()const;

        ~JsonArray();
        JsonArray();

    public:

        std::vector< Json > params;
    };

    class JsonObject : public JsonValue
    {
    public:

        typedef std::pair< std::string, Json > Param;
        typedef std::vector< Param > Params;
        typedef std::vector< int > Indices;
        typedef std::map< std::string, Indices > ParamMap;

        virtual void foreach( const std::function<void(const std::string&, const Json&)>& )const;
        virtual void foreach( const std::function<void(const Json&)>& )const;

        virtual const Json operator[]( const std::string& id )const;
        virtual const Json operator[]( uint32_t i )const;

        virtual bool insert( const std::string& id, Json json );

        const std::string& name( uint32_t i )const;
        
        virtual bool print( int spaces )const;
        uint32_t count()const;

        ~JsonObject();
        JsonObject();

    public:

        ParamMap param_map;
        Params params;
    };

}


namespace kege
{

    std::string JsonValue::_nullstr;

    const char* Json::getString( const std::string& key, const char* default_value )const
    {
        return ( _json == nullptr ) ? default_value : operator[]( key ).value();
    }

    int32_t Json::getInt( const std::string& key, int default_value )const
    {
        const char* v = operator[]( key ).value();
        return ( _json == nullptr ) ? default_value : atoi( v );
    }

    double Json::getFloat( const std::string& key, float default_value )const
    {
        const char* v = operator[]( key ).value();
        return ( v == nullptr ) ? default_value : atof( v );
    }

    bool Json::getBool( const std::string& key, bool default_value )const
    {
        const char* v = operator[]( key ).value();
        return ( v == nullptr ) ? default_value : strcmp( "true", v ) == 0 || strcmp( "1", v ) == 0;
    }

    const char* Json::getString( const char* str )const
    {
        return ( _json == nullptr ) ? "str" : _json->value().data();
    }

    int32_t Json::getInt( int32_t val )const
    {
        return ( _json == nullptr ) ? val : atoi( _json->value().data() );
    }

    double Json::getDouble( double val )const
    {
        return ( _json == nullptr ) ? val : atof( _json->value().data() );
    }

    float Json::getFloat( float val )const
    {
        return ( _json == nullptr ) ? val : atof( _json->value().data() );
    }

    bool Json::getBool( bool val )const
    {
        return ( _json == nullptr ) ? val : _json->value() == "true" || _json->value() == "1";
    }

    void Json::foreach( const std::function<void(const std::string&, const Json&)>& func )const
    {
        if ( _json == nullptr ) return;
        return _json->foreach( func );
    }

    void Json::foreach( const std::function<void(const Json&)>& func )const
    {
        if ( _json == nullptr ) return;
        return _json->foreach( func );
    }

    const Json Json::operator[]( const std::string& id )const
    {
        if ( _json == nullptr ) return *this;
        return _json->operator[]( id );
    }

    const Json Json::operator[]( const char* id )const
    {
        if ( _json == nullptr ) return *this;
        return _json->operator[]( id );
    }

    const Json Json::operator[]( uint32_t i )const
    {
        if ( _json == nullptr ) return *this;
        return _json->operator[]( i );
    }

    const Json Json::operator[]( int32_t i )const
    {
        if ( _json == nullptr ) return *this;
        return _json->operator[]( i );
    }

    Json& Json::operator=( const Json& json )
    {
        setref( json._json );
        return *this;
    }

    Json& Json::operator=( JsonValue* json )
    {
        setref( json );
        return *this;
    }

    Json& Json::operator=( Json&& json )
    {
        _json = json._json;
        json._json = nullptr;
        return *this;
    }

    const std::string& Json::name(uint32_t i)const
    {
        return _json->name( i );
    }

    bool Json::insert( const std::string& id, Json json )
    {
        return _json->insert( id, json );
    }

    bool Json::insert( Json json )
    {
        return _json->insert( json );
    }

    bool Json::print( int spaces )const
    {
        if ( _json == nullptr ) return false;
        return _json->print( spaces );
    }

    bool Json::valueIs( const char* str )const
    {
        if ( _json == nullptr ) return false;
        return strcmp( _json->value().data(), str ) == 0;
    }

    const char* Json::value()const
    {
        if ( _json == nullptr ) return "";
        return _json->value().data();
    }

    uint32_t Json::count()const
    {
        if ( _json == nullptr ) return 0;
        return _json->count();
    }
    
    Json::operator bool()const
    {
        return _json != nullptr;
    }

    void Json::clear()
    {
        if ( _json )
        {
            _json->_refcount--;
            if ( _json->_refcount <= 0 )
            {
                delete _json;
            }
            _json = nullptr;
        }
    }

    void Json::setref( JsonValue* json )
    {
        clear();
        _json = json;
        if ( _json )
        {
            _json->_refcount++;
        }
    }

    Json::Json( const Json& other )
    :   _json( nullptr )
    {
        setref( other._json );
    }
    Json::Json( Json&& other )
    :   _json( nullptr )
    {
        operator=( other );
    }

    Json::Json( JsonValue* json )
    :   _json( nullptr )
    {
        setref( json );
    }

    Json::~Json()
    {
        clear();
    }
    Json::Json()
    :   _json( nullptr )
    {}






    void JsonValue::foreach( const std::function<void(const std::string&, const Json&)>& )const
    {
    }
    void JsonValue::foreach( const std::function<void(const Json&)>& )const
    {
    }
    const Json JsonValue::operator[]( const std::string& )const
    {
        return {};
    }
    const Json JsonValue::operator[]( uint32_t )const
    {
        return {};
    }
    const std::string& JsonValue::name(uint32_t i)const
    {
        return _nullstr;
    }
    bool JsonValue::insert( const std::string& id, Json json )
    {
        return false;
    }
    bool JsonValue::insert( Json json )
    {
        return false;
    }
    bool JsonValue::print( int spaces )const
    {
        return false;
    }
    const std::string& JsonValue::value()const
    {
        return _nullstr;
    }
    uint32_t JsonValue::count()const
    {
        return 0;
    }
    JsonValue::~JsonValue()
    {}
    JsonValue::JsonValue()
    :   _refcount( 0 )
    {}



    JsonString::JsonString( const std::string& s )
    :   str( s )
    {}
    const std::string& JsonString::value()const
    {
        return str;
    }
    bool JsonString::print( int spaces )const
    {
        std::cout << str;
        return true;
    }


    void JsonArray::foreach( const std::function<void(const std::string&, const Json&)>& func )const
    {
        for (std::vector< Json >::const_iterator i = params.begin(); i != params.end(); ++i)
        {
            func( "", *i );
        }
    }
    void JsonArray::foreach( const std::function<void(const Json&)>& func )const
    {
        for (std::vector< Json >::const_iterator i = params.begin(); i != params.end(); ++i)
        {
            func( *i );
        }
    }

    const Json JsonArray::operator[]( uint32_t i )const
    {
        if( i >= params.size() ) return {};
        return params[ i ];
    }
    bool JsonArray::insert( const std::string& id, Json json )
    {
        return insert( json );
    }
    bool JsonArray::insert( Json json )
    {
        params.push_back( json );
        return true;
    }
    bool JsonArray::print( int spaces )const
    {
        std::cout <<"[";
        if ( !params.empty() )
        {
            params[0].print( spaces + 1 );
            for (int i = 1; i != params.size(); ++i)
            {
                std::cout <<", ";
                params[i].print( spaces + 1 );
            }
        }
        std::cout <<"]";
        return true;
    }
    const std::string& JsonArray::value()const
    {
        return _nullstr;
    }
    uint32_t JsonArray::count()const
    {
        return (uint32_t) params.size();
    }
    JsonArray::~JsonArray()
    {
        params.clear();
    }
    JsonArray::JsonArray()
    {}



    void JsonObject::foreach( const std::function<void(const std::string&, const Json&)>& func )const
    {
        for (Params::const_iterator i = params.begin(); i != params.end(); ++i)
        {
            func( i->first, i->second );
        }
    }
    void JsonObject::foreach( const std::function<void(const Json&)>& func )const
    {
        for (Params::const_iterator i = params.begin(); i != params.end(); ++i)
        {
            func( i->second );
        }
    }

    const Json JsonObject::operator[]( const std::string& id )const
    {
        ParamMap::const_iterator m = param_map.find( id );
        if ( m != param_map.end() )
        {
            for (int i = 0; i != m->second.size(); ++i)
            {
                if ( params[ m->second[ i ] ].first == id )
                {
                    return params[ m->second[ i ] ].second;
                }
            }
        }
        return {};
    }
    const std::string& JsonObject::name( uint32_t i )const
    {
        return params[ i ].first;
    }
    const Json JsonObject::operator[]( uint32_t i )const
    {
        return params[ i ].second;
    }
    bool JsonObject::insert( const std::string& id, Json json )
    {
        ParamMap::iterator m = param_map.find( id );
        if ( m == param_map.end() )
        {
            Indices& list = param_map.emplace( id, std::vector< int >() ).first->second;
            list.push_back( uint32_t( params.size() ) );
        }
        else
        {
            m->second.push_back( uint32_t( params.size() ) );
        }
        params.push_back({ id, json });
        return true;
    }
    bool JsonObject::print( int spaces )const
    {
        std::cout<<"\n";
        for (Params::const_iterator i = params.begin(); i != params.end(); i++)
        {
            for(int i=0; i<spaces; ++i) std::cout<<"- ";
            std::cout << (*i).first <<": ";
            if( (*i).second.print( spaces + 1 ) )
                std::cout<<"\n";
        }
        return false;
    }
    uint32_t JsonObject::count()const
    {
        return (uint32_t) params.size();
    }
    JsonObject::~JsonObject()
    {}
    JsonObject::JsonObject()
    {}

}


namespace kege
{

    Json JsonParser::parseValue( std::stack< Json >& stack, kege::JsonLexer::Token& token, kege::JsonLexer& lexer )
    {
        std::string name;
        std::string value;
        while ( token != kege::JsonLexer::END_STREAM )
        {
            switch ( token )
            {
                case kege::JsonLexer::FLOAT:
                case kege::JsonLexer::INTEGER:
                case kege::JsonLexer::STRING:
                {
                    value = lexer.getLexeme();
                    token = lexer.getNextToken();
                    switch ( token )
                    {
                        case kege::JsonLexer::COMMA:
                        {
                            token = lexer.getNextToken();
                            stack.top().insert( name, new JsonString( value ) );
                        }
                        break;

                        case kege::JsonLexer::CLOSE_CURLY_BRACKET:
                        case kege::JsonLexer::CLOSE_SQUARE_BRACKET:
                        {
                            stack.top().insert( name, new JsonString( value ) );
                        }
                        break;

                        case kege::JsonLexer::COLON:
                        {
                            token = lexer.getNextToken();
                            name = value;
                        }
                        break;

                        default:
                        {
                            throw std::runtime_error( "[ JsonParser ] : unexpected token #" + std::to_string( lexer.getCurrLine() ) + " (" + name + " : " + value + ")" );
                        }
                        break;
                    }
                    break;
                }

                case kege::JsonLexer::OPEN_CURLY_BRACKET:
                {
                    token = lexer.getNextToken();
                    if ( !stack.empty() )
                    {
                        JsonValue* json = new JsonObject;
                        stack.top().insert( name, json );
                        stack.push( json );
                    }
                    else
                    {
                        stack.push( new JsonObject );
                    }
                    break;
                }

                case kege::JsonLexer::OPEN_SQUARE_BRACKET:
                {
                    token = lexer.getNextToken();
                    if ( !stack.empty() )
                    {
                        JsonArray* json = new JsonArray;
                        stack.top().insert( name, json );
                        stack.push( json );
                    }
                    else
                    {
                        stack.push( new JsonArray );
                    }
                    break;
                }

                case kege::JsonLexer::CLOSE_SQUARE_BRACKET:
                case kege::JsonLexer::CLOSE_CURLY_BRACKET:
                {
                    token = lexer.getNextToken();
                    Json json = stack.top();
                    stack.pop();

                    if ( stack.empty() )
                    {
                        return json;
                    }
                    break;
                }

                default:
                {
                    token = lexer.getNextToken();
                    break;
                }
            }
        }
        return nullptr;
    }

    Json JsonParser::parse( const char* jsontext )
    {
        std::stack< Json > stack;
        kege::JsonLexer lexer( jsontext );
        try 
        {
            kege::JsonLexer::Token token = lexer.getNextToken();
            return parseValue( stack, token, lexer );
        }
        catch ( const std::runtime_error& erro )
        {
            std::cout << erro.what() <<"\n";
            return {};
        }
    }

    Json JsonParser::load( const char* filename )
    {
        std::vector< char > buffer;
        if ( kege::JsonLexer::load( buffer, filename ) )
        {
            return parse( buffer.data() );
        }
        return {};
    }

}
